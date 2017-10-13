#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>



using namespace std;
#define ADDU 1
#define SUBU 3
#define AND 4
#define OR  5
#define NOR 7
#define MemSize 65536 // memory size, in reality, the memory size should be 2^32, but for this lab, for the space resaon, we keep it as this large number, but the memory is still 32-bit addressable.

//test
void testRF();

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
               switch((int)ALUOP.to_ulong())
               {
                   //I-types
                   case 0x21: ALUresult = (int)oprand1.to_ulong() + (int)oprand2.to_ulong();//addu
                   case 0x23: ALUresult = (int)oprand1.to_ulong() - (int)oprand2.to_ulong();//subu
                   case 0x24: ALUresult = (int)oprand1.to_ulong() & (int)oprand2.to_ulong();//and
                   case 0x25: ALUresult = (int)oprand1.to_ulong() | (int)oprand2.to_ulong();//or
                   case 0x27: ALUresult = ~(int)oprand1.to_ulong() | ~(int)oprand2.to_ulong();//nor
                   //R-types
                   /*case 0x09: ALUresult =
                   case 0x04: ALUresult =
                   case 0x23: ALUresult =
                   case 0x2B: ALUresult =*/
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
                  
                  fileout.open("Results.txt",std::ios_base::app);
                  if (fileout.is_open())
                  {
                  
                  fileout <<pc<<' '<<WrRFAdd<<' '<<WrRFData<<' '<<RFWrEn<<' '<<WrMemAdd<<' '<<WrMemData<<' '<<WrMemEn << endl;
                     
                  }
                  else cout<<"Unable to open file";
                  fileout.close();

}

void controlUnit()
{
    
}

   
int main()
{   
  bitset<32> pc=0;
  RF myRF;
  ALU myALU;
  INSMem myInsMem;
  DataMem myDataMem;
  int i = 0;
  while (i < 20) //each loop body corresponds to one clock cycle.
  {
    pc = (int)pc.to_ulong() + 4;
    
    
    i++;
  
  
  // At the end of each cycle, fill in the corresponding data into "dumpResults" function to output files.
  // The first dumped pc value should be 0.
  //dumpResults(pc, 0, 0, 1, 0, 0, 0);

  }
  testRF();
  //myRF.OutputRF(); // dump RF;
  //myDataMem.OutputDataMem(); // dump data mem

  return 0;
        
}

//testing I/O of Register file
void testRF(){
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
