//
//  Bus_request.h
//  jfkjldfk
//
//  Created by Jiyang on 16/3/8.
//  Copyright © 2016年 Jiyang. All rights reserved.
//

#ifndef Bus_request_h
#define Bus_request_h
#include <systemc.h>

//------------------------------------


#define IMG_WIDTH 256
#define IMG_HEIGHT 256


#define TIME_UNIT 5

#define MEM_START_ADDRESS 1024
#define MEM_END_ADDRESS  (1024+IMG_WIDTH*IMG_HEIGHT-1)
#define MEM_DIST (1024+IMG_WIDTH*IMG_HEIGHT)
#define MEM_ANG (1024+IMG_WIDTH*IMG_HEIGHT+1)

#define CCD_capture_addr 512
#define CCD_done 513


#define dsp_need_process 90000
#define DSP_DIST 90001
#define DSP_ANG 90002
#define dsp_image_start 100001
#define dsp_image_end  (dsp_image_start+IMG_WIDTH*IMG_HEIGHT-1)

#define LCD_ON 300000
#define LCD_IMAGE_START_ADDR 300001
#define LCD_IMAGE_END_ADDR  (LCD_IMAGE_START_ADDR+IMG_WIDTH*IMG_HEIGHT-1)


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
