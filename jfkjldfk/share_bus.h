//
//  share_bus.hpp
//  jfkjldfk
//
//  Created by Jiyang on 16/3/8.
//  Copyright © 2016年 Jiyang. All rights reserved.
//

#ifndef share_bus_hpp
#define share_bus_hpp

#include <stdio.h>
#include "Bus_request.h"
#include "bus_interface.h"
#include <systemc.h>
#include <queue>
#include <vector>
#include <iostream>

using namespace std;

class share_bus:public sc_module,public bus_slave_if, public bus_master_if{
public:
    bool rcv_Response;
    sc_event    mst_ack,slv_ack,
                bus_request_event,slv_request_event,
                mst_ready,data_ready,slv_ready,
                end_transmission;
    
    unsigned int robin_index = 0;
    
    vector<queue<Request*>*> robin_vec;
    
    Request *current_request;
    
    //queue<Request*> rqt_queue;
    
    unsigned int cnt;
    unsigned int bus_data;
    SC_HAS_PROCESS(share_bus);
    share_bus(sc_module_name name):sc_module(name){
        
        robin_vec.push_back(new queue<Request*>);
        robin_vec.push_back(new queue<Request*>);
        
        
        rcv_Response =false;
        cnt = 0;
        SC_THREAD(arbiter);
    }
    
    
    bool MstBusRequest(unsigned mstId, bool rdnwr, unsigned addr, unsigned len);
    void SlvListen(unsigned &reqAddr, unsigned &reqRdnwr, unsigned &reqLen);
    void SlvAcknowledge();
    void MstReadData(unsigned &data);
    void SlvSendReadData(unsigned data);
    void MstWriteData(unsigned data);
    void SlvReceiveWriteData(unsigned &data);
    void arbiter();
    
    
};

#endif /* share_bus_hpp */
