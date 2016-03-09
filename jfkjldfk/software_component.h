//
//  software_component.hpp
//  jfkjldfk
//
//  Created by Jiyang on 16/3/8.
//  Copyright © 2016年 Jiyang. All rights reserved.
//

#ifndef software_component_hpp
#define software_component_hpp

#include <stdio.h>
#include "bus_interface.h"
#include "Bus_request.h"

class sw_component:public sc_module{
public:
    sc_port<bus_master_if> sw_mst;
    unsigned id;
    SC_HAS_PROCESS(sw_component);
    sw_component(sc_module_name name, unsigned int id_tag):sc_module(name){
        id = id_tag;
        SC_THREAD(main);
    }
    
    void main();
    
};


#endif /* software_component_hpp */
