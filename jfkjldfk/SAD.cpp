//
//  SAD.cpp
//  ECE576b hw1
//
//  Created by Jiyang on 16/1/28.
//  Copyright © 2016 Jiyang. All rights reserved.
//

#include <systemc.h>
#include <fstream>

#define NUM_BLOCKS 64
#define BLOCK_SIZE 64
#define INPUT1_ADDR 0
#define INPUT2_ADDR 16384
#define SAD_OUTPUT_ADDR 32768
#define MEM_SIZE 65536

using namespace std;





class simple_mem_if  : virtual public sc_interface
{
public:
    virtual bool Write ( unsigned  int addr,  unsigned  int data )  =  0 ;
    virtual bool Read ( unsigned  int addr,  unsigned int & data )  =  0 ;
} ;


//START OF SAD
class SAD : public sc_module{
public:
    int i, v;
    unsigned  int block;
    unsigned  int sad;
    sc_port<simple_mem_if>  ioPort;
    int addr1,addr2,addrOut;
    unsigned int value_Different;
    
    SC_HAS_PROCESS(SAD);
    SAD(sc_module_name name) : sc_module(name)
    {
        SC_THREAD(main);
    }
    
    void main(){
        unsigned int readIn1,readIn2;
        
        for  ( block = 0 ; block<NUM_BLOCKS; block ++)
        {
            wait(5,SC_NS);
            sad  =  0 ;
            for  (  i = 0 ; i<BLOCK_SIZE; i ++)
            {
                wait(5,SC_NS);
                value_Different = 0;
                addr1 = block * BLOCK_SIZE+i;
                addr2 = addr1;
                wait(10,SC_NS);
                addr1 = addr1 + INPUT1_ADDR;
                wait(5,SC_NS);
                addr2 = addr2 +INPUT2_ADDR;
                wait(5,SC_NS);
//                cout<<"START READING 1 at "<< sc_time_stamp()<<endl;
                ioPort->Read(addr1,readIn1);
//                cout<<"END READING 1 at "<< sc_time_stamp()<<endl;

                ioPort->Read(addr2,readIn2);
//                cout<<"END READING 2 at "<< sc_time_stamp()<<endl;

                
//                ioPort->Read(INPUT1_ADDR +( block * BLOCK_SIZE )+ i,readIn1);
//                
//                ioPort->Read(INPUT2_ADDR +( block * BLOCK_SIZE )+ i,readIn2);
                
//                value_Different = readIn1>=readIn2?readIn1-readIn2:readIn2-readIn1;
                
                if (readIn1>=readIn2) {
                    wait(5,SC_NS);
                    value_Different = readIn1-readIn2;
                }else{
                    wait(5,SC_NS);
                    value_Different = readIn2-readIn1;
                }
                
                wait(5,SC_NS);
                sad +=value_Different;
                wait(10,SC_NS);
            }
            addrOut =SAD_OUTPUT_ADDR + block;
            wait(5,SC_NS);
            ioPort->Write( addrOut,sad);
//            ioPort->Write( SAD_OUTPUT_ADDR + block,sad);
            cout<<sad <<" at "<<block<<" time "<<sc_time_stamp()<<endl;
            cout.flush();
            
        }
        
    }
    
};
//END OF SAD

//START OF MEM_RTL
class MEMORY_RTL: public sc_module{
public:
    sc_in<sc_logic> Clk;
    sc_in<sc_logic> Ren,Wen;
    sc_in<unsigned int> Addr,DataIn;
    sc_out<unsigned int> DataOut;
    sc_out<sc_logic> Ack;
    
    unsigned int memData[MEM_SIZE];
    
    SC_HAS_PROCESS(MEMORY_RTL);
    
    MEMORY_RTL ( sc_module_name name,
                const char * memInitFilename )  : sc_module ( name )
    {
        ifstream fileIn(memInitFilename);
        int mem_cnt,dataint;
        mem_cnt =0;
        if(!fileIn.is_open()){
            cout<<"not read file yet "<<endl;
            return;
        }
        
        
        while (fileIn >> dataint) {
            if (MEM_SIZE<=mem_cnt) {
                cout<<"Data larger than MEM_SIZE at "<<mem_cnt<<endl;
                return;
            }
            memData[mem_cnt++] = dataint;
            //cout<< dataint<<" ";
        }
        
        
        
        SC_METHOD ( rtl ) ;
        sensitive << Clk. pos () ;
        
    }
    
    void rtl(){         //to do!!!!!!!!!!!!!
//        Ack.write(sc_dt::SC_LOGIC_0);
        
        
        if (Ren.read()==sc_dt::SC_LOGIC_1&&Addr.read()<MEM_SIZE) {
//            cout<<"reading at  "<<memData[Addr.read()]<<endl;
                Ack.write(sc_dt::SC_LOGIC_1);
                DataOut.write(memData[Addr.read()]);
            
        }else if (Wen.read()==sc_dt::SC_LOGIC_1&&Addr.read()<MEM_SIZE) {
            
//            cout<<"write at  "<<memData[Addr.read()]<<endl;
                Ack.write(sc_dt::SC_LOGIC_1);
                memData[Addr.read()] = DataIn.read();
            
        }else{
            
            Ack.write(sc_dt::SC_LOGIC_0);
        }
    }
    
    
};
//END OF MEM_RTL



//START OF MEMORY COMPONENT
class memory : public sc_module, public simple_mem_if{
public:
    sc_signal<sc_logic> Ren, Wen,Clk,Ack;
    sc_signal<unsigned int> Addr,DataIn,DataOut;
    
    
    
    SC_HAS_PROCESS(memory);
    
    memory(sc_module_name name, std::string const& filename) : sc_module(name) {
        
        MEMORY_RTL *mem_rtl;
        mem_rtl= new MEMORY_RTL("Rtl_NAME",filename.c_str());
        mem_rtl->Ren(Ren);
        mem_rtl->Wen(Wen);
        mem_rtl->Clk(Clk);
        mem_rtl->Addr(Addr);
        mem_rtl->DataIn(DataIn);
        mem_rtl->DataOut(DataOut);
        mem_rtl->Ack(Ack);
        
        SC_THREAD(oscillator);
        
    }
    
    //to do!!!!!!!!!!!!!

    bool Write(unsigned int addr, unsigned int dataint){
//        if (addr>=MEM_SIZE) {
//            return  false;
//        }
        Addr.write(addr);
        DataIn.write(dataint);
        Wen.write(sc_dt::SC_LOGIC_1);
        wait(Clk.posedge_event());
        Wen.write(sc_dt::SC_LOGIC_0);
        wait(Clk.posedge_event());
        
        
        if (Ack.read()==sc_dt::SC_LOGIC_0) {
            cout<<"Address exceed the memory capacity in WRITE"<<endl;
            return false;
        }
        
        
//        cout<<"A write operation "<<sc_time_stamp()<<endl;
//        wait(Clk.posedge_event());
        
        
        return true;
    }
    
    
    //to do!!!!!!!!!!!!!
    bool Read(unsigned int addr, unsigned int &data){
//        if (addr>=MEM_SIZE) {
//            return  false;
//        }
        Addr.write(addr);
        Ren.write(sc_dt::SC_LOGIC_1);
        
        wait(Clk.posedge_event());
        Ren.write(sc_dt::SC_LOGIC_0);
        wait(Clk.posedge_event());
        
        
        if (Ack.read()==sc_dt::SC_LOGIC_0) {
            cout<<"Address exceed the memory capacity in READ"<<endl;
            return false;
        }else{
            data = DataOut.read();
        }
        

        return true;
    }
    
    
    
    void oscillator()
    {
        while(true) {
            Clk.write(SC_LOGIC_0);
            wait(2, SC_NS);
            Clk.write(SC_LOGIC_1);
            wait(2, SC_NS);
        }
    }
    
    
};
//END OF MEMORY

//TOP module to integrate all the components
class top_module:public sc_module{
public:
    SAD *SAD_inst;
    memory *mem_inst;
    sc_signal<sc_dt::sc_logic> clk;
    
    
    top_module(sc_module_name name,  std::string const& file) :sc_module(name){
        mem_inst = new memory("MEM1",file);
        SAD_inst =new SAD("SAD1");
        SAD_inst->ioPort(*mem_inst);
        
    }
    
};



int sc_main(int argc, char* argv[]){
    
    if (argc<=1) {
        cout<<"usage: SAD mem_filename"<<endl;
        exit(EXIT_FAILURE);
    }
    
    
    top_module top_module_inst("Top_inst_1", argv[1]);
    sc_start();
//    wait(500000,SC_NS);
//    sc_stop();
    
    
    return 0;
}