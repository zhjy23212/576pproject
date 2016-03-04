//
//  hwsw.cpp
//  HW2
//
//  Created by Jiyang on 16/2/18.
//  Copyright © 2016 Jiyang. All rights reserved.
//


//----------------------------------------------------------------
#include <systemc.h>
#include <queue>
#include <vector>
#include <iostream>
#include "bus_interface.h"
//----------------------------------------------------------------

using namespace std;


//----------------------------------------------------------------

#define LOOPS 1000
#define SIZE 5
#define ADDR_A 4096
#define ADDR_B 8192
#define ADDR_C 16384
#define HW_ADDR 1024
//----------------------------------------------------------------


unsigned globalId = 0;



//----------------------------------------------------------------
class Request{
public:
    unsigned int id;
    unsigned int addr;
    unsigned int len;
    bool rnw; //read not write
    
    //Request();
    
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
//----------------------------------------------------------------


class Memory: public sc_module{
public:
    sc_port<bus_slave_if>  ioPort;
    unsigned int  addr, Rdnwr,len;
    
    unsigned int a[36] = {0,0,0,0,0,0,0,0,9,4,7,9,0,12,14,15,16,11,0,2,3,4,5,6,0,4,3,2,1,2,0,2,7,6,4,9};
    unsigned int b[36] = {0,0,0,0,0,0,0,0,9,4,7,9,0,12,14,15,16,11,0,2,3,4,5,6,0,4,3,2,1,2,0,2,7,6,4,9};
    unsigned int c[36];
    
    SC_HAS_PROCESS(Memory);
    Memory(sc_module_name name) : sc_module(name) {
        SC_THREAD(listenThread);
    }
    
    void listenThread(){
        ioPort->SlvListen(addr, Rdnwr, len);
        if (addr>=ADDR_A&&addr<=ADDR_A+35) {
            ioPort->SlvAcknowledge();
            if (Rdnwr) {
                for (int i = 0; i<len; i++) {
                    ioPort->SlvSendReadData(a[addr-ADDR_A+i]);
                }
                ioPort->end_transmission.notify();
            }else{
                for (int i = 0; i<len; i++) {
                    ioPort->SlvReceiveWriteData(a[addr-ADDR_A+i]);
                }
                ioPort->end_transmission.notify();
            }
        }else if (addr>=ADDR_B&&addr<=ADDR_B+35){
            if (Rdnwr) {
                for (int i = 0; i<len; i++) {
                    ioPort->SlvSendReadData(b[addr-ADDR_A+i]);
                }
                ioPort->end_transmission.notify();
            }else{
                for (int i = 0; i<len; i++) {
                    ioPort->SlvReceiveWriteData(b[addr-ADDR_A+i]);
                }
                ioPort->end_transmission.notify();
            }
        }else if (addr>=ADDR_C&&addr<=ADDR_C+35){
            if (Rdnwr) {
                for (int i = 0; i<len; i++) {
                    ioPort->SlvSendReadData(c[addr-ADDR_A+i]);
                }
                ioPort->end_transmission.notify();
            }else{
                for (int i = 0; i<len; i++) {
                    ioPort->SlvReceiveWriteData(c[addr-ADDR_A+i]);
                }
                ioPort->end_transmission.notify();
            }
        }
    }
    
    
    
};
//----------------------------------------------------------------



class hw_accelerator:public sc_module{
public:
    // TODO: memory address mapping
    unsigned id;
    sc_port<bus_master_if> mst;
    sc_port<bus_slave_if> slv;
    hw_accelerator(sc_module_name name) : sc_module(name){
        id = globalId++;
        //Thread
    }
    
    SC_HAS_PROCESS(hw_accelerator);
    
    
    
};
//----------------------------------------------------------------

class share_bus:public sc_module,public bus_slave_if, public bus_master_if{
public:
    bool rcv_Response =false;
    sc_event    mst_ack,slv_ack,
                bus_request_event,slv_request_event,
                mst_ready,data_ready,slv_ready
                //end_transmission
    ;
    
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
    
    void MstWriteData(unsigned data){
        wait(slv_ready);
        bus_data = data;
        data_ready.notify();
    }
    
    void SlvListen(unsigned &reqAddr, unsigned &reqRdnwr, unsigned &reqLen){       //to do!
        wait(slv_request_event);
        reqAddr =  current_request.addr;
        reqRdnwr  = current_request.rnw;
        reqLen = current_request.len;
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
    
    void SlvReceiveWriteData(unsigned &data){
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
            wait(200, SC_NS, slv_ack);
            if (rcv_Response) {
                mst_ack.notify();
                wait(end_transmission);
            }else{
                mst_ack.notify();
            }
            
        }
    }
};

//----------------------------------------------------------------

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
                for(j=1;j<=SIZE;j++)      // Total Cycles: 52000,   Execs: 5000,  Iters: 5
                    c[i][j] = 0;
            sw_mst->MstBusRequest(id, false, HW_ADDR, 1);
            
            for(i=1;i<=SIZE;i++)        // Total Cycles: 757900,  Execs: 1000,  Iters: 5
                for(j=1;j<=SIZE;j++)      // Total Cycles: 752000,  Execs: 5000,  Iters: 5
                    for(k=1;k<=SIZE;k++)    // Total Cycles: 722500,  Execs: 25000, Iters: 5
                        c[i][j] += a[i][k] * b[k][j];
        }
        
//        return 0;
    }

};