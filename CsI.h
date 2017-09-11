#include "TObject.h"
#include "TTree.h"
const int MAXDET=20;
const int NPMT = 2;
/*
record_length      length(us)   Bin number     evt number
1                  2.56           0x400           1024  
2                  5.12           0x800            512   
4                 10.24           0x1000           256
8                 20.48           0x2000           128
16                40.96           0x4000            64
32                81.92           0x8000            32
64               163.84           0x10000           16
128              327.68           0x20000            8
256              655.36           0x40000            4
512             1310.72           0x80000            2
record_length = BINNUM/0x400
event_number  = 0x100000/BINNUM
*/
// const int BINNUM=0x80000;  
   const int BINNUM=0x8000;  //81.92us
   //const int BINNUM=0x400; //2.56us

const int DATA_POINT = BINNUM;
const int NCH = NPMT;
const int NDATA = DATA_POINT*NCH;
int DETNUM = 0;

class CsI{
 public:
  UChar_t trigger;//trigger pattern
  UChar_t tag[7];//tagging information
  Float_t FADC1[BINNUM];  // 
  Float_t FADC2[BINNUM];  // osc curve data
//  ULong64_t time;
//  Int_t rbsp;
  // member function
};

class head{
 public:
  Int_t rbsp;
  Int_t dt;
  Int_t pwc[NPMT];
  Int_t pnc[NPMT];
  Int_t pic[NPMT];
  Int_t ped[NPMT];
  Int_t thresh[NPMT];
};
