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
    Int_t ngen;
    Int_t gen_id[MAXGENPART];
    Float_t gen_pt[MAXGENPART], gen_eta[MAXGENPART], gen_phi[MAXGENPART], gen_en[MAXGENPART];
    Int_t ntk;
    Float_t tk_pt[MAXGENPART], tk_eta[MAXGENPART], tk_phi[MAXGENPART], tk_chi2[MAXGENPART],tk_nhits[MAXGENPART];
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
    t->Branch("gen_id",     hgcEvt.gen_id,     "gen_id[ngen]/I");
    t->Branch("gen_pt",     hgcEvt.gen_pt,     "gen_pt[ngen]/F");
    t->Branch("gen_eta",    hgcEvt.gen_eta,    "gen_eta[ngen]/F");
    t->Branch("gen_phi",    hgcEvt.gen_phi,    "gen_phi[ngen]/F");
    t->Branch("gen_en",     hgcEvt.gen_en,     "gen_en[ngen]/F"); 

    t->Branch("ntk",       &hgcEvt.ntk,        "ntk/I");
    t->Branch("tk_pt",      hgcEvt.tk_pt,      "tk_pt[ntk]/I");
    t->Branch("tk_eta",     hgcEvt.tk_eta,     "tk_eta[ntk]/F");
    t->Branch("tk_phi",     hgcEvt.tk_phi,     "tk_phi[ntk]/F");
    t->Branch("tk_chi2",    hgcEvt.tk_chi2,    "tk_chi2[ntk]/F");
    t->Branch("tk_nhits",   hgcEvt.tk_nhits,   "tk_nhits[ntk]/F"); 
    
    t->Branch("nhits",     &hgcEvt.nhits,      "nhits/I");
    t->Branch("hittype",    hgcEvt.hit_type,   "hit_type[nhits]/I");
    t->Branch("hitlayer",   hgcEvt.hit_layer,  "hit_layer[nhits]/I");
    t->Branch("hitx",       hgcEvt.hit_x,      "hit_x[nhits]/I");
    t->Branch("hity",       hgcEvt.hit_y,      "hit_y[nhits]/I");
    t->Branch("hitz",       hgcEvt.hit_z,      "hit_z[nhits]/I");
    t->Branch("hitedep",    hgcEvt.hit_edep,   "hit_edep[nhits]/F");
  }
  

}


#endif
