#include "UserCode/HGCalSummer2014/plugins/HGCAnalyzer.h"

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "TrackingTools/IPTools/interface/IPTools.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "Geometry/FCalGeometry/interface/HGCalGeometry.h"
#include "DataFormats/HGCRecHit/interface/HGCRecHitCollections.h"


#include <iostream>

using namespace std;

//
HGCAnalyzer::HGCAnalyzer( const edm::ParameterSet &iConfig ) : t_(0)
{
  genSource_       = iConfig.getParameter< std::string >("genSource");
  trackSource_     = iConfig.getParameter< std::string >("trackSource");
  geometrySource_  = iConfig.getParameter< std::vector<std::string> >("geometrySource");
  hitCollections_  = iConfig.getParameter< std::vector<std::string> >("hitCollections");
  hitThresholds_   = iConfig.getParameter< std::vector<double> >("hitThresholds");

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
      edm::Handle<HGCRecHitCollection> pfhits;
      iEvent.getByLabel(edm::InputTag(hitCollections_[i]), pfhits);

      edm::ESHandle<HGCalGeometry> geoHandle;
      iSetup.get<IdealGeometryRecord>().get(geometrySource_[i],geoHandle);
      const HGCalGeometry& hgcGeo = *geoHandle;

      for(HGCRecHitCollection::const_iterator hitIt=pfhits->begin(); hitIt!=pfhits->end(); hitIt++)
	{
	  double en=hitIt->energy()*1e6; //energy in keV
	  if(en<hitThresholds_[i]) continue;
	  
	  hgcEvt_.hit_type[hgcEvt_.nhits]=i;
	  unsigned int detId=hitIt->id();
	  int layer= ((detId>>19)&0x1f);
	  hgcEvt_.hit_layer[hgcEvt_.nhits]=layer;

	  //const FlatTrd *thisCell = static_cast<const FlatTrd*>(hgcGeo.getGeometry(detId));
	  const GlobalPoint pos( std::move( hgcGeo.getPosition( detId ) ) );

	  hgcEvt_.hit_x[hgcEvt_.nhits]=pos.x();
	  hgcEvt_.hit_y[hgcEvt_.nhits]=pos.y();
	  hgcEvt_.hit_z[hgcEvt_.nhits]=pos.z();
	  hgcEvt_.hit_edep[hgcEvt_.nhits]=en;
	  hgcEvt_.nhits++;

	  if(hgcEvt_.nhits>=MAXHITS) break;
	}
    }
  
  //fill tree
  if(hgcEvt_.nhits>0)  t_->Fill();

  cout << hgcEvt_.nhits << " HGC hits for " << hgcEvt_.ngen << " generated particles" << endl;

}



//define this as a plug-in
DEFINE_FWK_MODULE(HGCAnalyzer);
