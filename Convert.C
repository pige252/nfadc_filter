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
  TCanvas *c1 = new TCanvas("c1", "KIMS", 800, 600);
  c1->Divide(1,1);
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
  

  for(int j=0;j<nevent;j++){
    nbytes +=CSI->GetEvent(j);
    hoscd1->Reset(); hoscd2->Reset();
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
    c1->cd(1);
    hoscd1->Draw();
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









