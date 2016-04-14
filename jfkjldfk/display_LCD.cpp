//
//  display_LCD.cpp
//  jfkjldfk
//
//  Created by Jiyang on 16/4/11.
//  Copyright © 2016年 Jiyang. All rights reserved.
//

#include "display_LCD.hpp"

void display:: listenThread(){
    unsigned int addr, rdNwr,len;
    slv->SlvListen(addr, rdNwr, len);
    if(addr == LCD_ON){
        slv->SlvAcknowledge();
        slv->SlvReceiveWriteData(on_lcd_signal);
        std::cout<<"LCD ON"<<endl;
    }
    
    if (LCD_DATA_TRANS_FIN == addr) {
        slv->SlvAcknowledge();
        slv->SlvReceiveWriteData(done);
        std::cout<<"LCD data all got"<<endl;
    }
    
    
    
    if((addr>=LCD_IMAGE_START_ADDR && addr<= LCD_IMAGE_END_ADDR) && on_lcd_signal == 1){
        slv->SlvAcknowledge();
        if(!rdNwr){
            for (int i = 0; i < len; i++){
                slv->SlvReceiveWriteData(display_img[(addr - LCD_IMAGE_START_ADDR)/IMG_WIDTH][i]);
            }
        }
    }
    
    if (done == 1) {
        processThread();   //write the screen file
    }
}

void display:: processThread(){
    get_inst->Read(0, zoom);
    get_inst->Read(1, move);
    
    
}