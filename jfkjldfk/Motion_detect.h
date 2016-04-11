//
//  Motion_detect.hpp
//  jfkjldfk
//
//  Created by Jiyang on 16/4/11.
//  Copyright © 2016年 Jiyang. All rights reserved.
//

#ifndef Motion_detect_hpp
#define Motion_detect_hpp

#include <stdio.h>
#include "Bus_request.h"
#include "simple_interface.h"

class motion_detector:public sc_module,public simple_if{
public:
    unsigned int dist;
    unsigned int angle;
    motion_detector(sc_module_name name, unsigned int dist, unsigned int angle):sc_module(name){
        this->dist = dist;
        this->angle = angle;
    }
    
    bool Write ( unsigned  int addr,  unsigned  int data )  ;
    bool Read ( unsigned  int addr,  unsigned int & data )  ;
};



#endif /* Motion_detect_hpp */
