//
//  sw_main.hpp
//  jfkjldfk
//
//  Created by Jiyang on 16/4/11.
//  Copyright © 2016年 Jiyang. All rights reserved.
//

#ifndef sw_main_hpp
#define sw_main_hpp

#include <stdio.h>
#include "Bus_request.h"
#include "bus_interface.h"
class sw:public sc_module,public bus_master_if{
public:
    sc_port<bus_master_if> mst;
    unsigned int id;
    unsigned int motion;
    unsigned int direction;
    SC_HAS_PROCESS(sw);
    sw(sc_module_name name, unsigned int sw_id):sc_module(name){
        id = sw_id;
        SC_THREAD(main);
    }
    void main();
};


#endif /* sw_main_hpp */
