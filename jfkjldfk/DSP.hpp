//
//  DSP.hpp
//  jfkjldfk
//
//  Created by Jiyang on 16/4/11.
//  Copyright © 2016年 Jiyang. All rights reserved.
//

#ifndef DSP_hpp
#define DSP_hpp
#include <systemc.h>
#include "bus_interface.h"
#include "Bus_request.h"
#include "simple_interface.h"
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <algorithm>
#include <complex.h>
#include <math.h>

#define PI 3.14159
using namespace std;
using namespace cv;

#endif /* DSP_hpp */
class DSP:public sc_module{
public:
    sc_event ready1;
    sc_event ready2;
    sc_port<bus_master_if> mstinout;
    sc_port<bus_slave_if> slvinout;
    vector<vector<unsigned int> > img;
    unsigned id;
    unsigned done,dist,angle;
    SC_HAS_PROCESS(DSP);
    
    DSP(sc_module_name name,unsigned int dsp_id):sc_module(name){
        id=dsp_id;
        done=0;
        vector<unsigned> temp(256);
        for (int i=0; i<256; i++) {
            img.push_back(temp);
        }
        SC_THREAD(dspmst);
        SC_THREAD(dspslv);
        
    }
    void dspmst();
    void dspslv();
    void dspdenoise();
    double psnr();
    double nsrget();
    Mat fft2(Mat I,Size size);
    Mat psf2otf(Mat psf, int height, int width);
    Mat  special(double dist,double angle);
    Mat  dspdeblur(Mat imgmat, unsigned dist, unsigned int angle,double NSR);
    unsigned char  median(Mat imgmat,int x, int y );
};