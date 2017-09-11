#include <stdio.h>
#include <string.h>
#include <Riostream.h>
#include <math.h>
//
#include "TROOT.h"
#include "TFile.h"
#include "TH2.h"
#include "TNtuple.h"
#include "TTree.h"
#include "TBranch.h"
#include "TLeaf.h"
#include "TClonesArray.h"
#include "TGFileDialog.h"
#include <TGLabel.h>
#include "TSpline.h"

//const int CHNUMCAMAC = 2000;

//*****************NTUPLE DEFINED******************//  
class PMT {
 public:
  Float_t t0,t1,rqc,rqcped,rqcp,rqcn,rqt,rqtp,qc,qt;
  Float_t ped,pedrms;
//  Float_t adc;
  Float_t maxq,maxt;
  Int_t nc;
  Int_t   nclst;
  Float_t tpeak[BINNUM],clust[BINNUM],height[BINNUM],width[BINNUM],rms[BINNUM],time[BINNUM];
};

class DET {
 public:
  Int_t iev;
  Int_t trfh;//sec units 
  Int_t trfl;//nano sec units 
  Int_t nc;
  Int_t TrgType;
  Float_t t0,t1,rqc,rqcped,rqcp,rqcn,qc;
  Float_t mt,rms;
  Float_t Egpib,Epegpib,Enegpib;
  Float_t clust12,qcovnc;
  PMT     pmt[NPMT];
};

DET det[MAXDET];

int ReadData(TTree *tree, TObjArray *leaves, CsI& csi, int np);
int ReadHead(TTree *tree, TObjArray *leaves, head& head, int np);
int Pedestal(float*, int, int, int, float, float*, float*,int);
int MakeCluster(float*, float, int*, int*, float*, float*, float*, float*, float*);
void READinFile();
void DefineNTUPLE(TTree*);

char infile[255],outfile[255],calfile[255];

const int UPDATE = 1000;

float THV[MAXDET][NPMT], MAXQ;
int MAXEVENTS;
//float POISSONpara[MAXDET][NPMT][5];
float PMTgain[MAXDET][NPMT];

Int_t nCrystal;
