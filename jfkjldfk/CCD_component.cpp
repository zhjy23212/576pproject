//
//  CCD_component.cpp
//  jfkjldfk
//
//  Created by Jiyang on 16/4/11.
//  Copyright © 2016年 Jiyang. All rights reserved.
//

#include "CCD_component.h"

void CCD::ccdmst(){
    while (1) {
        wait(ccdready);
        for (int i=0; i<IMG_HEIGHT; i++) {
            bool ack=false;
            ack=MstBusRequest(ccdid, false, MEM_START_ADDRESS, IMG_WIDTH);
            if (ack==true) {
                for (int j=0; j<IMG_WIDTH; j++) {
                    mstinout->MstWriteData(image[i][j]);
                    if (j+1==IMG_WIDTH) {
                        done=1;
                    }
                }
            }
        }
        bool ack1=false;
        ack1=MstBusRequest(ccdid, false, MEM_DIST, 1);
        if (ack1==true) {
            mstinout->MstWriteData(dist);
        }
        
        bool ack2=false;
        ack2=MstBusRequest(ccdid, false, MEM_ANG, 1);
        if (ack2==true) {
            mstinout->MstWriteData(angle);
        }
    }
}

void CCD::ccdslv(){
    while (1) {
        slvinout->SlvListen(addr, rdnwr, len);
        if (addr==CCD_capture_addr) {
            slvinout->SlvAcknowledge();
            if (rdnwr==0) {
                slvinout->SlvReceiveWriteData(softwarestart);
                ccdready.notify();
            }
        }
        
        if (addr==CCD_done) {
            slvinout->SlvAcknowledge();
            if (rdnwr==1) {
                slvinout->SlvSendReadData(done);
                done=0;
                ccdready.notify();
            }
        }
    }
}