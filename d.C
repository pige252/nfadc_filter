{
//  gSystem->LoadMacro("DET16.h");
//  
  gROOT->LoadMacro("CsI.h");
  gROOT->LoadMacro("analysis.h");  
  TFile *a=new TFile("nfadc400.root");
  TTree *CSI = (TTree*)a->Get("CSI");
  CSI->GetEntry(10);
  
  TTree *HEAD = (TTree*)a->Get("head");
  HEAD->GetEntry(1);
  Int_t nevent=CSI->GetEntries();
  //int DATA_POINT = 0x4000;
  TH1F *hoscd1 = new TH1F("hoscd1", "signal 1", DATA_POINT, 0, DATA_POINT*2.5);
  TH1F *hoscd2 = new TH1F("hoscd2", "signal 2", DATA_POINT, 0, DATA_POINT*2.5);
  TCanvas *c1 = new TCanvas("c1", "KIMS", 1200, 900);
  c1->Divide(2,2);
  c1->Draw();
  CsI CsI1;
  
  DETNUM=1;
  char temp[255];
  Float_t oscc[DETNUM][NPMT][DATA_POINT];
  TObjArray *leaves=CSI->GetBranch("CsI1")->GetListOfLeaves();
  TLeaf *l_leafb  = (TLeaf*)leaves->At(0);   l_leafb->SetAddress(&CsI1.trigger);
  TLeaf *l_leafa  = (TLeaf*)leaves->At(1);   l_leafa->SetAddress(&CsI1.tag);
  TLeaf *l_leafc  = (TLeaf*)leaves->At(2);   l_leafc->SetAddress(&CsI1.FADC1);
  TLeaf *l_leafd  = (TLeaf*)leaves->At(3);   l_leafd->SetAddress(&CsI1.FADC2);
//  TLeaf *l_leafd  = (TLeaf*)leaves->At(4);   l_leafd->SetAddress(&CsI1.FADC2);
//  TLeaf *l_leafh  = (TLeaf*)leaves->At(6);   l_leafh->SetAddress(&CsI1.time);

//  TObjArray *leaves1=HEAD->GetBranch("head1")->GetListOfLeaves();

  Int_t nbytes = 0;
  Int_t hnbytes = 0;
//  int n = DATA_POINT;
  int div = 2;
  int n = DATA_POINT/div;
  float ave[n];
  float hp[n];
  float lp[n];
  float hpt=0.95;
  float lpt=0.5;
  float d_m;
  float ma_m;
  float ma[n];
  float mwd_m[n];
  int m = 16;
  int l = 4;
  float pz = 0.50;
  TH1F *h0 = new TH1F("h0","average filter",n,0,n*2.5);
  TH1F *h1 = new TH1F("h1","high pass filter",n,0,n*2.5);
  TH1F *h2 = new TH1F("h2","low pass filter",n,0,n*2.5);
  TH1F *h3 = new TH1F("h3","trapezoidal filter",n,0,n*2.5);

  for(int j=0;j<nevent;j++){
    nbytes +=CSI->GetEvent(j);
    hoscd1->Reset(); hoscd2->Reset();
    h0->Reset();h1->Reset();h2->Reset();h3->Reset();
    for(int idet=0;idet<DETNUM;idet++){
      for(int k=0;k<DATA_POINT;k++){
        oscc[idet][0][k]=(CsI1.FADC1[k]);//hdata[idet].ped[0]);
        oscc[idet][1][k]=(CsI1.FADC2[k]);//hdata[idet].ped[1]);
        hoscd1->Fill(2.5*k+1,oscc[idet][0][k]);
        hoscd2->Fill(2.5*k+1,oscc[idet][1][k]);
      }//for BINNUM
    }
    /*
    for(int i=0;i<DATA_POINT;i++){
      hoscd1->Fill(i+1,CsI1.FADC1[i]&0xFF);
      hoscd2->Fill(i+1,CsI1.FADC2[i]&0xFF);
//      hoscd1->Fill(i,CsI1.FADC1[i]&0xFF);
//      hoscd2->Fill(i,CsI1.FADC2[i]&0xFF);
    }*/
///////////////////////////////////////////////////////////////////////
//	average filter

    for(int i=0;i<n;i++){
      ave[i]=0;
      for(int k=0;k<div;k++) ave[i]+=CsI1.FADC1[div*i+k];
      ave[i]=ave[i]/div;
      h0->Fill(2.5*i+1,ave[i]);
    }

///////////////////////////////////////////////////////////////////////
//	high pass filter

    hp[0]=ave[0];
    for(int i=1;i<n;i++){
      hp[i] = hpt * (hp[i-1] + ave[i] - ave[i-1]);
      h1->Fill(2.5*i+1,hp[i]);
    }
      
///////////////////////////////////////////////////////////////////////
//	low pass filter


    lp[0]=hp[0];
    for(int i=1;i<n;i++){
      lp[i] = lp[i-1] + lpt * (hp[i] - lp[i-1]);
      h2->Fill(2.5*i+1,lp[i]);
    }


///////////////////////////////////////////////////////////////////////
//	trapezoidal filter
    for(int i=m;i<n;i++){
//      d_m = CsI1.FADC1[i]-CsI1.FADC1[i-m];
      d_m = lp[i]-lp[i-m];
      ma_m = 0.0;
      for(int k=i-m;k<i;k++){
//        ma_m = ma_m + CsI1.FADC1[k];
        ma_m = ma_m + lp[k];
      }
      mwd_m[i]=d_m + ((ma_m / m) - lp[k]) * pz;
//      mwd_m[i]=d_m;
    }

    for(int ii=l;ii<n;ii++){
	ma[ii]=0.0;
        for(int kk=ii-l;kk<ii;kk++) ma[ii]=ma[ii]+mwd_m[kk];
	ma[ii]=ma[ii]/(float)l;
    }
    for(int ij=m+l;ij<n;ij++) h3->Fill(2.5*ij+1,ma[ij]);
///////////////////////////////////////////////////////////////////////




    c1->cd(1);
    h0->Draw();
    c1->cd(2);
    h1->Draw();
    c1->cd(3);
    h2->Draw();
    c1->cd(4);
    h3->Draw();


    //    c1->cd(2);
    //hoscd2->Draw();
    c1->Modified();
    c1->Update();
  //  CsI1.tagP = (CsI1.tag130627_GAGG1_Cs137_preamp_HV1700.rootP&0x7FF);
  //  printf("evt number = %d\n tagP %d trigger position=%d %d",j,CsI1.tagP, (tagposition[0]+DATA_POINT-23936/2)%DATA_POINT, (tagposition[1]+DATA_POINT-23936/2)%DATA_POINT);
//    printf("evt number = %d\n trigger position=%d %d",j,tagposition[0], tagposition[1]);
//      CSI->AutoSave();
    gSystem->Exec("date");
   // scanf("%d",&temp);
    fgets(temp,255,stdin);
    if(temp[0]=='x'||temp[0]=='X'){
      break;
    }
     //getchar();
  }
//  hfile->Write();
//  return 1;
}
//gBenchmark->Show("bench");
//-------------------------------------









