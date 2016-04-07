//
//  Memory.hpp
//  jfkjldfk
//
//  Created by Jiyang on 4/6/16.
//  Copyright © 2016 Jiyang. All rights reserved.
//

#ifndef Memory_hpp
#define Memory_hpp

#include <stdio.h>
#include "Bus_request.h"
#include <systemc.h>
#include "bus_interface.h"
#include <vector>


class Memory:public sc_module,public bus_slave_if{
public:
    sc_port<bus_slave_if>  slv;
    SC_HAS_PROCESS(Memory);
    
    Memory(sc_module_name name):sc_module(name){
        image.reserve(IMG_HEIGHT*IMG_WIDTH);
        listenThread();
    }
    void listenThread();
    
    
private:
    std::vector<unsigned int> image ;
    unsigned distance;
    unsigned angle;
    
    //bool hasImage = false;
};


#endif /* Memory_hpp */
