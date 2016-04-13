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
#include "share_bus.h"


class Memory:public sc_module{
public:
    sc_port<bus_slave_if>  slv;
    SC_HAS_PROCESS(Memory);
    std::vector<unsigned int> image ;
    unsigned distance;
    unsigned angle;
    
    Memory(sc_module_name name):sc_module(name){
        image.assign(IMG_HEIGHT*IMG_WIDTH,0);
        SC_THREAD(listenThread) ;
    }
    void listenThread();
    
    

    
    
    //bool hasImage = false;
};


#endif /* Memory_hpp */
