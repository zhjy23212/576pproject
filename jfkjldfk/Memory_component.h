//
//  Memory_component.hpp
//  jfkjldfk
//
//  Created by Jiyang on 16/3/8.
//  Copyright © 2016年 Jiyang. All rights reserved.
//

#ifndef Memory_component_h
#define Memory_component_h

#include <systemc.h>
#include "Bus_request.h"
#include "bus_interface.h"
#include <queue>
#include <vector>
#include <iostream>

using namespace std;

//----------------------------------------------------------------


class Memory: public sc_module{
public:
    sc_port<bus_slave_if>  ioPort;
    unsigned int  addr, Rdnwr,len;
    vector<unsigned int>  mem_data;
    vector<unsigned int>  c;
    vector<unsigned int>  a ;
    vector<unsigned int>  b ;
    
    
    SC_HAS_PROCESS(Memory);
    Memory(sc_module_name name) : sc_module(name) {
        
        a = {0,0,0,0,0,0,0,0,9,4,7,9,0,12,14,15,16,11,0,2,3,4,5,6,0,4,3,2,1,2,0,2,7,6,4,9};
        b = {0,0,0,0,0,0,0,0,9,4,7,9,0,12,14,15,16,11,0,2,3,4,5,6,0,4,3,2,1,2,0,2,7,6,4,9};
        c.assign(36, 0);
        mem_data = a;
        mem_data.insert(mem_data.end(), b.begin(), b.end());
        mem_data.insert(mem_data.end(), c.begin(),c.end());
        SC_THREAD(listenThread);
    }
    
    void listenThread();
};




#endif /* Memory_component_hpp */
