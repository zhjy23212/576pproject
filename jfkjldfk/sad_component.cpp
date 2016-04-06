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

void sad_component:: SingleRead(unsigned address, unsigned int &data){
    bool ack = sad_mst->MstBusRequest(this->id, true, address, 1);
    if (ack) {
        sad_mst->MstReadData(data);
    }
}

void  sad_component:: main(){
    
    
    for (block=0; block<NUM_BLOCKS; block++)
    {
        sad = 0;
        wait(5,SC_NS);
        int v;
        
        int block_addr = block*BLOCK_SIZE;
        wait(5,SC_NS);

        for(int i = 0; i<BLOCK_SIZE;i++){
            unsigned int v1,v2;
            int offset = block_addr+i;
            wait(5,SC_NS);
            
            SingleRead(INPUT1_ADDR+offset, v1);
            wait(5,SC_NS);
            
            SingleRead(INPUT2_ADDR+offset, v2);
            wait(5,SC_NS);
            
            v = v1-v2;
            wait(5,SC_NS);
            
            if(v<0){
                wait(5,SC_NS);
                v = -v;
            }
            wait(5,SC_NS);
            
            sad += v;
            wait(5,SC_NS);
            
        }
        std::cout<<"WRITE SAD @ "<<sc_time_stamp()<<" BLOCK "<<block<<"  sad = " <<sad<< endl;
        SingleWrite(SAD_OUTPUT_ADDR+block, sad);
        wait(5,SC_NS);
        
        
        
    }
    
    std::cout<<"SAD computation finished!"<<endl;
        
}



