#include "UserCode/HGCalSummer2014/plugins/HGCAnalyzer.h"

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "TrackingTools/IPTools/interface/IPTools.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/HGCRecHit/interface/HGCRecHitCollections.h"

#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"

#include "DataFormats/TrajectorySeed/interface/PropagationDirection.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateTransform.h"

#include <iostream>

using namespace std;

//
HGCAnalyzer::HGCAnalyzer( const edm::ParameterSet &iConfig ) : 
  t_(0), 
  eTkPropagator_(0),
  piTkPropagator_(0)
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
void HGCAnalyzer::initTrackPropagators(edm::ESHandle<MagneticField> &bField,std::map<int,const HGCalGeometry *> &hgcGeometries)
{
  eTkPropagator_  = new PropagatorWithMaterial(alongMomentum,0.000511,bField.product());
  piTkPropagator_ = new PropagatorWithMaterial(alongMomentum,0.1396,  bField.product());
  
  Surface::RotationType rot; //unit rotation matrix
  for(std::map<int,const HGCalGeometry *>::iterator it = hgcGeometries.begin();
      it!= hgcGeometries.end();
      it++)
    {

      const HGCalDDDConstants &dddCons=it->second->topology().dddConstants();
      float Z(dddCons.getFirstTrForm()->h3v.z());
      float Radius(dddCons.getFirstModule(true)->tl+dddCons.getFirstTrForm()->h3v.perp());
      std::cout << "HGC subdet: " << it->first << " z=" << Z << " radius=" << Radius << std::endl;

      minusSurface_[it->first] = ReferenceCountingPointer<BoundDisk> ( new BoundDisk( Surface::PositionType(0,0,-Z),  rot, new SimpleDiskBounds( 0, Radius,  -0.001, 0.001)));
      plusSurface_[it->first]  = ReferenceCountingPointer<BoundDisk> ( new BoundDisk( Surface::PositionType(0,0,+Z),  rot, new SimpleDiskBounds( 0, Radius,  -0.001, 0.001)));
    }
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
  
  //PF clusters
  //hgcEvt_.nclus=0;
  
  std::map<int,const HGCalGeometry *> hgcGeometries;
  for(size_t i=0; i<geometrySource_.size(); i++)
    {
      edm::ESHandle<HGCalGeometry> hgcGeo;
      iSetup.get<IdealGeometryRecord>().get(geometrySource_[i],hgcGeo);
      hgcGeometries[i]=hgcGeo.product();
    }
  
  //hits
  hgcEvt_.nhits=0;
  for(size_t i=0; i<hitCollections_.size(); i++)
    {
      edm::Handle<HGCRecHitCollection> pfhits;
      iEvent.getByLabel(edm::InputTag(hitCollections_[i]), pfhits);
      const HGCalGeometry& hgcGeo = *(hgcGeometries[i]);

      for(HGCRecHitCollection::const_iterator hitIt=pfhits->begin(); hitIt!=pfhits->end(); hitIt++)
	{
	  double en=hitIt->energy()*1e6; //energy in keV
	  if(en<hitThresholds_[i]) continue;
	  
	  hgcEvt_.hit_type[hgcEvt_.nhits]=i;
	  unsigned int detId=hitIt->id();
	  int layer= ((detId>>19)&0x1f);
	  hgcEvt_.hit_layer[hgcEvt_.nhits]=layer;

	  const GlobalPoint pos( std::move( hgcGeo.getPosition( detId ) ) );
	  hgcEvt_.hit_x[hgcEvt_.nhits]=pos.x();
	  hgcEvt_.hit_y[hgcEvt_.nhits]=pos.y();
	  hgcEvt_.hit_z[hgcEvt_.nhits]=pos.z();
	  hgcEvt_.hit_edep[hgcEvt_.nhits]=en;
	  hgcEvt_.nhits++;
	  
	  if(hgcEvt_.nhits>=MAXHITS) break;
	}
    }
  
  
  //tracks
  hgcEvt_.ntk=0;
  edm::ESHandle<MagneticField> bField;
  iSetup.get<IdealMagneticFieldRecord>().get(bField);
  if(eTkPropagator_==0) initTrackPropagators(bField,hgcGeometries);
  edm::Handle<reco::TrackCollection> tracks;
  iEvent.getByLabel(edm::InputTag(trackSource_), tracks);
  edm::ESHandle<TrackerGeometry> tkGeom;
  iSetup.get<TrackerDigiGeometryRecord>().get( tkGeom );
  for(std::vector<reco::Track>::const_iterator tIt = tracks->begin(); tIt != tracks->end(); tIt++)
    {
      //it should be safe to cut away these tracks
      if(fabs(tIt->eta())<1.45 || fabs(tIt->eta())>3.5) continue;
      
      hgcEvt_.tk_pt[hgcEvt_.ntk]    = tIt->pt();
      hgcEvt_.tk_eta[hgcEvt_.ntk]   = tIt->eta();
      hgcEvt_.tk_phi[hgcEvt_.ntk]   = tIt->phi();
      hgcEvt_.tk_chi2[hgcEvt_.ntk]  = tIt->normalizedChi2();
      hgcEvt_.tk_nhits[hgcEvt_.ntk] = tIt->hitPattern().trackerLayersWithMeasurement();

      //track propagation to HGC surfaces based on 
      //https://cmssdt.cern.ch/SDT/lxr/source/RecoEgamma/EgammaPhotonProducers/src/ConversionProducer.cc?v=CMSSW_6_2_0_SLHC10
      const TrajectoryStateOnSurface myTSOS = trajectoryStateTransform::outerStateOnSurface(*tIt, *(tkGeom.product()), bField.product());
      std::map<int, ReferenceCountingPointer<BoundDisk> >::iterator itBegin( myTSOS.globalPosition().z()>0 ? plusSurface_.begin() : minusSurface_.begin() );
      std::map<int, ReferenceCountingPointer<BoundDisk> >::iterator itEnd  ( myTSOS.globalPosition().z()>0 ? plusSurface_.end()   : minusSurface_.end() );
      for(std::map<int, ReferenceCountingPointer<BoundDisk> >::iterator it = itBegin; it!=itEnd; it++)
	{
	  //TrajectoryStateOnSurface eStateAtSurface   = eTkPropagator_->propagate  (myTSOS, *(it->second) );
	  TrajectoryStateOnSurface piStateAtSurface  = piTkPropagator_->propagate  (myTSOS, *(it->second) );
	  if(piStateAtSurface.isValid())
	    {
	      GlobalPoint pt=piStateAtSurface.globalPosition();
	      if(it->first==0)   { hgcEvt_.tk_eehit_x[hgcEvt_.ntk]=pt.x();  hgcEvt_.tk_eehit_y[hgcEvt_.ntk]=pt.y();  }
	      if(it->first==1)   { hgcEvt_.tk_hefhit_x[hgcEvt_.ntk]=pt.x(); hgcEvt_.tk_hefhit_y[hgcEvt_.ntk]=pt.y(); }
	      if(it->first==2)   { hgcEvt_.tk_hebhit_x[hgcEvt_.ntk]=pt.x(); hgcEvt_.tk_hebhit_y[hgcEvt_.ntk]=pt.y(); }
	    }
	}
      hgcEvt_.ntk++;

      if(hgcEvt_.ntk>=MAXGENPART) break;
    }

  
  //fill tree
  if(hgcEvt_.nhits>0)  t_->Fill();

  cout << hgcEvt_.nhits << " HGC hits for " << hgcEvt_.ngen << " generated particles" << endl;

}



//define this as a plug-in
DEFINE_FWK_MODULE(HGCAnalyzer);
