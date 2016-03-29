//
//  Traffic_Generator.cpp
//  jfkjldfk
//
//  Created by Jiyang on 16/3/28.
//  Copyright © 2016年 Jiyang. All rights reserved.
//

#include "Traffic_Generator.h"


unsigned int Traffic_Generator:: random(int a, int b){
    srand(unsigned(time(0)));
    return (unsigned int)(a+rand()%(b-a+1));
}

void Traffic_Generator:: BurstWrite(unsigned address, std::vector<unsigned int> vec, unsigned len){
    bool ack = mst->MstBusRequest(this->id, false, address, len);
    if(ack){
        for(unsigned int i = 0 ; i<len;i++){
            //cout<<"data is "<<vec[i]<<endl;
            mst->MstWriteData(vec[i]);
        }
    }
}

void Traffic_Generator:: BurstRead(unsigned address, std::vector<unsigned int> &vec, unsigned len){
    bool ack = mst->MstBusRequest(this->id, true, address, len);
    //cout<<"burst in and ack is "<<ack<<endl;
    if(ack){
        for(unsigned int i = 0 ; i<len;i++){
            unsigned int  data_inst;
            mst->MstReadData(data_inst);
            vec.push_back(data_inst);
        }
    }
}


void Traffic_Generator:: main(){
    while (1) {
        unsigned int ranLen  = random(32, 64);
       // std::cout<<"Traffic generator id->"<<this->id<<" @"<<sc_time_stamp()<<endl;
        std::vector<unsigned int> vec;
        BurstRead(INPUT1_ADDR, vec, ranLen);
        wait(this->period,SC_NS);
        
    }
}