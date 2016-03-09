//
//  hw_accelerator.cpp
//  jfkjldfk
//
//  Created by Jiyang on 16/3/8.
//  Copyright © 2016年 Jiyang. All rights reserved.
//

#include <stdio.h>
#include "hw_accelerator.h"

using namespace std;

void hw_accelerator:: BurstWrite(unsigned address, vector<unsigned int> vec, unsigned len){
    bool ack = mst->MstBusRequest(this->id, false, address, len);
    if(ack){
        for(int i = 0 ; i<len;i++){
            //cout<<"data is "<<vec[i]<<endl;
            mst->MstWriteData(vec[i]);
        }
    }
}

void hw_accelerator:: BurstRead(unsigned address, vector<unsigned int> &vec, unsigned len){
    bool ack = mst->MstBusRequest(this->id, true, address, len);
    //cout<<"burst in and ack is "<<ack<<endl;
    if(ack){
        for(int i = 0 ; i<len;i++){
            unsigned int  data_inst;
            mst->MstReadData(data_inst);
            vec.push_back(data_inst);
        }
    }
}


void hw_accelerator:: SingleWrite(unsigned address, unsigned int data){
    bool ack = mst->MstBusRequest(this->id, false, address, 1);
    if (ack) {
        mst->MstWriteData(data);
    }
}


void hw_accelerator:: SingleRead(unsigned address, unsigned int &data){
    bool ack = mst->MstBusRequest(this->id, true, address, 1);
    if (ack) {
        mst->MstReadData(data);
    }
}

void hw_accelerator:: slave_thread(){
    while (true) {
        slv->SlvListen(local_addr, local_Rdnwr, local_reqLen);
        //cout << local_addr << endl;
        if (local_addr>=HW_ADDR_a&&local_addr<=HW_ADDR_done) {
            
            slv->SlvAcknowledge();
            
            if (local_addr == HW_ADDR_a) {
                slv->SlvReceiveWriteData(array_a_addr);
            }
            else if (local_addr == HW_ADDR_b) {
                slv->SlvReceiveWriteData(array_b_addr);
            }
            else if (local_addr == HW_ADDR_c) {
                slv->SlvReceiveWriteData(array_c_addr);
            }
            else if (local_addr == HW_ADDR_op) {
                slv->SlvReceiveWriteData(op_type);
                done=0;
                start_signal.notify();
            }
            else if (local_addr == HW_ADDR_done) {
                slv->SlvSendReadData(done);
            }
        }
    }
}





void hw_accelerator:: master_thread(){
    while (1) {
        wait(start_signal);
        //cout<<"now enter start_signal"<<endl;
        if(op_type ==1||op_type==2){
            
            if(op_type==1&&do_burst){
                vector<unsigned int> temp(SIZE,0);
                //cout<<"here we finish wirte 0"<<endl;
                BurstWrite(array_c_addr, temp, SIZE);
                done = 1;
                op_type = 0;
                
            }else if(op_type==1&&!do_burst){
                for (int i =0; i<5; i++) {
                    SingleWrite(array_c_addr+i, 0);
                }
                op_type = 0;
                done=1;
                
            }else if (op_type==2&&do_burst){
                //cout<<"now in op 2"<< endl;
                unsigned int cij;
                vector<unsigned int> a_vec;
                BurstRead(array_a_addr, a_vec, SIZE);
                //cout << "here" << endl;
                SingleRead(array_c_addr, cij);
                //cout<<"cij = "<<cij<<endl;
                unsigned int b_data;
                
                vector<unsigned int> b_vec;
                for (int i = 0; i<SIZE; i++) {
                    SingleRead(array_b_addr+6*i, b_data);
                    b_vec.push_back(b_data);
                }
                for (int i = 0; i<SIZE; i++) {
                    cij += a_vec[i]*b_vec[i];
                }
                
                SingleWrite(array_c_addr, cij);
                op_type = 0;
                done = 1;
            }else if (op_type==2&&!do_burst){
                unsigned int cij;
                vector<unsigned int> a_vec;
                SingleRead(array_c_addr, cij);
                unsigned int a_data;
                unsigned int b_data;
                vector<unsigned int> b_vec;
                for (int i = 0; i<SIZE; i++) {
                    SingleRead(array_a_addr+i, a_data);
                    a_vec.push_back(a_data);
                    SingleRead(array_b_addr+6*i, b_data);
                    b_vec.push_back(b_data);
                }
                for (int i = 0; i<SIZE; i++) {
                    cij += a_vec[i]*b_vec[i];
                }
                SingleWrite(array_c_addr, cij);
                op_type = 0;
                done=1;
            }
        }
    }
}

