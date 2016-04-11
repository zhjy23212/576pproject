//
//  Motion_detect.cpp
//  jfkjldfk
//
//  Created by Jiyang on 16/4/11.
//  Copyright © 2016年 Jiyang. All rights reserved.
//

#include "Motion_detect.h"
bool motion_detector:: Read(unsigned int addr, unsigned int &data){
    if(addr == 0){
        data = dist;
    }else if(addr == 1){
        data = angle;
    }else{
        return false;
    }
    return true;
}

bool motion_detector:: Write(unsigned int addr, unsigned int dataint){
    if (addr == 0){
        dist = dataint;
    }else if(addr == 1){
        angle = dataint;
    }else{
        return false;
    }
    return true;
}