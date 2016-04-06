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
#include "sad_component.h"
#include "Traffic_Generator.h"
//----------------------------------------------------------------
using namespace std;
//----------------------------------------------------------------
//vector<unsigned int>  mem_data;
//----------------------------------------------------------------


//----------------------------------------------------------------
class top_module:public sc_module{
    
public:
    unsigned int id ;
    
    share_bus *bus_inst;
    hw_accelerator *hw_inst;
    Memory *mem_inst;
    sw_component *sw_inst;
    sad_component *sad_inst;
    Traffic_Generator *tra_1,*tra_2;
    top_module(sc_module_name name,const char* filename, int period_1, int period_2) :sc_module(name){
        id = 0;
        sw_inst = new sw_component("sw_1",id++);
        sad_inst = new sad_component("sad1",id++);
        hw_inst = new hw_accelerator("hw_1",id++);
        tra_1 = new Traffic_Generator("tra1",id++,period_1);
        tra_2 = new Traffic_Generator("tra2",id++,period_2);
        mem_inst = new Memory("mem_1",filename);
        bus_inst = new share_bus("bus_1",id);
        
        sw_inst->sw_mst(*bus_inst);
        hw_inst->mst(*bus_inst);
        hw_inst->slv(*bus_inst);
        sad_inst->sad_mst(*bus_inst);
        tra_1->mst(*bus_inst);
        tra_2->mst(*bus_inst);
        mem_inst->ioPort(*bus_inst);
        
    }
    
    void top_print(){
        cout<<"The name of top module = "<< this->name()<<endl;
    }
};

//----------------------------------------------------------------


int sc_main(int argc, char* argv[]){
    if (argc<=3) {
        cout<<"USAGE: ./hwsw_main mem_filename.txt traffic_1_time traffic_2_time"<<endl;
        exit(EXIT_FAILURE);
    }
    
    
    cout<<"sc_main"<<endl;
    top_module *top = new top_module("top1",argv[1],atoi(argv[2]),atoi(argv[3]));
    top->top_print();
    sc_start();
    
    return 0;
}
