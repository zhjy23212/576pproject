//
//  display_LCD.hpp
//  jfkjldfk
//
//  Created by Jiyang on 16/4/11.
//  Copyright © 2016年 Jiyang. All rights reserved.
//

#ifndef display_LCD_hpp
#define display_LCD_hpp

#include <stdio.h>
#include "bus_interface.h"
#include "Bus_request.h"
#include "simple_interface.h"

class display:public sc_module{
public:
    std::vector<std::vector<unsigned int>> display_img = std::vector<std::vector<unsigned int>>(IMG_HEIGHT,std::vector<unsigned int>(IMG_WIDTH,0));
    unsigned int  done = 0;
    sc_port<bus_slave_if> slv;
    sc_port<simple_if> get_inst;
    unsigned int zoom = 0, move = 0;
    unsigned int on_lcd_signal = 0;
    SC_HAS_PROCESS(display);
    sc_event display_event;
    display(sc_module_name name):sc_module(name){
        
        SC_THREAD(listenThread);
    }
    
    
    void listenThread(); //keep waiting and execute larger operation everytime
    void processThread();
    bool Write ( unsigned  int addr,  unsigned  int data )  ;
    bool Read ( unsigned  int addr,  unsigned int & data )  ;
    
};

#endif /* display_LCD_hpp */
