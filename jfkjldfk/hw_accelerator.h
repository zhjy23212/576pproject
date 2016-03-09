//
//  hw_accelerator.h
//  jfkjldfk
//
//  Created by Jiyang on 16/3/8.
//  Copyright © 2016年 Jiyang. All rights reserved.
//

#ifndef hw_accelerator_h
#define hw_accelerator_h

#include "bus_interface.h"
#include "Bus_request.h"
#include <systemc.h>
#include <queue>
#include <vector>
#include <iostream>

using namespace std;


class hw_accelerator:public sc_module{
public:
    
    unsigned id;
    sc_port<bus_master_if> mst;
    sc_port<bus_slave_if> slv;
    unsigned int local_addr,local_Rdnwr,local_reqLen;
    unsigned array_a_addr,array_b_addr,array_c_addr;
    sc_event start_signal;
    unsigned int op_type ,local_data;
    unsigned int done;
    
    bool do_burst;    //change to  use burst mode or single
    
    SC_HAS_PROCESS(hw_accelerator);
    hw_accelerator(sc_module_name name, unsigned int id_tag) : sc_module(name){
        id = id_tag;
        do_burst = true;
        done = 0;
        
        SC_THREAD(master_thread);
        SC_THREAD(slave_thread);
        
        
    }
    
    void BurstWrite(unsigned address, vector<unsigned int> vec, unsigned len);
    
    void BurstRead(unsigned address, vector<unsigned int> &vec, unsigned len);
    
    
    void SingleWrite(unsigned address, unsigned int data);
    
    
    void SingleRead(unsigned address, unsigned int &data);
    
    void slave_thread();
    
    
    
    
    
    void master_thread();
    
    
    
};


#endif /* hw_accelerator_h */
