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
        data = move_down;
    }else if (addr == 2){
        data = move_right;
    }else{
        return false;
    }
    return true;
}

bool display_controller:: Write(unsigned int addr, unsigned int dataint){
    if (addr == 0){
        zoom = dataint;
    }else if(addr == 1){
        move_down = dataint;
    }else if (addr == 2){
        move_right = dataint;
    }else{
        return false;
    }
    return true;
}