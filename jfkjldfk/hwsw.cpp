//
//  hwsw.cpp
//  jfkjldfk
//
//  Created by Jiyang on 16/2/18.
//  Copyright © 2016 Jiyang. All rights reserved.
//

#include <systemc.h>
#include <queue>
#include <vector>
#include <iostream>
#include "bus_interface.h"

using namespace std;
#define LOOPS 1000
#define SIZE 5
#define ADDR_A 0
#define ADDR_B 36
#define ADDR_C 72

unsigned globalId = 0;

class Request{
public:
    unsigned int id;
    unsigned int addr;
    unsigned int len;
    bool rnw; //read not write
    
    sc_event transfer_done;
    Request();
    
    Request(unsigned int user_id,
            bool rnw,
            unsigned int addr,
            unsigned int len
            ){
        this->id = user_id;
        this->addr = addr;
        this->rnw = rnw;
        this->len = len;
    }
    
    Request(const Request &b){
        id = b.id;
        rnw = b.rnw;
        addr = b.addr;
        len = b.len;
    }
    
    Request operator=(const Request &b){
        return b;
    }
   
};


bool operator<( Request rqs1, Request rqs2 ){
    return rqs1.id > rqs2.id;
    // return "true" if rqs1's priority is higher than rqs2(the smaller the higher)
}


class Memory: public sc_module{
public:
    sc_port<bus_slave_if>  ioPort;
    
    unsigned int a[36] = {0,0,0,0,0,0,0,0,9,4,7,9,0,12,14,15,16,11,0,2,3,4,5,6,0,4,3,2,1,2,0,2,7,6,4,9}; // ADDR range is 0 - 35
    unsigned int b[36] = {0,0,0,0,0,0,0,0,9,4,7,9,0,12,14,15,16,11,0,2,3,4,5,6,0,4,3,2,1,2,0,2,7,6,4,9}; // ADDR range is 36 - 71
    unsigned int c[36]; //AddR range is 72 - 107
    Memory(sc_module_name name) : sc_module(name) {
        
    }
};


class hw_accelerator:public sc_module{
public:
    unsigned id;
    sc_port<bus_master_if> mst;
    sc_port<bus_slave_if> slv;
    hw_accelerator(sc_module_name name) : sc_module(name){
        id = globalId++;
    }
    
};


class share_bus:public sc_module,public bus_slave_if, public bus_master_if{
public:
    bool rcv_Response =false;
    sc_event mst_ack,slv_ack, bus_request_event,slv_request_event,mst_ready,data_ready,end_transmission,slv_ready;
    
    Request current_request;
    
    priority_queue<Request> queue ;
    
    unsigned int bus_data;
    
    bool MstBusRequest(unsigned mstId, bool rdnwr, unsigned addr, unsigned len){
        queue.push(Request(mstId, rdnwr, addr, len));
        bus_request_event.notify();
        do{
            wait(mst_ack);
            
        }while (current_request.id ==  mstId);
        return rcv_Response;
    }
    
    
    
    void MstReadData(unsigned &data){
        mst_ready.notify();
        wait(data_ready);
        data = bus_data;
    }
    
    void MstWriteData(unsigned data){   //to do
        wait(slv_ready);
        bus_data = data;
        data_ready.notify();
    }
    
    void SlvListen(unsigned &reqAddr, unsigned &reqRdnwr, unsigned &reqLen){       //to do!
        
    }
    
    void SlvAcknowledge(){
        rcv_Response = true;
        slv_ack.notify();
    }
    
    
    void SlvSendReadData(unsigned data){
        wait(mst_ready);
        bus_data = data;
        data_ready.notify();
    }
    
    void SlvReceiveWriteData(unsigned &data){       //to do
        slv_ready.notify();
        wait(data_ready);
        data  = bus_data;
        
    }
    
    void arbiter(){
        while (1) {
            if (queue.empty()) {
                wait(bus_request_event);
            }
            current_request =  queue.top();
            queue.pop();
            slv_request_event.notify();
            rcv_Response = false;
            wait(50, SC_NS, slv_ack);
            if (rcv_Response) {
                mst_ack.notify();
                wait(end_transmission);
            }else{
                mst_ack.notify();
            }
            
        }
    }
};



class sw_component:public sc_module{
public:
    sc_port<bus_master_if> sw_mst;
    unsigned id;
    SC_HAS_PROCESS(sw_component);
    sw_component(sc_module_name name):sc_module(name){
        id = globalId++;
        SC_METHOD(main);
    }
    
    void main() // Total Cycles: 819343
    {
        int n;
        int i,j,k;
        
        for (n = 0 ; n < LOOPS ; n++) // Total Cycles: 818600,  Execs: 1,     Iters: 1000
        {
            for(i=1;i<=SIZE;i++)        // Total Cycles: 57900,   Execs: 1000,  Iters: 5
                for(j=1;j<=SIZE;j++)
//                {
//                    bool  ack = sw_mst->MstBusRequest(id, false, ADDR_C+(SIZE+1)*i+j+1, 25);
//                    if(ack){
//                        sw_mst->MstWriteData(0);
//                        
//                    }
//                }// Total Cycles: 52000,   Execs: 5000,  Iters: 5
                    c[i][j] = 0;
            
            
            for(i=1;i<=SIZE;i++)        // Total Cycles: 757900,  Execs: 1000,  Iters: 5
                for(j=1;j<=SIZE;j++)      // Total Cycles: 752000,  Execs: 5000,  Iters: 5
                    for(k=1;k<=SIZE;k++)    // Total Cycles: 722500,  Execs: 25000, Iters: 5
                        c[i][j] += a[i][k] * b[k][j];
        }
        
//        return 0;
    }

};