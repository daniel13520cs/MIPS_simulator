#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>

#define ADDU 1
#define SUBU 3
#define AND 4
#define OR  5
#define NOR 7

#define ADDIU 9
using namespace std;
#define MemSize 65536 // memory size, in reality, the memory size should be 2^32, but for this lab, for the space resaon, we keep it as this large number, but the memory is still 32-bit addressable.

/* opcode/Function ************************************************/
/*//R-Type
#define ADDU 0x21
#define SUBU 0x23
#define AND 0x24
#define OR 0x25
#define NOR 0x27
//I-Type
#define ADDIU 0x09
#define BEQ 0x04
#define LW 0x23
#define SW 0x2B
//J-Type
#define J 0x02
#define HALT 0x3F*/
//*******************************************************************/

//test
void testtest();
void testRF();
void testALU();
void testMem();

//checkt multiple bits
u_long test(bitset<32> in, int start, int end);

class RF
{
    public:
        bitset<32> ReadData1, ReadData2; 
     	RF()
    	{
          Registers.resize(32);
          Registers[0] = bitset<32> (0);
        }
  
        void ReadWrite(bitset<5> RdReg1, bitset<5> RdReg2, bitset<5> WrtReg, bitset<32> WrtData, bitset<1> WrtEnable)
        {
          // implement the funciton by you.
          int RdReg1_int = (int)(RdReg1.to_ulong());
          int RdReg2_int = (int)(RdReg2.to_ulong());
          int WrtReg_int = (int)(WrtReg.to_ulong());
          
          /*IMPORTANT NOTE: indside the register file, perform write before the read operation, otherwise the reg value is not updated with the clk cycle*/
          /*write operation*/
          Registers[WrtReg_int] = (WrtEnable == true)? WrtData : Registers[WrtReg_int];
          //hard code $r0 = 0 in case writting value to $r0
          Registers[0] = 0x0;
          
          /*read operation*/
          ReadData1 = Registers[RdReg1_int];
          ReadData2 = Registers[RdReg2_int];
         }
		 
	void OutputRF()
             {
               ofstream rfout;
               //erase the last result
               rfout.open("RFresult.txt",std::ofstream::out | std::ofstream::trunc);
                  if (rfout.is_open())
                  {
                    rfout<<"A state of RF:"<<endl;
                  for (int j = 0; j<32; j++)
                      {
                        //modified std:hex to print hex
                        rfout << (int)Registers[j].to_ulong()<<endl;
                      }
                     
                  }
                  else cout<<"Unable to open file";
                  rfout.close();
               
               }     
	private:
            vector<bitset<32> >Registers;
    
};

class ALU
{
      public:
             bitset<32> ALUresult;
             bitset<32> ALUOperation (bitset<3> ALUOP, bitset<32> oprand1, bitset<32> oprand2)
             {   
               // implement the ALU operations by you.
               switch(ALUOP.to_ulong())
               {
                 //R-types
                 case ADDU: ALUresult = oprand1.to_ulong() + oprand2.to_ulong(); break;//addu
                 case SUBU: ALUresult = oprand1.to_ulong() - oprand2.to_ulong(); break;//subu
                 case AND: ALUresult = oprand1.to_ulong() & oprand2.to_ulong(); break;//and
                 case OR: ALUresult = oprand1.to_ulong() | oprand2.to_ulong(); break;//or
                 case NOR: ALUresult = ~(int)oprand1.to_ulong() | ~(int)oprand2.to_ulong(); break;//nor
                 //I-types
                 //the result of SignExtension comes from oprand2
                 case 0x9: ALUresult = ALUresult = (int)oprand1.to_ulong() + (int)oprand2.to_ulong(); break;//addiu
                 //J-types and branch
                 default: ALUresult = NULL;//branch, jump, halt dont' care
               }
               return ALUresult;
             }

};

class INSMem
{
      public:
          bitset<32> Instruction;
          INSMem()
          {       IMem.resize(MemSize); 
                  ifstream imem;
                  string line;
                  int i=0;
                  imem.open("imem.txt");
                  if (imem.is_open())
                  {
                  while (getline(imem,line))
                     {
                        IMem[i] = bitset<8>(line.substr(0,8)); //modified line
                        i++;
                     }
                     
                  }
                  else cout<<"Unable to open file";
                  imem.close();
                     
                  }
                  
          bitset<32> ReadMemory (bitset<32> ReadAddress) 
              {    
                // implement by you. (Read the byte at the ReadAddress and the following three byte).
                u_long instru = 0;
                instru += IMem[ReadAddress.to_ulong()].to_ulong() << 24;
                instru += IMem[ReadAddress.to_ulong()+1].to_ulong() << 16;
                instru += IMem[ReadAddress.to_ulong()+2].to_ulong() << 8;
                instru += IMem[ReadAddress.to_ulong()+3].to_ulong();
                Instruction = (bitset<32>) instru;
                return Instruction;
              }     
      
      private:
           vector<bitset<8> > IMem;
      
};
      
class DataMem    
{
      public:
          bitset<32> readdata;  
          DataMem()
          {
             DMem.resize(MemSize); 
             ifstream dmem;
                  string line;
                  int i=0;
                  dmem.open("dmem.txt");
                  if (dmem.is_open())
                  {
                  while (getline(dmem,line))
                       {      
                        DMem[i] = bitset<8>(line.substr(0,8));//modified line
                        i++;
                       }
                  }
                  else cout<<"Unable to open file";
                  dmem.close();
          
          }  
          bitset<32> MemoryAccess (bitset<32> Address, bitset<32> WriteData, bitset<1> readmem, bitset<1> writemem) 
          {
            // implement by you.
            if(writemem == 1) {
              DMem[Address.to_ulong()] = test(WriteData, 31, 23);
              DMem[Address.to_ulong()+1] = test(WriteData, 23, 16);
              DMem[Address.to_ulong()+2] = test(WriteData, 15, 8);
              DMem[Address.to_ulong()+3] = test(WriteData, 7, 0);
            }
            
            //fetch data from dMem
            if(readmem == 0){return readdata;}
            u_long data = 0;
            data += DMem[Address.to_ulong()].to_ulong() << 24;
            data += DMem[Address.to_ulong()+1].to_ulong() << 16;
            data += DMem[Address.to_ulong()+2].to_ulong() << 8;
            data += DMem[Address.to_ulong()+3].to_ulong();
            readdata = (bitset<32>) data;
            return readdata;
          }   
                     
          void OutputDataMem()
          {
               ofstream dmemout;
                  dmemout.open("dmemresult.txt");
                  if (dmemout.is_open())
                  {
                  for (int j = 0; j< 1000; j++)
                       {     
                        dmemout << DMem[j]<<endl;
                       }
                     
                  }
                  else cout<<"Unable to open file";
                  dmemout.close();
               
               }             
      
      private:
           vector<bitset<8> > DMem;
  
};  

void dumpResults(bitset<32> pc, bitset<5> WrRFAdd, bitset<32> WrRFData, bitset<1> RFWrEn, bitset<32> WrMemAdd, bitset<32> WrMemData, bitset<1> WrMemEn)
{
                  ofstream fileout;
                  
                  fileout.open("Results.txt",std::ofstream::app);
                  if (fileout.is_open())
                  {
                  
                  fileout <<"pc"<<pc<<"\n"<<"WrRFAdd"<<WrRFAdd<<"\n"<<"WrRFData"<<WrRFData<<"\n"<<"RFWrEn"<<RFWrEn<<"\n"<<"WrMemAdd"<<WrMemAdd<<"\n"
                    <<"WrMemData"<<WrMemData<<"\n"<<"WrMemEn"<<WrMemEn<<"\n"<< endl;
                     
                  }
                  else cout<<"Unable to open file";
                  fileout.close();
}

class Control{
  
  public: Control(){
    this -> Inst = NULL;
  }
  
  public:
    bitset<32> Inst;
    bitset<1> RegDst;
    bitset<1> WrtEnable;
    bitset<3> ALUOp;
    bitset<1> ALUSrc;
    bitset<1> memToReg;
    bitset<1> memRead;
    bitset<1> memWrite;
  
    //IF**********************************************************************/
    void setInst(bitset<32> Inst){
      this -> Inst = Inst;
    }
  
    /*Reg/ALU************************************************************************/
    void RegDstSig()
    {
      //
      if(this -> Inst == 0x0) {
        this -> RegDst = 0x0;
      } else {
        this -> RegDst = 0x1;
      }
    }
  
    void WrtEnableSig(){
      this -> WrtEnable = ~(isStore() | isBranch() | jType());
    }
  
    bool isStore(){
      //whether the inst is SW or not
      return (test(this->Inst, 31, 26) == 0x2B); //SW
    }
  
    bool isBranch(){
      return (test(this -> Inst,31,26) == 0x4);
    }
    bool jType(){
      return (test(this -> Inst,31,26) == 0x2);
    }
  
    void ALUSrcSig(){
      this -> ALUSrc = (test(Inst,31,26)==0x0);
    }
    void ALUOpSig()
    {
      bitset<32> Inst = this -> Inst;
      bitset<3> ALUop = NULL;
      if ((test(Inst,31,26) == 0x23) | (test(Inst,31,26) == 0x2B)){ //LW or SW
        ALUop = 0x1;
      } else if (test(Inst,31,26)==0x0){
        ALUop = test(Inst,2,0);
      } else {
        ALUop = test(Inst,28,26);
      }
    }
    
  
    /* MEM/WR **********************************************************************/
    void memToRegSig()
    {
      //whether the inst is LW or not
      this -> memToReg = (test(this->Inst, 31, 26) == 0x23); //LW
    }
  
    void memReadSig(){
      this -> memRead = (test(this->Inst, 31, 26) == 0x23);
    }
  
    void memWriteSig(){
      //whether the inst is SW or not
      this -> memWrite = (test(this->Inst, 31, 26) == 0x2B); //SW
    }
  
};

bitset<32> two_to_one_mux_32(bitset<1> sel, bitset<32> first, bitset<32> second){
  return (sel == 0)? first : second;
}
bitset<5> two_to_one_mux_5(bitset<1> sel, bitset<5> first, bitset<5> second){
  return (sel == 0)? first : second;
}

bitset<32> signExtension(bitset<16> immediate) {
  int imm = (int)immediate.to_ulong();
  int value = (0x0000FFFF & imm);
  int mask = 0x00008000;
  if (mask & imm) {
    value += 0xFFFF0000;
  }
  return value;
}

u_long test(bitset<32> in, int start, int end)
{
  u_long res = 0;
  for (int i=start; i>=end; i--){
    res = (res << 1) + in.test(i);
  }
  return res;
}

int main()
{
  bitset<32> pc=0;
  bitset<32> newInst = 0;
  RF myRF;
  ALU myALU;
  INSMem myInsMem;
  DataMem myDataMem;
  //stage2
  bitset<32> signExtendImm;
  bitset<32> WrtData;
  bitset<5> WrtReg;
  //stage3
  bitset<32> WrMemAdd;
  bitset<32> WrMemData;
  
  Control* control1 = new Control();
  Control* control2 = new Control();
  Control* control3 = new Control();
  
  int i = 0;
  while (i < 20) //each loop body corresponds to one clock cycle.
  {
    /*The architectural state consists of the Program Counter (PC), the Register File (RF) and the Data Memory (DataMem). control-unit*/
    /*The first stage (Stage1) of
    the pipeline performs instruction fetch (IF). The second stage (Stage2) performs instruction decode/RF read (ID/RF)
   and execute (EX). The third stage (Stage3) performs data memory load/store (MEM) and writes back to the RF (WB).*/
    
    
    //stage 3
    control3->setInst(control2->Inst);
    control3-> memToRegSig();
    control3-> memReadSig();
    control3-> memWriteSig();
    control3-> WrtEnableSig();
    control3->RegDstSig();
    WrMemAdd = myALU.ALUresult;
    WrMemData = myRF.ReadData2;
    myDataMem.MemoryAccess(WrMemAdd, WrMemData, control3->memRead, control3->memWrite);
    //Wr back mux (sel, 0, 1)
    WrtData = two_to_one_mux_32(control3->memToReg, WrMemData, myDataMem.readdata);
    WrtReg = two_to_one_mux_5(control3->RegDst, test(control3->Inst, 15, 11), test(control3->Inst, 20, 16));
    
    //stage 2
    control2->setInst(control1->Inst);
    control2->ALUOpSig();
    control2->ALUSrcSig();
    signExtendImm = signExtension((int)test(control2->Inst,15,0));
    two_to_one_mux_32(control2->ALUSrc, signExtendImm, myRF.ReadData2);
    myRF.ReadWrite(test(control2->Inst,25,21), test(control2->Inst,20,16), WrtReg, WrtData, control2-> WrtEnable);
    myALU.ALUresult = myALU.ALUOperation(control2->ALUOp, myRF.ReadData1, myRF.ReadData2);
    
    
    //stage 1 (value of instruction)
    newInst = myInsMem.ReadMemory(pc);
    control1->setInst(newInst);
    cout << "newInst" << newInst <<"\n";//debug
    pc = pc.to_ulong() + 4;
    i++;
    
  
  // At the end of each cycle, fill in the corresponding data into "dumpResults" function to output files.
  // The first dumped pc value should be 0.
    dumpResults(pc, WrtReg, WrtData, control3->WrtEnable, WrMemAdd, WrMemData, control3->WrtEnable);
    //dumpResults(bitset<32> pc, bitset<5> WrRFAdd, bitset<32> WrRFData, bitset<1> RFWrEn, bitset<32> WrMemAdd(address), bitset<32> WrMemData, bitset<1> WrMemEn)
    i++;
  }
  
  myRF.OutputRF(); // dump RF;
  myDataMem.OutputDataMem(); // dump data mem
  i++;
  return 0;
        
}


//testing I/O of Register file
void testRF()
{
  RF rf;
  rf.ReadData1 = 0x0;
  rf.ReadData2 = 0x0;
  printf("ReadData1 = %d\n", (int) rf.ReadData1.to_ulong());
  printf("ReadData2 = %d\n", (int) rf.ReadData2.to_ulong());
  for(int i = 0; i <= 31; i ++){
    cout << "writing " << i << "to register " << i << "\n";
    rf.ReadWrite(i, i, i, i, 1);
    printf("rf.ReadData1 = %d\n", (int)rf.ReadData1.to_ulong());
    printf("rf.ReadData2 = %d\n", (int)rf.ReadData2.to_ulong());
  }
  printf("Writing 1 to $r0\n");
  rf.ReadWrite(0, 0, 0, 1, 1);
  printf("Regesters[0] = %d\n", (int)rf.ReadData1.to_ulong());
  rf.OutputRF();
}

//testing I/O of ALU units
void testALU()
{
  ALU alu;
  for(int i = 0; i < 10; i++){
    cout << "i = " << i << "\n";
    alu.ALUOperation(i,0x0FFFFFFF,0x0);
    cout << "ALUresult is" << alu.ALUresult << "when ALUOP is " << i << "\n";
  }
}

//test I/O of DataMem units
void testMem()
{
  DataMem mem;
  for(int i =8 ; i < 8 * 8; i*=2){
    cout << "writing" << i <<  "to Mem[" << i << "]\n";
    mem.MemoryAccess(i, i, 0, 1);
    cout << "Reading Mem[" << i << "] and its value is" << mem.MemoryAccess(i, i, 1, 0) << "\n";
  }
}

void testtest(){
  bitset<32> re = test(0x0000000F, 4, 0);
  cout << "hey it is" << re;
  bitset<32> tempp = (00000001 << 1) + 00000001;
std:cout << "test is fucking" << tempp;
}

void testSignExntension(){
  cout << signExtension(0x0001) << "\n";
  cout << signExtension(0x8000) << "\n";
}




  
