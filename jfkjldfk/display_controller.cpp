//
//  display_controller.cpp
//  jfkjldfk
//
//  Created by Jiyang on 16/4/11.
//  Copyright © 2016年 Jiyang. All rights reserved.
//

#include "display_controller.hpp"
bool display_controller:: Read(unsigned int addr, unsigned int &data){
    if(addr == 0){
        data = zoom;
    }else if(addr == 1){
        data = move;
    }else{
        return false;
    }
    return true;
}

bool display_controller:: Write(unsigned int addr, unsigned int dataint){
    if (addr == 0){
        zoom = dataint;
    }else if(addr == 1){
        move = dataint;
    }else{
        return false;
    }
    return true;
}