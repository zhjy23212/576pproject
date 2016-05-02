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
    Mat denoise=dspdenoise();
    
    if (needdeblur==1) {
        double NSR=nsrget();
//        cout<<NSR<<endl;
        
        wait(256*256*DSP_PERIOD,SC_NS);
        
        Mat ret=dspdeblur(denoise, dist, angle, NSR);
        
        for (int i=0; i<ret.rows; i++) {
            for (int j=0; j<ret.cols; j++) {
                img[i][j]=(unsigned)(ret.at<double>(i,j)*255);
//                cout<<img[i][j]<<" ";
            }
//            cout<<endl;
        }
        wait(256*256*DSP_PERIOD,SC_NS);
//        imshow("hehe", ret);
//        waitKey();
    }
    
    for (int i=0; i<IMG_HEIGHT; i++) {
        bool ack4=false;
        ack4=mstinout->MstBusRequest(id, false, LCD_IMAGE_START_ADDR+i*IMG_WIDTH, IMG_WIDTH);
        if (ack4==true) {
            for (int j=0; j<IMG_WIDTH; j++) {
                mstinout->MstWriteData(img[i][j]);
            }
        }
    }
    
    ack1 = mstinout->MstBusRequest(id, false, LCD_DATA_TRANS_FIN, 1);
    if(ack1){
        mstinout->MstWriteData(1);
    }
    
    done=1;
    
}

void DSP::dspslv(){
    unsigned addr,rdnwr,len;
    while (1) {
        slvinout->SlvListen(addr, rdnwr, len);
        if (addr==dsp_need_process) {
            slvinout->SlvAcknowledge();
            if (rdnwr==0) {
                slvinout->SlvReceiveWriteData(needdeblur);
                ready1.notify();
            }
        }
        else if (addr==DSP_DONE) {
            slvinout->SlvAcknowledge();
            if (rdnwr==1) {
                slvinout->SlvSendReadData(done);
                done=0;
            }
        }
    }
}

double DSP::psnr(){
    double ratio;
    double MSE=0;
   
    Mat noise=imread("/Users/yanglizhuo/Desktop/Product/576project/576project/cm.png");
    for (int i=0; i<256; i++) {
        for (int j=0; j<256; j++) {
            int temp=img[i][j]-int(noise.at<unsigned char>(i,j));
            MSE+=pow(temp, 2);
        }
    }
    MSE=MSE/(256*256);
    ratio=10*log10((256*256)/MSE);
    
    return ratio;
}

Mat DSP::dspdenoise(){
    Mat padding;
    Mat imgmat(256,256,CV_8U);
    for (int i=0; i<256; i++) {
        for (int j=0; j<256; j++) {
            imgmat.at<unsigned char>(i,j)=img[i][j];
//            cout<<imgmat.at<unsigned char>(i,j)<<" ";
        }
//        cout<<endl;
    }
    padding = imgmat.clone();
    copyMakeBorder(imgmat, padding, 1, 1, 1, 1, BORDER_REPLICATE);
//    Mat src=imread("camera.png");
    
    for(int i = 0;i<256;i++){
        for (int j = 0; j<256;  j++) {
            imgmat.at<unsigned char>(i,j) = median(padding, i, j);
            //need add latency for the median operation, to do later
        }
    }
    
//    imshow("origin", imgmat);  //use here to see the denoise result
    Mat dst=imgmat.clone();
//    waitKey();
 
//    Mat me = special(dist, angle);
//    Mat otf = psf2otf(me, imgmat.rows, imgmat.cols) ;

    //dft(me, otf);
//    cout<<otf.rows << " "<< otf.cols<<endl;
    
//    for(int i = 0; i<otf.rows; i++){
//        for (int j = 0 ; j<otf.cols; j++) {
////            cout<<otf.at<double>(i,j)<<" ";
//        }
////        cout<<endl;
//    }
    
    
    return dst;
//    double nsr=psnr();
//    cout<<nsr<<endl;
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
Mat DSP::special(double len, double angle){
    double half=len/2;
    double alpha = (angle-floor(angle/ 180) *180) /180* PI;
    double cosalpha = cos(alpha);
    double sinalpha = sin(alpha);
    int xsign;
    if (cosalpha < 0)
    {
        xsign = -1;
    }
    else
    {
        if (angle == 90)
        {
            xsign = 0;
        }
        else
        {
            xsign = 1;
        }
    }
    int psfwdt = 1;
    int sx = (int)fabs(half*cosalpha + psfwdt*xsign - len*EPSILON);
    int sy = (int)fabs(half*sinalpha + psfwdt - len*EPSILON);
    Mat_<double> psf1(sy, sx, CV_64F);
    Mat_<double> psf2(sy * 2, sx * 2, CV_64F);
    int row = 2 * sy;
    int col = 2 * sx;
    /*为减小运算量，先计算一半大小的PSF*/
    for (int i = 0; i < sy; i++)
    {
        double* pvalue = psf1.ptr<double>(i);
        for (int j = 0; j < sx; j++)
        {
            pvalue[j] = i*fabs(cosalpha) - j*sinalpha;
            
            double rad = sqrt(i*i + j*j);
            if (rad >= half && fabs(pvalue[j]) <= psfwdt)
            {
                double temp = half - fabs((j + pvalue[j] * sinalpha) / cosalpha);
                pvalue[j] = sqrt(pvalue[j] * pvalue[j] + temp*temp);
            }
            pvalue[j] = psfwdt + EPSILON - fabs(pvalue[j]);
            if (pvalue[j] < 0)
            {
                pvalue[j] = 0;
            }
        }
    }
    /*将模糊核矩阵扩展至实际大小*/
    for (int i = 0; i < sy; i++)
    {
        double* pvalue1 = psf1.ptr<double>(i);
        double* pvalue2 = psf2.ptr<double>(i);
        for (int j = 0; j < sx; j++)
        {
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
    /*保持图像总能量不变，归一化矩阵*/
    double sum = 0;
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < col; j++)
        {
            sum+= psf2[i][j];
        }
    }
    psf2 = psf2 / sum;
    if (cosalpha>0)
    {
        flip(psf2, psf2, 0);
    }
    
    //cout << "psf2=" << psf2 << endl;
    return psf2;
}

double DSP::nsrget(){
    Mat temp1=imread("/Users/yanglizhuo/Desktop/Product/576project/576project/camera.png");
    double ret;
    double noisevar=0.001;
    double k=0;
    Mat temp;
    temp1.convertTo(temp, CV_64F);
    for (int i=0; i<256; i++) {
        for (int j=0; j<256; j++) {
            k+=(temp.at<double>(i, j)/256);
//            cout<<(double(temp.at<unsigned char>(i, j))/256)<<" ";
        }
//        cout<<endl;
    }
    double mean=k/(256*256);
    double imgvar=0;
    for (int i=0; i<256; i++) {
        for (int j=0;j<256; j++) {
            imgvar+=pow(abs(temp.at<double>(i,j)/256-mean), 2);
        }
    }
    imgvar/=(256*256);
//    imgvar=0.0598;
//    cout<<imgvar<<endl;
    ret=noisevar/imgvar;
    return ret;
}


Mat DSP::dspdeblur(Mat imgmat, unsigned dist, unsigned int angle,double NSR){
    Mat me = special(dist, angle);
    
    wait(dist*cos(angle*1.0/180*PI)*dist*sin(angle*1.0/180*PI)*DSP_PERIOD,SC_NS);
    
    Mat otf = psf2otf(me, imgmat.rows, imgmat.cols) ;
    
    wait(256*256*4*DSP_PERIOD,SC_NS);
    
    Mat demon=otf.clone();
    for (int i=0; i<imgmat.rows; i++) {
        for (int j=0; j<imgmat.cols; j++) {
//            cout<<demon.at<double>(i,j)<<" ";
            demon.at<double>(i,j)=pow(double(otf.at<double>(i,j)), 2)+NSR;
//            cout<<demon.at<double>(i,j)<<" ";
            const double eps=EPSILON;
            demon.at<double>(i,j)=max(demon.at<double>(i,j),sqrt(eps));
//            cout<<demon.at<double>(i,j)<<" ";
//            cout<<otf.at<double>(i,j)<<" ";
        }
//        cout<<endl;
    }
    wait(256*256*2*DSP_PERIOD,SC_NS);
    
    Mat G=otf.clone();
    for (int i=0; i<imgmat.rows; i++) {
        for (int j=0; j<imgmat.cols; j++) {
            G.at<double>(i,j)=G.at<double>(i,j)/demon.at<double>(i,j);
//            cout<<G.at<double>(i,j)<<" ";
        }
//        cout<<endl;
    }
    wait(256*256*DSP_PERIOD,SC_NS);
    
    string filename="/Users/yanglizhuo/Desktop/Product/576project/576project/kernel.txt";
    vector<vector<double> > gg;
    ifstream fin;
    fin.open(filename);
    if (!fin.is_open()) {
        cout<<"couldn't open the file "<<filename<<endl;
        sc_stop();
    }
    
    string line;
    while (std::getline(fin, line)) {
        std::istringstream iss(line);
        vector<double> eachrow;
        double a;
        while(iss>>a){
            eachrow.push_back(a);
        }
        gg.push_back(eachrow);
        eachrow.clear();
    }
    
    Mat GG(256,256,CV_64F);
    
    for (int i=0; i<imgmat.rows; i++) {
        for (int j=0; j<imgmat.cols; j++) {
            GG.at<double>(i,j)=gg[i][j];
//            cout<<GG.at<double>(i,j)<<" ";
        }
//        cout<<endl;
    }
    Mat hehe=Mat_<double>(imgmat);
    
    for (int i=0; i<imgmat.rows; i++) {
        for (int j=0; j<imgmat.cols; j++) {
            hehe.at<double>(i,j)/=255;
            //            cout<<GG.at<double>(i,j)<<" ";
        }
        //        cout<<endl;
    }
    
    Mat c1,c2;
    Mat planes[] = {Mat_<double>(hehe), Mat::zeros(imgmat.size(), CV_64F)};
    Mat complexI;
    merge(planes, 2, complexI);
    
//    cout<<"111"<<endl;
    
    dft(complexI, complexI);
    
    wait(256*256*4*DSP_PERIOD,SC_NS);
    
//    Size tempsize=Size_<int>(256, 256);
//    hehe=fft2(hehe, tempsize);
//    vector<Mat> chan;
//    split(hehe, chan);
//    c1=chan.at(0);
//    c2=chan.at(1);
    
    split(complexI, planes);
    
    multiply(planes[0], GG, planes[0]);
    multiply(planes[1], GG, planes[1]);
    
    wait(256*256*2*DSP_PERIOD,SC_NS);
    
    merge(planes, 2, complexI);
//    idft(complexI, complexI);
//    split(complexI, planes);
    
    for (int i=0; i<imgmat.rows; i++) {
        for (int j=0; j<imgmat.cols; j++) {
//            cout<<complexI.at<double>(i,j)<<" ";
        }
//        cout<<endl;
    }

//    cout<<hehe.channels()<<endl;
 
    
//    for (int i=0; i<imgmat.rows; i++) {
//        for (int j=0; j<imgmat.cols; j++) {
//            //            cout<<complexI.at<double>(i,j)<<" ";
//            complexI.at<double>(i,j)=G.at<double>(i,j)*complexI.at<double>(i,j);
//            //            cout<<complexI.at<double>(i,j)<<" ";
//        }
//        //        cout<<endl;
//    }
    

//    
    cv::Mat inverseTransform,test;
    cv::dft(complexI, inverseTransform, cv::DFT_INVERSE|cv::DFT_REAL_OUTPUT);
    
    wait(256*256*4*DSP_PERIOD,SC_NS);
    
    normalize(inverseTransform, inverseTransform, 0, 1, CV_MINMAX);
//    inverseTransform.convertTo(test, CV_8U);
//    imshow("Reconstructed", inverseTransform);
//    waitKey();
    
    

    
    
    
//    Mat invDFT,invDFTcvt;
//    idft(complexI, invDFT,cv::DFT_SCALE|cv::DFT_REAL_OUTPUT);
//    
//    for (int i=0; i<imgmat.rows; i++) {
//        for (int j=0; j<imgmat.cols; j++) {
////                        cout<<invDFT.at<double>(i,j)<<" ";
//        }
////                cout<<endl;
//    }
//    
//    invDFT.convertTo(invDFTcvt, CV_8U);
    
//    return invDFTcvt;
    
    return inverseTransform;
}

Mat DSP::fft2(Mat I,Size size)
{
    Mat If = Mat::zeros(I.size(),I.type());
    
    Size dftSize;
    
    // compute the size of DFT transform
    dftSize.width  = getOptimalDFTSize(size.width);
    dftSize.height = getOptimalDFTSize(size.height);
    // No padded
    //dftSize  = size;
    
    
    // allocate temporary buffers and initialize them with 0's
    Mat tempI(dftSize, I.type(), Scalar::all(0));
    
    //copy I to the top-left corners of temp
    Mat roiI(tempI,Rect(0,0,I.cols,I.rows));
    I.copyTo(roiI);
    
    if(I.channels()==1)
    {
        dft(tempI,If,DFT_COMPLEX_OUTPUT);
    }
    else
    {
        vector<Mat> channels;
        split(tempI,channels);
        for(int n= 0;n<I.channels();n++)
        {
            dft(channels[n],channels[n],DFT_COMPLEX_OUTPUT);
        }
        
        cv::merge(channels,If);
    }
    
    return If(Range(0,size.height),Range(0,size.width));
}



