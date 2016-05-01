//
//  display_LCD.cpp
//
//  This program simulate the functionality screen in an actual camara system
//  The process for this is when get LCD-ON signal, it start retrieving the zoom and move parameter from controller
//  With the processed image,this writes back a txt file to simulate the output of screen
//  We can examine it by reading the data into MATLAB and output it.
//
//
//  Created by Jiyang on 16/4/11.
//  Copyright © 2016 Jiyang. All rights reserved.
//

#include "display_LCD.hpp"

void display:: listenThread(){
    unsigned int addr, rdNwr,len;
    while (1) {
        slv->SlvListen(addr, rdNwr, len);
        if(addr == LCD_ON){
            slv->SlvAcknowledge();
            slv->SlvReceiveWriteData(on_lcd_signal);
            std::cout<<"LCD ON"<<endl;
        }
        
        else if (LCD_DATA_TRANS_FIN == addr) {
            slv->SlvAcknowledge();
            slv->SlvReceiveWriteData(done);
            std::cout<<"LCD data all got"<<endl;
        }
        
        
        
        else if((addr>=LCD_IMAGE_START_ADDR && addr<= LCD_IMAGE_END_ADDR) && on_lcd_signal == 1){
            slv->SlvAcknowledge();
            if(!rdNwr){
                for (int i = 0; i < len; i++){
                    slv->SlvReceiveWriteData(display_img[(addr - LCD_IMAGE_START_ADDR)/IMG_WIDTH][i]);
                }
            }
        }
        
        
        
        if (done == 1) {
            processThread();   //write the screen file
            done =0;
        }
    }
    
}

void display:: processThread(){
    get_inst->Read(0, zoom);
    get_inst->Read(1, move_down);
    get_inst->Read(2, move_right);
    // to obtain the controller instruction
    std::vector<std::vector<unsigned int>> temp(IMG_HEIGHT,std::vector<unsigned int>(IMG_WIDTH,0));
    std::vector<std::vector<unsigned int>> zoom_array(IMG_HEIGHT/2,std::vector<unsigned int>(IMG_WIDTH/2,0));
    std::__1::ofstream fileout("/Users/yanglizhuo/Desktop/Product/576project/576project/screen.txt");
    if(zoom == 0){
        for(int i = 0; i < IMG_HEIGHT; i++){
            for (int j = 0; j < IMG_WIDTH; j++) {
                
                fileout<<display_img[i][j];
                if (j!=IMG_WIDTH -1) {
                    fileout<<" ";
                }
            }
            if (i != IMG_HEIGHT-1) {
                fileout<<endl;
            }
        }
    }else{
        unsigned int start_row = 0, start_col = 0;
        if (move_down<=IMG_HEIGHT/2) {
            start_row += move_down;
        }else{
            start_row = IMG_HEIGHT/2;
        }
        
        if(move_right <= IMG_WIDTH/2){
            start_col += move_right;
        }else{
            start_col = IMG_WIDTH/2;
        }
        for(int i = 0; i < zoom_array.size(); i++){
            for (int j = 0; j<zoom_array[0].size(); j++) {
                zoom_array[i][j]  = display_img[start_row + i][start_col + j];
            }
        }
        
        for (int  i = 0; i < temp.size(); i++) {
            for (int j = 0; j < temp[0].size(); j++) {
                if (i % 2 == 0) {
                    if (j % 2 == 0) {
                        temp[i][j] = zoom_array[i/2][j/2];
                    }else{
                        temp[i][j] = temp[i][j-1];
                    }
                }else{
                    temp[i][j] = temp[i-1][j];
                }
            }
        }
        //temp is the array to write to file when zoomed
        for(int i = 0; i < temp.size(); i++){
            for (int j = 0; j < temp[0].size(); j++) {
                
                fileout<<temp[i][j];
                if(j != temp[0].size()-1){
                    fileout<<" ";
                }
            }
            if(i !=temp.size()-1){
                fileout<<std::endl;
            }
        }
    
    }
    
    fileout<<flush;
    fileout.close();
    
}