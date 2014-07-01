#include "UserCode/HGCalSummer2014/plugins/HGCAnalyzer.h"

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "TrackingTools/IPTools/interface/IPTools.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include <iostream>

using namespace std;

//
HGCAnalyzer::HGCAnalyzer( const edm::ParameterSet &iConfig ) : t_(0)
{
  genSource_       = iConfig.getParameter< std::string >("genSource");
  trackSource_     = iConfig.getParameter< std::string >("trackSource");
  hitCollections_  = iConfig.getParameter< std::vector<std::string> >("hitCollections");

  edm::Service<TFileService> fs;
  t_=fs->make<TTree>("HGC","Event Summary");
  HGCEvent::createTree(t_,hgcEvt_);
}

//
HGCAnalyzer::~HGCAnalyzer()
{
}

//
void HGCAnalyzer::analyze( const edm::Event &iEvent, const edm::EventSetup &iSetup)
{
  
  //event header
  hgcEvt_.run    = iEvent.id().run();
  hgcEvt_.lumi   = iEvent.luminosityBlock();
  hgcEvt_.event  = t_->GetEntriesFast()+1; //iEvent.id().event();

  //get gen level information
  edm::Handle<edm::View<reco::Candidate> > genParticles;
  iEvent.getByLabel(edm::InputTag(genSource_), genParticles);
  hgcEvt_.ngen=0;
  for(size_t i = 0; i < genParticles->size(); ++ i)
    {
      const reco::GenParticle & p = dynamic_cast<const reco::GenParticle &>( (*genParticles)[i] );
      if(p.status()!=1) continue;
      hgcEvt_.gen_id[hgcEvt_.ngen]=p.pdgId();
      hgcEvt_.gen_pt[hgcEvt_.ngen]=p.pt();
      hgcEvt_.gen_eta[hgcEvt_.ngen]=p.eta();
      hgcEvt_.gen_phi[hgcEvt_.ngen]=p.phi();
      hgcEvt_.gen_en[hgcEvt_.ngen]=p.energy();
      hgcEvt_.ngen++;
  
      //check if limits are respected
      if(hgcEvt_.ngen>=MAXGENPART) break;
    }
  
  //tracks
  hgcEvt_.ntk=0;
  edm::Handle<reco::TrackCollection> tracks;
  iEvent.getByLabel(edm::InputTag(trackSource_), tracks);
  for(std::vector<reco::Track>::const_iterator tIt = tracks->begin(); tIt != tracks->end(); tIt++)
    {
      hgcEvt_.tk_pt[hgcEvt_.ntk]    = tIt->pt();
      hgcEvt_.tk_eta[hgcEvt_.ntk]   = tIt->eta();
      hgcEvt_.tk_phi[hgcEvt_.ntk]   = tIt->phi();
      hgcEvt_.tk_chi2[hgcEvt_.ntk]  = tIt->normalizedChi2();
      hgcEvt_.tk_nhits[hgcEvt_.ntk] = tIt->hitPattern().trackerLayersWithMeasurement();
      hgcEvt_.ntk++;
      if(hgcEvt_.ntk>=MAXGENPART) break;

    }

  //hits
  hgcEvt_.nhits=0;
  for(size_t i=0; i<hitCollections_.size(); i++)
    {
      
    }
 
  //fill tree
  if(hgcEvt_.nhits>0)  t_->Fill();
}



//define this as a plug-in
DEFINE_FWK_MODULE(HGCAnalyzer);
