//
//  share_bus.cpp
//  jfkjldfk
//
//  Created by Jiyang on 16/3/8.
//  Copyright © 2016年 Jiyang. All rights reserved.
//

#include "share_bus.h"

using namespace::std;

    
    bool share_bus:: MstBusRequest(unsigned mstId, bool rdnwr, unsigned addr, unsigned len){
        rqt_queue.push(new Request(mstId, rdnwr, addr, len));
        //cout<<"id "<<mstId<<" DO_READ "<<rdnwr<<" ADDR "<< addr<<" LEN "<<endl;
        bus_request_event.notify();
        do{
            
            wait(mst_ack);
        }while (current_request->id !=  mstId);
        return rcv_Response;
    }
    
    void share_bus:: SlvListen(unsigned &reqAddr, unsigned &reqRdnwr, unsigned &reqLen){
        //        cout<<"slv_request_event"<<endl;
        wait(slv_request_event);
        reqAddr =  current_request->addr;
        reqRdnwr  = current_request->rnw;
        reqLen = current_request->len;
    }
    
    void share_bus:: SlvAcknowledge(){
        rcv_Response = true;
        slv_ack.notify();
    }
    
    void share_bus:: MstReadData(unsigned &data){
        mst_ready.notify();
        //        cout<<"data ready in MSTREADDATA"<<endl;
        wait(data_ready);
        data = bus_data;
        cnt++;
        //        cout << cnt << current_request->len << endl;
        if(cnt==current_request->len){
            end_transmission.notify();
            cnt=0;
        }
    }
    
    
    void share_bus:: SlvSendReadData(unsigned data){
        //        cout<<"mst_ready SlvSendReadData"<<endl;
        wait(mst_ready);
        bus_data = data;
        data_ready.notify();
    }
    
    void share_bus:: MstWriteData(unsigned data){
        bus_data = data;
        mst_ready.notify();
        //        cout<<"slv_ready in  MstWriteData"<<endl;
        wait(slv_ready);
    }
    
    void share_bus:: SlvReceiveWriteData(unsigned &data){
        //        cout<<"mst_ready in SlvReceiveWriteData"<<endl;
        wait(mst_ready);
        slv_ready.notify();
        data  = bus_data;
        cnt++;
        if(cnt==current_request->len){
            end_transmission.notify();
            cnt=0;
        }
    }
    
    
    
    void share_bus:: arbiter(){
        while (1) {
            if (rqt_queue.empty()) {
                //                cout<<"waiting bus_request_event"<<endl;
                wait(bus_request_event);
            }
            current_request  =  rqt_queue.front();
            rqt_queue.pop();
            cnt = 0;
            slv_request_event.notify();
            rcv_Response = false;
            
            
            wait(200, SC_NS, slv_ack);
            //            cout<<"slv_ack"<<endl;
            //wait(slv_ack);
            
            //            cout << "get slv_ack" << endl;
            if (rcv_Response) {
                mst_ack.notify();
                //                cout<<"wait end_trans"<<endl;
                wait(end_transmission);
                //                cout << "end trans" << endl;
                //cout<<sc_time_stamp()<<endl;
            }else{
                mst_ack.notify();
            }
            
        }
    }
    
    

