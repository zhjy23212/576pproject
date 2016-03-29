//
//  software_component.cpp
//  jfkjldfk
//
//  Created by Jiyang on 16/3/8.
//  Copyright © 2016年 Jiyang. All rights reserved.
//

#include "software_component.h"
void sw_component:: main() // Total Cycles: 819343
{
    int n;
    int i,j;
    
    //----------comment from here for loop1
    
    for (n = 0 ; n < LOOPS ; n++) // Total Cycles: 818600,  Execs: 1,     Iters: 1000
    {
        //cout<< "WRITE ADDR C time "<<n<<endl;
        
        
        for(i=1;i<=SIZE;i++){        // Total Cycles: 57900,   Execs: 1000,  Iters: 5
            //                for(j=1;j<=SIZE;j++)      // Total Cycles: 52000,   Execs: 5000,  Iters: 5
            //                    c[i][j] = 0;
            
            bool ack = sw_mst->MstBusRequest(this->id, false, HW_ADDR_c, 1);
            if(ack){
                sw_mst->MstWriteData(ADDR_begin+72+(SIZE+1)*i+1);
            }
            ack = sw_mst->MstBusRequest(this->id, false, HW_ADDR_op, 1);
            if(ack){
                sw_mst->MstWriteData(1);
            }
            
            unsigned int done_sig = 0;
            
            //cout<< "busy waiting here"<<endl;
            
            
            
            do{
                //                    cout << "busy request" << endl;
                ack = sw_mst->MstBusRequest(this->id, true, HW_ADDR_done, 1);
                if(ack){
                    sw_mst->MstReadData(done_sig);
                }
                
            }while (done_sig==0);
            
            //cout << "finish" << endl;
        }
        
        
        //----------COMMENT LOOP1 ENDS HERE
        cout<< "NOW START LOOP2 FOR SW "<<n<<endl;
        
        
        //----------comment from here for loop2
        
        for(i=1;i<=SIZE;i++){        // Total Cycles: 757900,  Execs: 1000,  Iters: 5
            for(j=1;j<=SIZE;j++){      // Total Cycles: 752000,  Execs: 5000,  Iters: 5
                
                //                    for(k=1;k<=SIZE;k++)    // Total Cycles: 722500,  Execs: 25000, Iters: 5
                //                        c[i][j] += a[i][k] * b[k][j];
                bool ack = sw_mst->MstBusRequest(id, false, HW_ADDR_c, 1);
                if(ack){
                    sw_mst->MstWriteData(ADDR_begin+72+i*6+j);
                }
                //cout<< "cacaulate c at time "<<n<<endl;
                ack = sw_mst->MstBusRequest(id, false, HW_ADDR_a, 1);
                if(ack){
                    sw_mst->MstWriteData(ADDR_begin+i*6+1);
                }
                //cout<< "cacaulate c at time "<<n+1<<endl;
                ack = sw_mst->MstBusRequest(id, false, HW_ADDR_b, 1);
                if(ack){
                    sw_mst->MstWriteData(ADDR_begin+36+(SIZE+1)*1+j);
                }
                //cout<< "cacaulate c at time "<<n+2<<endl;
                ack = sw_mst->MstBusRequest(id, false, HW_ADDR_op, 1);
                if(ack){
                    sw_mst->MstWriteData(2);
                }
                
                unsigned int done = 0;
                do{
                    ack = sw_mst->MstBusRequest(id, true, HW_ADDR_done, 1);
                    if(ack){
                        sw_mst->MstReadData(done);
                    }
                    
                }while (done!=1);
                
            }
        }
        
        
        //----------comment for loop2 ends here
        
    }
    
    cout<<endl<<"YES finished! TIME@ "<<sc_time_stamp()<<" in 200MHz clock"<<endl;
    //wait(5000, SC_NS);
    sc_stop();
}