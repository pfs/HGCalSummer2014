#ifndef HGCEventSummary_h
#define HGCEventSummary_h

#include "TTree.h"


/**
   @struct HGCEvent_t
   @short stores basic information of an event
   @author P. Silva (psilva@cern.ch)
 */


namespace HGCEvent
{

#define MAXGENPART 100
#define MAXHITS  10000
  
  struct HGCEvent_t
  {
    Int_t run,lumi,event;
    Int_t ngen,gen_nbrem,gen_nconv;
    Int_t gen_id[MAXGENPART],gen_vtx[MAXGENPART],gen_vty[MAXGENPART],gen_vtz[MAXGENPART];
    Float_t gen_pt[MAXGENPART], gen_eta[MAXGENPART], gen_phi[MAXGENPART], gen_en[MAXGENPART];
    Int_t ntk;
    Float_t tk_pt[MAXGENPART], tk_eta[MAXGENPART], tk_phi[MAXGENPART], tk_chi2[MAXGENPART],tk_nhits[MAXGENPART];
    Float_t tk_eehit_x[MAXGENPART][31],  tk_eehit_y[MAXGENPART][31];
    Float_t tk_hefhit_x[MAXGENPART][12], tk_hefhit_y[MAXGENPART][12];
    Float_t tk_hebhit_x[MAXGENPART][10], tk_hebhit_y[MAXGENPART][10];
    Int_t nhits;
    Int_t hit_type[MAXHITS], hit_layer[MAXHITS];
    Float_t hit_x[MAXHITS], hit_y[MAXHITS], hit_z[MAXHITS], hit_edep[MAXHITS];
  };

  /**
     @short creates the branches of the tree
   */
  void createTree(TTree *t, HGCEvent::HGCEvent_t &hgcEvt)
  {
    t->Branch("run",       &hgcEvt.run,        "run/I");
    t->Branch("lumi",      &hgcEvt.lumi,       "lumi/I");
    t->Branch("event",     &hgcEvt.event,      "event/I");
    
    t->Branch("ngen",      &hgcEvt.ngen,       "ngen/I");
    t->Branch("gen_nbrem", &hgcEvt.gen_nbrem,  "gen_nbrem/I");
    t->Branch("gen_nconv", &hgcEvt.gen_nconv,  "gen_nconv/I");
    t->Branch("gen_id",     hgcEvt.gen_id,     "gen_id[ngen]/I");
    t->Branch("gen_vtx",    hgcEvt.gen_vtx,    "gen_vtx[ngen]/I");
    t->Branch("gen_vty",    hgcEvt.gen_vty,    "gen_vty[ngen]/I");
    t->Branch("gen_vtz",    hgcEvt.gen_vtz,    "gen_vtz[ngen]/I");
    t->Branch("gen_pt",     hgcEvt.gen_pt,     "gen_pt[ngen]/F");
    t->Branch("gen_eta",    hgcEvt.gen_eta,    "gen_eta[ngen]/F");
    t->Branch("gen_phi",    hgcEvt.gen_phi,    "gen_phi[ngen]/F");
    t->Branch("gen_en",     hgcEvt.gen_en,     "gen_en[ngen]/F"); 

    t->Branch("ntk",         &hgcEvt.ntk,          "ntk/I");
    t->Branch("tk_pt",        hgcEvt.tk_pt,        "tk_pt[ntk]/F");
    t->Branch("tk_eta",       hgcEvt.tk_eta,       "tk_eta[ntk]/F");
    t->Branch("tk_phi",       hgcEvt.tk_phi,       "tk_phi[ntk]/F");
    t->Branch("tk_chi2",      hgcEvt.tk_chi2,      "tk_chi2[ntk]/F");
    t->Branch("tk_nhits",     hgcEvt.tk_nhits,     "tk_nhits[ntk]/F"); 
    t->Branch("tk_eehit_x",   hgcEvt.tk_eehit_x,   "tk_eehit_x[ntk][31]/F"); 
    t->Branch("tk_eehit_y",   hgcEvt.tk_eehit_y,   "tk_eehit_y[ntk][31]/F"); 
    t->Branch("tk_hefhit_x",  hgcEvt.tk_hefhit_x,  "tk_hefhit_x[ntk][12]/F"); 
    t->Branch("tk_hefhit_y",  hgcEvt.tk_hefhit_y,  "tk_hefhit_y[ntk][12]/F"); 
    t->Branch("tk_hebhit_x",  hgcEvt.tk_hebhit_x,  "tk_hebhit_x[ntk][10]/F"); 
    t->Branch("tk_hebhit_y",  hgcEvt.tk_hebhit_y,  "tk_hebhit_y[ntk][10]/F"); 
    
    t->Branch("nhits",     &hgcEvt.nhits,      "nhits/I");
    t->Branch("hit_type",   hgcEvt.hit_type,   "hit_type[nhits]/I");
    t->Branch("hit_layer",  hgcEvt.hit_layer,  "hit_layer[nhits]/I");
    t->Branch("hit_x",      hgcEvt.hit_x,      "hit_x[nhits]/F");
    t->Branch("hit_y",      hgcEvt.hit_y,      "hit_y[nhits]/F");
    t->Branch("hit_z",      hgcEvt.hit_z,      "hit_z[nhits]/F");
    t->Branch("hit_edep",   hgcEvt.hit_edep,   "hit_edep[nhits]/F");
  }

  /**
     @short creates the branches of the tree
   */
  void attachToTree(TTree *t, HGCEvent::HGCEvent_t &hgcEvt)
  {
    t->SetBranchAddress("run",       &hgcEvt.run);
    t->SetBranchAddress("lumi",      &hgcEvt.lumi);
    t->SetBranchAddress("event",     &hgcEvt.event);
    
    t->SetBranchAddress("ngen",      &hgcEvt.ngen);
    t->SetBranchAddress("gen_nbrem", &hgcEvt.gen_nbrem);
    t->SetBranchAddress("gen_nconv", &hgcEvt.gen_nconv);
    t->SetBranchAddress("gen_id",     hgcEvt.gen_id);
    t->SetBranchAddress("gen_vtx",    hgcEvt.gen_vtx);
    t->SetBranchAddress("gen_vty",    hgcEvt.gen_vty);
    t->SetBranchAddress("gen_vtz",    hgcEvt.gen_vtz);
    t->SetBranchAddress("gen_pt",     hgcEvt.gen_pt);
    t->SetBranchAddress("gen_eta",    hgcEvt.gen_eta);
    t->SetBranchAddress("gen_phi",    hgcEvt.gen_phi);
    t->SetBranchAddress("gen_en",     hgcEvt.gen_en);

    t->SetBranchAddress("ntk",         &hgcEvt.ntk);
    t->SetBranchAddress("tk_pt",        hgcEvt.tk_pt);
    t->SetBranchAddress("tk_eta",       hgcEvt.tk_eta);
    t->SetBranchAddress("tk_phi",       hgcEvt.tk_phi);
    t->SetBranchAddress("tk_chi2",      hgcEvt.tk_chi2);
    t->SetBranchAddress("tk_nhits",     hgcEvt.tk_nhits);
    t->SetBranchAddress("tk_eehit_x",   hgcEvt.tk_eehit_x);
    t->SetBranchAddress("tk_eehit_y",   hgcEvt.tk_eehit_y);
    t->SetBranchAddress("tk_hefhit_x",  hgcEvt.tk_hefhit_x);
    t->SetBranchAddress("tk_hefhit_y",  hgcEvt.tk_hefhit_y);
    t->SetBranchAddress("tk_hebhit_x",  hgcEvt.tk_hebhit_x);
    t->SetBranchAddress("tk_hebhit_y",  hgcEvt.tk_hebhit_y);
    
    t->SetBranchAddress("nhits",     &hgcEvt.nhits);
    t->SetBranchAddress("hit_type",   hgcEvt.hit_type);
    t->SetBranchAddress("hit_layer",  hgcEvt.hit_layer);
    t->SetBranchAddress("hit_x",      hgcEvt.hit_x);
    t->SetBranchAddress("hit_y",      hgcEvt.hit_y);
    t->SetBranchAddress("hit_z",      hgcEvt.hit_z);
    t->SetBranchAddress("hit_edep",   hgcEvt.hit_edep);
  }
  

}


#endif
