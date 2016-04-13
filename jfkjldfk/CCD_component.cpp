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
        done = 0;
        bool ack1=false;
        ack1=mstinout->MstBusRequest(ccdid, false, MEM_DIST, 1);
        if (ack1==true) {
            mstinout->MstWriteData(dist);
        }
        
        bool ack2=false;
        ack2=mstinout->MstBusRequest(ccdid, false, MEM_ANG, 1);
        if (ack2==true) {
            mstinout->MstWriteData(angle);
        }
        
        for (int i=0; i<IMG_HEIGHT; i++) {
            bool ack=false;
            ack=mstinout->MstBusRequest(ccdid, false, MEM_START_ADDRESS+IMG_WIDTH*i, IMG_WIDTH);
            if (ack==true) {
                for (int j=0; j<IMG_WIDTH; j++) {
                    mstinout->MstWriteData(image[i][j]);
                }
            }
            
        }
        done = 1;
    }
}

void CCD::ccdslv(){
    while (1) {
        
        
        slvinout->SlvListen(addr, rdnwr, len);
        if (addr==CCD_capture_addr) {
            inandout->Read(0, dist);
            inandout->Read(1, angle);
            slvinout->SlvAcknowledge();
            if (rdnwr==0) {
                slvinout->SlvReceiveWriteData(softwarestart);
                ccdready.notify();
            }
        }
        
        if (addr == CCD_done) {
            slvinout->SlvAcknowledge();
            if (rdnwr==1) {
                slvinout->SlvSendReadData(done);
                done=0;
            }
        }
    }
}