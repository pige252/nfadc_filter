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
  TCanvas *c1 = new TCanvas("c1", "KIMS", 1200, 900);
//  c1->Divide(2,2);
//  c1->Draw();
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
  int div = 5;
  float divv = 2.5*div;
  int n = DATA_POINT/div;
  float ave[n];
  float hpa=0.99;
  float hpb=(1.0+hpa)/2.0;
  float lpa=0.95;
  float lpb=1.0-lpa;
  float d_m;
  float ma_m;
  float ma[nevent][n];
  float mwd_m[n];
  float height[nevent];
//  int m = 16;
//  int l = 4;
  int m = 5000/div;
  int l = 1500/div;
//  float pz = 0.0020;
  float pz = 0.2000;
  TH1F *h0 = new TH1F("h0","average filter",n,0,n*divv);
  TH1F *h1 = new TH1F("h1","low pass filter",n,0,n*divv);
  TH1F *h2 = new TH1F("h2","high pass filter",n,0,n*divv);
  TH1F *h3 = new TH1F("h3","trapezoidal filter",n,0,n*divv);
  TH1F *h4 = new TH1F("h4","trapezoidal filter",1000,0,1000);
  Float_t ht;

  
  TFile *tp = new TFile("nfadc400_TP.root","recreate");
  TTree *td = new TTree("td","trapezoidal filter data");
  td->Branch("ht",&ht);
  
  for(int j=0;j<nevent;j++){
    nbytes +=CSI->GetEvent(j);
    h0->Reset();h1->Reset();h2->Reset();h3->Reset();

///////////////////////////////////////////////////////////////////////
//	average filter

    for(int i=0;i<n;i++){
      ave[i]=0;
      for(int k=0;k<div;k++) ave[i]+=CsI1.FADC1[div*i+k];
      ave[i]=ave[i]/div;
//      h0->Fill(i*divv+1,ave[i]);
    }
/*
///////////////////////////////////////////////////////////////////////
//	high pass filter

    hp[0]=ave[0];
    for(int i=1;i<n;i++){
      hp[i] = hpa * (hp[i-1] + ave[i] - ave[i-1]);
      h1->Fill(2.5*i+1,hp[i]);
    }
      
///////////////////////////////////////////////////////////////////////
//	low pass filter


    lp[0]=hp[0];
    for(int i=1;i<n;i++){
      lp[i] = lp[i-1] + lpa * (hp[i] - lp[i-1]);
      h2->Fill(2.5*i+1,lp[i]);
    }
*/

    float lp[n];
    float hp[n];
    float hhp[n];
    int th = 20;
    int start= 0;
    int trigger = 0;
    lp[0] = ave[0];
    hp[0] = 0.0;
    hhp[0] = 0.0;

    for(int i=1;i<n;i++){
      lp[i] = lpb*ave[i] + lpa*lp[i-1];
      hp[i] = hpb*lp[i] - hpb*lp[i-1] + hpa*hp[i-1];
      hhp[i] = hpb*hp[i] - hpb*hp[i-1] + hpa*hhp[i-1];
      if(hhp[i]>=th&&start==0) start = i;

//      h1->Fill(i*divv+1,lp[i]);
//      h2->Fill(i*divv+1,hhp[i]);
    }
    for(int i=start;i<n;i++){
        if(hhp[i]<=0){
          trigger = i;
          break;
        }
    }

///////////////////////////////////////////////////////////////////////
//	trapezoidal filter
    for(int i=m;i<trigger+2*l;i++){
//      d_m = CsI1.FADC1[i]-CsI1.FADC1[i-m];
      d_m = ave[i]-ave[i-m];
      ma_m = 0.0;
      for(int k=i-m;k<i;k++){
//        ma_m = ma_m + CsI1.FADC1[k];
        ma_m = ma_m + ave[k];
      }
//      mwd_m[i]=d_m + ((ma_m / m) - lp[k]) * pz;
      mwd_m[i]=d_m + (ma_m / m) * pz;
//      mwd_m[i]=d_m;
    }

    for(int ii=l;ii<trigger+2*l;ii++){
        ma[j][ii]=0.0;
        for(int kk=ii-l;kk<ii;kk++) ma[j][ii]=ma[j][ii]+mwd_m[kk];
	      ma[j][ii]=ma[j][ii]/(float)l;
    }
//    for(int ij=m+l;ij<n;ij++) h3->Fill(ij*divv+1,ma[ij]);
///////////////////////////////////////////////////////////////////////
//find real input data(=rawdata-ped)
///////////////////////////////////////////////////////////////////////
	  int peak = 640/div+l;//1.6us
	  int peaktime = peak + trigger;
	  float ped = 0.0;
	  int atime = 1000;
    int shift = 100;
	  for(int pednum=start-atime-shift;pednum<start-shift;pednum++){
		  ped += ma[j][pednum];
	  }
	  ped = ped/atime;

	  height[j]=ma[j][peaktime]-ped;
    ht=height[j];

    h4->Fill(height[j]);
    td->Fill();

/*
    c1->cd(1);
    h0->Draw();
    c1->cd(2);
    h1->Draw();
    c1->cd(3);
    h2->Draw();
    c1->cd(4);
    h3->Draw();
*/

//    c1->Modified();
//    c1->Update();
    gSystem->Exec("date");
    cout << j << " : " << ped << " : " << peaktime << " : " << height[j] << endl;
   // scanf("%d",&temp);
   /*
    fgets(temp,255,stdin);
    if(temp[0]=='x'||temp[0]=='X'){
      break;
    }
     //getchar();
	 */
  }
  c1->cd();
  h4->Draw();
  tp->cd();
  tp->Write();
//  hfile->Write();
//  return 1;
}
//gBenchmark->Show("bench");
//-------------------------------------