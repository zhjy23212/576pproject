//
//  sad_component.hpp
//  jfkjldfk
//
//  Created by Jiyang on 16/3/23.
//  Copyright © 2016年 Jiyang. All rights reserved.
//

#ifndef sad_component_hpp
#define sad_component_hpp

#include <stdio.h>
#include <vector>
#include "bus_interface.h"
#include "Bus_request.h"

class sad_component:public sc_module{
public:
    sc_port<bus_master_if> sad_mst;
    unsigned id;

    unsigned int sad;
    unsigned int block;
    int i, v;
    SC_HAS_PROCESS(sad_component);
    sad_component(sc_module_name name, unsigned int id_tag):sc_module(name){
        this->id = id_tag;
        SC_THREAD(main);
    }
    
    void main();
    
    void BurstWrite(unsigned address, std::vector<unsigned int> vec, unsigned len);
    
    void BurstRead(unsigned address, std::vector<unsigned int> &vec, unsigned len);
    
    void SingleWrite(unsigned address, unsigned int data);
};




/*
int i, v;
unsigned int block;
unsigned int sad;
for (block=0; block<NUM_BLOCKS; block++)
{
    sad = 0;
    for (i=0; i<BLOCK_SIZE; i++)
    {
        v = MEM[INPUT1_ADDR+(block*BLOCK_SIZE)+i] -
        MEM[INPUT2_ADDR+(block*BLOCK_SIZE)+i];
        if( v < 0 ) v = -v;
            sad += v;
            }
    MEM[SAD_OUTPUT_ADDR+block] = sad;
}
 */

#endif /* sad_component_hpp */
