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
        robin_vec[mstId]->push(new Request(mstId, rdnwr, addr, len));
        
        //rqt_queue.push(new Request(mstId, rdnwr, addr, len));
        bus_request_event.notify();
        do{
            wait(mst_ack);
        }while (current_request->id !=  mstId);
        wait(3*TIME_UNIT,SC_NS);
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
        wait(1*TIME_UNIT,SC_NS);
        rcv_Response = true;
        slv_ack.notify();
    }
    
    void share_bus:: MstReadData(unsigned &data){
        mst_ready.notify();
        wait(data_ready);
        data = bus_data;
        cnt++;
        if(cnt==current_request->len){
            end_transmission.notify();
            cnt=0;
        }
    }
    
    
    void share_bus:: SlvSendReadData(unsigned data){
        //        cout<<"mst_ready SlvSendReadData"<<endl;
        wait(mst_ready);
        wait(1*TIME_UNIT,SC_NS);
        bus_data = data;
        data_ready.notify();
    }
    
    void share_bus:: MstWriteData(unsigned data){
        wait(1*TIME_UNIT,SC_NS);
        bus_data = data;
        mst_ready.notify();
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
            
            if (robin_vec[0]->empty()&&robin_vec[1]->empty()) {
                wait(bus_request_event);
            }
            /*
            if (rqt_queue.empty()) {
                wait(bus_request_event);
            }
            */
            
            while (1) {
                if (robin_index>=robin_vec.size()) {
                    robin_index = 0 ;
                }
                
                if(!robin_vec[robin_index]->empty()){
                    current_request = robin_vec[robin_index]->front();
                    robin_vec[robin_index]->pop();
                    robin_index++;
                    break;
                }else{
                    robin_index++;
                }
            }
            
            //current_request  =  rqt_queue.front();
            //rqt_queue.pop();
            cnt = 0;
            slv_request_event.notify();
            rcv_Response = false;
            
            
            wait(40, SC_NS, slv_ack);  //twice of acknowledge time for timeout
            
            if (rcv_Response) {
                mst_ack.notify();
                //                cout<<"wait end_trans"<<endl;
                wait(end_transmission);
                wait(1*TIME_UNIT,SC_NS);
            }else{
                mst_ack.notify();
            }
            
        }
    }
    
    

