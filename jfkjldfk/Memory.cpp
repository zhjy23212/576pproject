//
//  Memory.cpp
//  jfkjldfk
//
//  Created by Jiyang on 4/6/16.
//  Copyright © 2016 Jiyang. All rights reserved.
//

#include "Memory.h"



void Memory::listenThread(){
    unsigned int reqAddr, reqLen,rdNwr;
    while (1) {
        slv->SlvListen(reqAddr, rdNwr, reqLen);
        if (reqAddr>=MEM_START_ADDRESS&&reqAddr<=MEM_END_ADDRESS) {
            slv->SlvAcknowledge();
            if (rdNwr) {
                for (int i = 0; i<reqLen; i++) {
                    slv->SlvSendReadData(image[reqAddr-MEM_START_ADDRESS+i]);
                }
            }else{
                for (int i = 0; i<reqLen; i++) {
                    slv->SlvReceiveWriteData(image[reqAddr-MEM_START_ADDRESS+i]);
                    
                }
            }
        }
        
        if (reqAddr == MEM_DIST ) {
            slv->SlvAcknowledge();
            if(rdNwr){
                slv->SlvSendReadData(distance);
            }else{
                slv->SlvReceiveWriteData(distance);
            }
        }
        
        if (reqAddr == MEM_ANG ) {
            slv->SlvAcknowledge();
            if(rdNwr){
                slv->SlvSendReadData(angle);
            }else{
                slv->SlvReceiveWriteData(angle);
            }
        }
    }
}

