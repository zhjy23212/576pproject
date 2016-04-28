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
    Mat padding;
    Mat imgmat(256,256,CV_8U);
    for (int i=0; i<256; i++) {
        for (int j=0; j<256; j++) {
            imgmat.at<unsigned char>(i,j)=img[i][j];
        }
    }
    padding = imgmat.clone();
    copyMakeBorder(imgmat, padding, 1, 1, 1, 1, BORDER_REPLICATE);
    //Mat src=imread("camera.png");
    
    for(int i = 0;i<256;i++){
        for (int j = 0; j<256;  j++) {
            imgmat.at<unsigned char>(i,j) = median(padding, i, j);
            //need add latency for the median operation, to do later
        }
    }
    
    //imshow("origin", imgmat);  //use here to see the denoise result
    //Mat dst=imgmat.clone();
    //medianBlur(imgmat, dst, 3);
    //waitKey();
    /*
    Mat img = imgmat.clone();
    Mat planes[] = {Mat_<float>(img), Mat::zeros(img.size(), CV_32F)};
    Mat complexI;    //Complex plane to contain the DFT coefficients {[0]-Real,[1]-Img}
    merge(planes, 2, complexI);
    dft(complexI, complexI);  // Applying DFT
    
    
    split(complexI, planes);                   // planes[0] = Re(DFT(I), planes[1] = Im(DFT(I))
    magnitude(planes[0], planes[1], planes[0]);// planes[0] = magnitude
    Mat magI = planes[0];
    
    magI += Scalar::all(1);                    // switch to logarithmic scale
    log(magI, magI);
    
    // crop the spectrum, if it has an odd number of rows or columns
    magI = magI(Rect(0, 0, magI.cols & -2, magI.rows & -2));
    
    // rearrange the quadrants of Fourier image  so that the origin is at the image center
    int cx = magI.cols/2;
    int cy = magI.rows/2;
    
    Mat q0(magI, Rect(0, 0, cx, cy));   // Top-Left - Create a ROI per quadrant
    Mat q1(magI, Rect(cx, 0, cx, cy));  // Top-Right
    Mat q2(magI, Rect(0, cy, cx, cy));  // Bottom-Left
    Mat q3(magI, Rect(cx, cy, cx, cy)); // Bottom-Right
    
    Mat tmp;                           // swap quadrants (Top-Left with Bottom-Right)
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);
    
    q1.copyTo(tmp);                    // swap quadrant (Top-Right with Bottom-Left)
    q2.copyTo(q1);
    tmp.copyTo(q2);
    
    normalize(magI, magI, 0, 1, CV_MINMAX); // Transform the matrix with float values into a
    // viewable image form (float between values 0 and 1).
    
    imshow("spectrum magnitude", magI);
    waitKey();
     */
    Mat me = special(dist, angle);
    Mat otf = psf2otf(me, imgmat.rows, imgmat.cols) ;
    //dft(me, otf);
    cout<<otf.rows << " "<< otf.cols<<endl;
    
    for(int i = 0; i<otf.rows; i++){
        for (int j = 0 ; j<otf.cols; j++) {
            cout<<otf.at<double>(i,j)<<" ";
        }
        cout<<endl;
    }
}

unsigned char DSP:: median(Mat imgmat,int x, int y ){
    vector<unsigned char> temp;
    for(int i = x; i<x+3;i++){
        for (int j = y; j<y+3; j++) {
            if(i == x && j == y){
                temp.push_back(imgmat.at<unsigned char>(i,j));
            }else{
                std::vector<unsigned char>::iterator up= std::upper_bound(temp.begin(), temp.end(), imgmat.at<unsigned char>(i,j));
                temp.insert(up, imgmat.at<unsigned char>(i,j));
            }
        }
    }
    return temp[4];
}

Mat DSP::psf2otf(cv::Mat psf, int height, int width){
    Mat otf = Mat::zeros(height,width,psf.type());
    
    Size dftSize;
    dftSize.width  = getOptimalDFTSize(width);
    dftSize.height = getOptimalDFTSize(height);
    
    // allocate temporary buffers and initialize them with 0's
    Mat temp(dftSize, psf.type(), Scalar::all(0));
    
    //copy psf to the top-left corners of temp
    psf.copyTo(temp(cv::Rect_<double>(0,0,psf.cols,psf.rows)));
    
    
    // Circularly shift otf so that the "center" of the PSF is at the
    // (0,0) element of the array.
    Mat psf2 = temp.clone();
    
    int cx = psf.cols/2;
    int cy = psf.rows/2;
    
    Mat p0(temp, Rect(0, 0, cx, cy));                   // Top-Left - Create a ROI per quadrant
    Mat p1(temp, Rect(cx, 0, psf2.cols-cx, cy));            // Top-Right
    Mat p2(temp, Rect(0, cy, cx, psf2.rows - cy));      // Bottom-Left
    Mat p3(temp, Rect(cx,cy, psf2.cols-cx, psf2.rows - cy)); // Bottom-Right
    
    
    Mat q0(psf2, Rect(0, 0,  psf2.cols-cx, psf2.rows - cy));// Top-Left - Create a ROI per quadrant
    Mat q1(psf2, Rect(psf2.cols-cx, 0, cx, psf2.rows - cy));// Top-Right
    Mat q2(psf2, Rect(0, psf2.rows - cy, psf2.cols-cx, cy));        // Bottom-Left
    Mat q3(psf2, Rect(psf2.cols-cx, psf2.rows - cy,cx,cy)); // Bottom-Right
    
    // swap quadrants (Top-Left with Bottom-Right)
    p0.copyTo(q3);
    p3.copyTo(q0);
    
    // swap quadrant (Top-Right with Bottom-Left)
    p1.copyTo(q2);
    p2.copyTo(q1);
    
    // Computer the OTF
    Mat planes[] = {Mat_<double>(psf2), Mat::zeros(psf2.size(), CV_64F)};
    Mat complexI;
    merge(planes, 2, complexI);
    
    dft(psf2, complexI);
    otf = complexI;
    
    return otf;
}

// this return the motion blur kernel
Mat DSP::special(unsigned int dist, unsigned int angle){
    double half = (dist-1)/2.0;
    double phi = PI*(angle%180)/180;
    double cosphi = cos(phi);
    double sinphi = sin(phi);
    int sign = 1;
    if (cosphi<0) {
        sign = -1;
    }
    int linedt = 1;
    int sx = (int) fabs(half*cosphi +linedt * sign - 0.000001*dist);
    int sy = (int) fabs(half*sinphi +linedt - 0.000001*dist);
    Mat_<double> psf1(sy,sx,CV_64F);
    Mat_<double> psf2(sy*2,sx*2,CV_64F);
    int row = 2 * sy;
    int col = 2 * sx;
    for (int i = 0; i<sy; i++) {
        double* pvalue = psf1.ptr<double>(i);
        for (int j = 0 ; j<sx; j++) {
            pvalue[j] = i *fabs(cosphi) -  j*sinphi;
            double rad = sqrt(i*i +j*j);
            if (rad >= half && fabs(pvalue[j])<=linedt) {
                double temp = half - fabs((j+pvalue[j]*sinphi)/cosphi);
                pvalue[j] = sqrt(pvalue[j] * pvalue[j] + temp*temp);
            }
            pvalue[j] = linedt + 0.000001 - fabs(pvalue[j]);
            if(pvalue[j]<0){
                pvalue[j] = 0;
            }
        }
    }
    
    for (int i = 0; i< sy; i++) {
        double* pvalue1 = psf1.ptr<double >(i);
        double* pvalue2 = psf2.ptr<double >(i);
        for (int j = 0; j<sx ; j++) {
            pvalue2[j] = pvalue1[j];
        }
    }
    
    for (int i = 0; i < sy; i++)
    {
        for (int j = 0; j < sx; j++)
        {
            psf2[2 * sy -1 - i][2 * sx -1 - j] = psf1[i][j];
            psf2[sy + i][j] = 0;
            psf2[i][sx + j] = 0;
        }
    }
    
    double sum = 0;
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < col; j++)
        {
            sum+= psf2[i][j];
        }
    }
    psf2 = psf2 / sum;
    if (cosphi>0)
    {
        flip(psf2, psf2, 0);
    }
    
    return psf2;
}

/*
 Mat DSP::dspdeblur(Mat imgmat, unsigned dist, unsigned int angle,int NSR){
    
     
}*/





