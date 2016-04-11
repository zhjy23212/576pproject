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
    
    bool Read(unsigned int addr, unsigned int &data){
        if(addr == 0){
            data = dist;
        }else if(addr == 1){
            data = angle;
        }else{
            return false;
        }
        return true;
    }
    
    bool Write(unsigned int addr, unsigned int dataint){
        if (addr == 0){
            dist = dataint;
        }else if(addr == 1){
            angle = dataint;
        }else{
            return false;
        }
        return true;
    }

    
};



#endif /* Motion_detect_hpp */
