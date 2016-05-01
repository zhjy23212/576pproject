//
//  sw_main.cpp
//  jfkjldfk
//
//  Created by Jiyang on 16/4/11.
//  Copyright © 2016年 Jiyang. All rights reserved.
//

#include "software.h"
void sw::main(){
    unsigned int  done_sig =0;
    unsigned int dist = 0;
    bool ack = mst->MstBusRequest(this->id, false, CCD_capture_addr, 1);
    if(ack){
        mst->MstWriteData(1);
    }
    
    do{
        ack = mst->MstBusRequest(this->id, true, CCD_done, 1);
        if (ack) {
            mst->MstReadData(done_sig);
        }
    }while (done_sig == 0);
    
    ack = mst->MstBusRequest(this->id, true, MEM_DIST, 1);
    if (ack) {
        mst->MstReadData(dist);
    }
    
    //not implemented these part of func
    
    
    if (dist<=3) {
        ack = mst->MstBusRequest(this->id, false, dsp_need_process, 1);
        if(ack){
            mst->MstWriteData(0);
        }
    }else{
        ack = mst->MstBusRequest(this->id, false, dsp_need_process, 1);
        if(ack){
            mst->MstWriteData(1);
        }
    }
    
    ack = mst->MstBusRequest(this->id, false, LCD_ON,  1);
    if (ack) {
        mst->MstWriteData(1);
    }
    
    do{
        ack = mst->MstBusRequest(this->id, true, DSP_DONE, 1);
        if (ack) {
            mst->MstReadData(done_sig);
        }
    }while (done_sig == 0);

    
    
}