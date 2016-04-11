//
//  sw_main.cpp
//  jfkjldfk
//
//  Created by Jiyang on 16/4/11.
//  Copyright © 2016年 Jiyang. All rights reserved.
//

#include "software.h"
void sw::main(){
    unsigned int  CCD_done_sig =0,dsp_done_sig = 0;
    unsigned int dist = 0;
    bool ack = mst->MstBusRequest(this->id, false, CCD_capture_addr, 1);
    if(ack){
        mst->MstWriteData(1);
    }
    do{
        ack = mst->MstBusRequest(this->id, true, CCD_done, 1);
        if (ack) {
            mst->MstReadData(CCD_done_sig);
        }
    }while (CCD_done_sig == 0);
    
    ack = MstBusRequest(this->id, true, MEM_DIST, 1);
    if (ack) {
        mst->MstReadData(dist);
    }
    
    if (dist<=5) {
        ack = MstBusRequest(this->id, false, dsp_need_process, 1);
        if(ack){
            mst->MstWriteData(0);
        }
    }else{
        ack = MstBusRequest(this->id, false, dsp_need_process, 1);
        if(ack){
            mst->MstWriteData(1);
        }
    }
    
    do{
        ack = mst->MstBusRequest(this->id, true, DSP_DONE, 1);
        if (ack) {
            mst->MstReadData(dsp_done_sig);
        }
    }while (dsp_done_sig == 0);
    
    ack = MstBusRequest(this->id, false, LCD_ON,  1);
    if (ack) {
        mst->MstWriteData(1);
    }
    
}