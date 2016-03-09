//
//  Memory_component.cpp
//  jfkjldfk
//
//  Created by Jiyang on 16/3/8.
//  Copyright © 2016年 Jiyang. All rights reserved.
//

#include "Memory_component.h"


void Memory::  listenThread(){
    while (1) {
        ioPort->SlvListen(addr, Rdnwr, len);
        if(addr>=ADDR_begin&&addr+len<=ADDR_end+1){
            
            ioPort->SlvAcknowledge();
            if (Rdnwr) {
                for (int i = 0; i<len; i++) {
                    //cout<<"NOW IN MEM "<<mem_data[addr-ADDR_begin+i]<<endl;
                    ioPort->SlvSendReadData(mem_data[addr-ADDR_begin+i]);
                    
                }
            }else{
                for (int i = 0; i<len; i++) {
                    ioPort->SlvReceiveWriteData(mem_data[addr-ADDR_begin+i]);
                    
                    int cnt = 0;
                    for (int i = 72; i<108; i++) {
                        cout<<mem_data[i]<<" ";
                        cnt++;
                        if(cnt==6){
                            cout<<endl;
                            cnt = 0;
                        }
                    }
                    cout<<endl;
                }
            }
        }
    }
}