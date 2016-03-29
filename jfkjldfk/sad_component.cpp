//
//  sad_component.cpp
//  jfkjldfk
//
//  Created by Jiyang on 16/3/23.
//  Copyright © 2016年 Jiyang. All rights reserved.
//

#include "sad_component.h"

void sad_component:: BurstWrite(unsigned address, std::vector<unsigned int> vec, unsigned len){
    bool ack = sad_mst->MstBusRequest(this->id, false, address, len);
    if(ack){
        for(int i = 0 ; i<len;i++){
            //cout<<"data is "<<vec[i]<<endl;
            sad_mst->MstWriteData(vec[i]);
        }
    }
}

void sad_component:: BurstRead(unsigned address, std::vector<unsigned int> &vec, unsigned len){
    bool ack = sad_mst->MstBusRequest(this->id, true, address, len);
    //cout<<"burst in and ack is "<<ack<<endl;
    if(ack){
        for(int i = 0 ; i<len;i++){
            unsigned int  data_inst;
            sad_mst->MstReadData(data_inst);
            vec.push_back(data_inst);
        }
    }
}

void sad_component:: SingleWrite(unsigned address, unsigned int data){
    bool ack = sad_mst->MstBusRequest(this->id, false, address, 1);
    if (ack) {
        sad_mst->MstWriteData(data);
    }
}

void  sad_component:: main(){
    
    
    for (block=0; block<NUM_BLOCKS; block++)
    {
        std::vector<unsigned int> v1,v2;
        sad = 0;
        int v;
        BurstRead(INPUT1_ADDR+(block*BLOCK_SIZE), v1, BLOCK_SIZE);
        BurstRead(INPUT2_ADDR+(block*BLOCK_SIZE), v2, BLOCK_SIZE);
        
        for(int i = 0; i<v1.size();i++){
            v = v1[i]-v2[i];
            wait(TIME_UNIT,SC_NS);
            if(v<0) v = -v;
            wait(TIME_UNIT,SC_NS);
            sad += v;
            wait(TIME_UNIT,SC_NS);
            
        }
        std::cout<<"WRITE SAD "<<sc_time_stamp()<<"  sad = " <<sad<< endl;
        SingleWrite(SAD_OUTPUT_ADDR+block, sad);
        
        
        
    }
        
}



