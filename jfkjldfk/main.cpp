//
//  main.cpp
//  576project
//
//  Created by 杨力卓 on 16/4/12.
//  Copyright © 2016年 University of Arizona (Procurement & Contracting). All rights reserved.
//

#include "bus_interface.h"
#include "Bus_request.h"
#include "simple_interface.h"
#include <vector>
#include "CCD_component.h"
#include "Memory.h"
#include "Motion_detect.h"
#include "share_bus.h"
#include "software.h"
#include <iostream>
#include <fstream>
#include "systemc.h"

using namespace std;

class top : public sc_module {                              //top component
public:
    sw*sw1;
    CCD*ccd1;
    share_bus*bus1;
    Memory*mem1;
    motion_detector*motion1;
    top(sc_module_name name, char* filename, char* dist, char* ang):sc_module(name){
        unsigned int distance=atoi(dist);
       unsigned int angle=atoi(ang);
        bus1=new share_bus("bus1",50);
        motion1=new motion_detector("motion1",distance,angle);
        mem1=new Memory("mem1");
        sw1=new sw("sw1",51);
        ccd1= new CCD("ccd1",filename);
        sw1->mst(*bus1);
        ccd1->inandout(*motion1);
        ccd1->mstinout(*bus1);
        ccd1->slvinout(*bus1);
        mem1->slv(*bus1);
    }
};

int sc_main (int argc, char * argv[]) {
    if (argc<=3){
        cout<<"usage:file distance angle"<<endl;
        exit(0);
    }
    //Memory memo(argv[1]);
    top("top1",argv[1],argv[2],argv[3]);
    sc_start();
    return 0;
}
