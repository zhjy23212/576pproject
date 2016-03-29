//
//  Traffic_Generator.hpp
//  jfkjldfk
//
//  Created by Jiyang on 16/3/28.
//  Copyright © 2016年 Jiyang. All rights reserved.
//

#ifndef Traffic_Generator_hpp
#define Traffic_Generator_hpp

#include <stdio.h>
#include "Bus_request.h"
#include <vector>
#include "bus_interface.h"
#include <ctime>






class Traffic_Generator:public sc_module{
public:
    
    int id;
    int period;
    sc_port<bus_master_if> mst;
    
    unsigned int random(int a, int b);
    
    void main();
    
    void BurstWrite(unsigned address, std::vector<unsigned int> vec, unsigned len);
    
    void BurstRead(unsigned address, std::vector<unsigned int> &vec, unsigned len);

    SC_HAS_PROCESS(Traffic_Generator);
    
    Traffic_Generator(sc_module_name name, int id, int period):sc_module(name){
        
        this->id = id;
        this->period = period;
        SC_THREAD(main);
    }
};



#endif /* Traffic_Generator_hpp */
