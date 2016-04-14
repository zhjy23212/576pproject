//
//  display_controller.hpp
//  jfkjldfk
//
//  Created by Jiyang on 16/4/11.
//  Copyright © 2016年 Jiyang. All rights reserved.
//

#ifndef display_controller_hpp
#define display_controller_hpp

#include <stdio.h>
#include "Bus_request.h"
#include "simple_interface.h"

class display_controller:public sc_module, public simple_if{
public:
    
    unsigned int zoom;
    unsigned int move;
    display_controller(sc_module_name name, unsigned int zoom, unsigned int move):sc_module(name){
        this->zoom = zoom;
        this->move = move;
    }
    
    bool Write ( unsigned  int addr,  unsigned  int data )  ;
    bool Read ( unsigned  int addr,  unsigned int & data )  ;
};

#endif /* display_controller_hpp */
