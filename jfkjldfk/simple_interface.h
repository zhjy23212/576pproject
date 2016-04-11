//
//  simple_interface.h
//  jfkjldfk
//
//  Created by Jiyang on 16/4/11.
//  Copyright © 2016年 Jiyang. All rights reserved.
//

#ifndef simple_interface_h
#define simple_interface_h

#include <systemc.h>

class simple_if  : virtual public sc_interface
{
public:
    virtual bool Write ( unsigned  int addr,  unsigned  int data )  =  0 ;
    virtual bool Read ( unsigned  int addr,  unsigned int & data )  =  0 ;
};

#endif /* simple_interface_h */
