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

class display:public sc_module, public simple_if{
public:
    std::vector<std::vector<unsigned int>> display_img;
    unsigned int  done = 0;
    sc_port<bus_slave_if> slv;
    unsigned int larger = 0;
    
    
    
    
    void listenThread(); //keep waiting and execute larger operation everytime
    
    bool Write ( unsigned  int addr,  unsigned  int data )  ;
    bool Read ( unsigned  int addr,  unsigned int & data )  ;
    
};

#endif /* display_LCD_hpp */
