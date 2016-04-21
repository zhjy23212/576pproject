//
//  DSP.cpp
//  jfkjldfk
//
//  Created by Jiyang on 16/4/11.
//  Copyright © 2016年 Jiyang. All rights reserved.
//

#include "DSP.hpp"

//Mat test(256,256,CV_8UC1);

void DSP::dspmst(){
    wait(ready1);
    bool ack1=false;
    ack1=mstinout->MstBusRequest(id, true, MEM_DIST, 1);
    if (ack1==true) {
        mstinout->MstReadData(dist);
    }
    bool ack2=false;
    ack2=mstinout->MstBusRequest(id, true, MEM_ANG, 1);
    if (ack2==true) {
        mstinout->MstReadData(angle);
    }
    
    for (int i=0; i<IMG_HEIGHT; i++) {
        bool ack3=false;
        ack3=mstinout->MstBusRequest(id, true, MEM_START_ADDRESS+IMG_WIDTH*i, IMG_WIDTH);
        if (ack3==true) {
            for (int j=0; j<IMG_WIDTH; j++) {
                mstinout->MstReadData(img[i][j]);
//                cout<<img[i][j]<<" ";
            }
        }
//        cout<<endl;
    }
    dspdenoise();
    
}

void DSP::dspslv(){
    unsigned addr,rdnwr,len;
    unsigned needdeblur;
    while (1) {
        slvinout->SlvListen(addr, rdnwr, len);
        if (addr==dsp_need_process) {
            slvinout->SlvAcknowledge();
            if (rdnwr==0) {
                slvinout->SlvReceiveWriteData(needdeblur);
                ready1.notify();
            }
        }
    }
}

void DSP::dspdenoise(){
    Mat imgmat(256,256,CV_8UC1);
    for (int i=0; i<256; i++) {
        for (int j=0; j<256; j++) {
            imgmat.at<Vec3b>(i,j)=img[i][j];
        }
    }
    Mat src=imread("/Users/yanglizhuo/Desktop/Product/576project/576project/camera.png");
    imshow("origin", src);
    Mat dst=imgmat.clone();
    medianBlur(imgmat, dst, 3);
//    imshow("hehe", dst);
    waitKey();
}


void DSP::dspdeblur(){
    
}





