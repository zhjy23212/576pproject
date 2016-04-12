//
//  CCD_component.hpp
//  jfkjldfk
//
//  Created by Jiyang on 16/4/11.
//  Copyright © 2016年 Jiyang. All rights reserved.
//

//this component mainly record the image data imported from external file
//when read the file, also it will request motion data from its motion detector
//it is via direct connetction, not by bus

#ifndef CCD_component_hpp
#define CCD_component_hpp

#include <stdio.h>
#include "bus_interface.h"
#include "Bus_request.h"
#include "simple_interface.h"
#include <vector>
#include "share_bus.h"

using namespace std;

#endif /* CCD_component_hpp */

class CCD: public sc_module{
public:
    vector<vector<unsigned int> > image;
    sc_event ccdready;
    unsigned ccdid;
    unsigned int dist;
    unsigned int angle;
    unsigned int addr;
    unsigned int rdnwr;
    unsigned int len;
    unsigned softwarestart;
    unsigned done;
    sc_port<bus_master_if> mstinout;
    sc_port<bus_slave_if> slvinout;
    sc_port<simple_if> inandout;
    SC_HAS_PROCESS(CCD);
    
    CCD(sc_module_name name, unsigned int idNum ,char* filename):sc_module(name){
        this->ccdid = idNum;
        ifstream fin;
        fin.open(filename);
        if (!fin.is_open()) {
            cout<<"couldn't open the file "<<filename<<endl;
            sc_stop();
        }
        
        string line;
        while (std::getline(fin, line)) {
            std::istringstream iss(line);
            vector<unsigned int> eachrow;
            int a;
            while(iss>>a){
                eachrow.push_back(a);
            }
            image.push_back(eachrow);
            eachrow.clear();
        }
        
        done=0;
        
        SC_THREAD(ccdmst);
        SC_THREAD(ccdslv);
    }
    
    void ccdmst();
    void ccdslv();
};