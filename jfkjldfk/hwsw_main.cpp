//
//  hwsw.cpp
//  HW2
//
//  Created by Jiyang on 16/2/18.
//  Copyright © 2016 Jiyang. All rights reserved.
//


//----------------------------------------------------------------
#include "bus_interface.h"
#include "Bus_request.h"
#include "share_bus.h"
#include "hw_accelerator.h"
#include "Memory_component.h"
#include "software_component.h"
//----------------------------------------------------------------
using namespace std;
//----------------------------------------------------------------
//vector<unsigned int>  mem_data;
//----------------------------------------------------------------


//----------------------------------------------------------------
class top_module:public sc_module{
    
public:
    unsigned int id = 0;
    
    share_bus *bus_inst;
    hw_accelerator *hw_inst;
    Memory *mem_inst;
    sw_component *sw_inst;
    top_module(sc_module_name name) :sc_module(name){
        sw_inst = new sw_component("sw_1",id++);
        hw_inst = new hw_accelerator("hw_1",id++);
        mem_inst = new Memory("mem_1");
        bus_inst = new share_bus("bus_1");
        
        sw_inst->sw_mst(*bus_inst);
        hw_inst->mst(*bus_inst);
        hw_inst->slv(*bus_inst);
        mem_inst->ioPort(*bus_inst);
        
    }
};

//----------------------------------------------------------------


int sc_main(int argc, char* argv[]){
    cout<<"sc_main"<<endl;
    top_module *top = new top_module("top1");
    sc_start();
    
    return 0;
}
