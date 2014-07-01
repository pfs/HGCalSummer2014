#ifndef _HGCAnalyzer_h_
#define _HGCAnalyzer_h_

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESTransientHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "DataFormats/Candidate/interface/Candidate.h"

#include "UserCode/HGCalSummer2014/interface/HGCEventSummary.h"

#include <string>

/**
   @class HGCAnalyzer
   @short a simple analyzer
   @author P. Silva (CERN)
*/

class HGCAnalyzer : public edm::EDAnalyzer 
{
  
 public:
  
  explicit HGCAnalyzer( const edm::ParameterSet& );
  ~HGCAnalyzer();
  virtual void analyze( const edm::Event&, const edm::EventSetup& );

 private:

  std::string genSource_,trackSource_;
  std::vector<std::string> hitCollections_;
  TTree *t_;
  HGCEvent::HGCEvent_t hgcEvt_;
};
 

#endif
