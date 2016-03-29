//
//  Bus_request.h
//  jfkjldfk
//
//  Created by Jiyang on 16/3/8.
//  Copyright © 2016年 Jiyang. All rights reserved.
//

#ifndef Bus_request_h
#define Bus_request_h


//------------------------------------
#define LOOPS 1000
#define SIZE 5
#define ADDR_begin 4096
#define ADDR_end 4096+107

#define HW_ADDR_a 1024
#define HW_ADDR_b 1025
#define HW_ADDR_c 1026
#define HW_ADDR_op 1027
#define HW_ADDR_done 1028


#define TIME_UNIT 5

#define NUM_BLOCKS 64
#define BLOCK_SIZE 64
#define INPUT1_ADDR 10000
#define INPUT2_ADDR 26384
#define SAD_OUTPUT_ADDR 42768
//------------------------------------


class Request{
public:
    unsigned int id;
    unsigned int addr;
    unsigned int len;
    bool rnw; //read not write
    
    Request();
    
    Request(unsigned int user_id,
            bool rnw,
            unsigned int addr,
            unsigned int len
            ){
        this->id = user_id;
        this->addr = addr;
        this->rnw = rnw;
        this->len = len;
    }
    
};

#endif /* Bus_request_h */
