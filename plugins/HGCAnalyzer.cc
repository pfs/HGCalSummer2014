#include "UserCode/HGCalSummer2014/plugins/HGCAnalyzer.h"

#include "SimDataFormats/Track/interface/SimTrack.h"
#include "SimDataFormats/Track/interface/SimTrackContainer.h"
#include "SimDataFormats/Vertex/interface/SimVertex.h"
#include "SimDataFormats/Vertex/interface/SimVertexContainer.h"

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
  piTkPropagator_(0)
{
  std::cout << "[HGCAnalyzer::CTOR]" << std::endl; 

  g4Tracks_        = iConfig.getParameter< std::string >("g4Tracks");
  g4Vertices_      = iConfig.getParameter< std::string >("g4Vertices");
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
  std::cout << "[HGCAnalyzer::initTrackPropagators]" << std::endl;
  piTkPropagator_ = new PropagatorWithMaterial(alongMomentum,0.1396,  bField.product());
  
  Surface::RotationType rot; //unit rotation matrix
  for(std::map<int,const HGCalGeometry *>::iterator it = hgcGeometries.begin();
      it!= hgcGeometries.end();
      it++)
    {
      std::cout << "HGC subdet: " << it->first << std::endl;
      const HGCalDDDConstants &dddCons=it->second->topology().dddConstants();

      std::vector< ReferenceCountingPointer<BoundDisk> > iMinusSurfaces, iPlusSurfaces;
      std::vector<HGCalDDDConstants::hgtrform>::const_iterator firstLayerIt = dddCons.getFirstTrForm();
      std::vector<HGCalDDDConstants::hgtrform>::const_iterator lastLayerIt  = dddCons.getLastTrForm();
      for(std::vector<HGCalDDDConstants::hgtrform>::const_iterator layerIt=firstLayerIt; layerIt!=lastLayerIt; layerIt++)
	{
	  float Z(layerIt->h3v.z());
	  float Radius(dddCons.getLastModule(true)->tl+layerIt->h3v.perp());

	  std::cout << " z=" << Z << std::flush;

	  iMinusSurfaces.push_back(ReferenceCountingPointer<BoundDisk> ( new BoundDisk( Surface::PositionType(0,0,-Z),  rot, new SimpleDiskBounds( 0, Radius,  -0.001, 0.001))));
	  iPlusSurfaces.push_back(ReferenceCountingPointer<BoundDisk> ( new BoundDisk( Surface::PositionType(0,0,+Z),  rot, new SimpleDiskBounds( 0, Radius,  -0.001, 0.001))));
	}
      std::cout << " | total " << minusSurface_.size() << " layer" << std::endl;

      minusSurface_[it->first] = iMinusSurfaces;
      plusSurface_[it->first]  = iPlusSurfaces;
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
  //cf. https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideDataFormatSimG4Core
  edm::Handle<edm::SimTrackContainer> SimTk;
  iEvent.getByLabel(g4Tracks_,SimTk);
  edm::Handle<edm::SimVertexContainer> SimVtx;
  iEvent.getByLabel(g4Vertices_,SimVtx);
  hgcEvt_.ngen=0;

  //the primary
  const SimTrack &tk=SimTk->at(0);
  const math::XYZTLorentzVectorD &p4=tk.momentum() ;
  hgcEvt_.gen_id[hgcEvt_.ngen]  = tk.type();
  hgcEvt_.gen_vtx[hgcEvt_.ngen] = 0;
  hgcEvt_.gen_pt[hgcEvt_.ngen]  = p4.pt();
  hgcEvt_.gen_eta[hgcEvt_.ngen] = p4.eta();
  hgcEvt_.gen_phi[hgcEvt_.ngen] = p4.phi();
  hgcEvt_.gen_en[hgcEvt_.ngen] = p4.energy();
  hgcEvt_.ngen++;

  //consider only vertices with more than two particles
  std::map<int,int> vtxMult;
  for (unsigned int isimtk = 0; isimtk < SimTk->size() ; isimtk++ ) 
    {
      const SimTrack &tk=SimTk->at(isimtk);
      int vtxIdx=tk.vertIndex();
      if(vtxIdx<=0) continue;
      if(vtxMult.find(vtxIdx)==vtxMult.end()) vtxMult[vtxIdx]=0;
      vtxMult[vtxIdx]++;
    }
  for(std::map<int,int>::iterator vtxIt=vtxMult.begin(); vtxIt!=vtxMult.end(); vtxIt++)
    {
      if(vtxIt->second<2) continue;
      for (unsigned int isimtk = 0; isimtk < SimTk->size() ; isimtk++ )
	{
	  const SimTrack &tk=SimTk->at(isimtk);
	  int vtxIdx=tk.vertIndex();
	  if(vtxIdx!=vtxIt->first) continue;
	  const math::XYZTLorentzVectorD &p4 = tk.momentum() ;
	  hgcEvt_.gen_id[hgcEvt_.ngen]       = tk.type();
	  hgcEvt_.gen_vtx[hgcEvt_.ngen]      = vtxIdx;
	  hgcEvt_.gen_pt[hgcEvt_.ngen]       = p4.pt();
	  hgcEvt_.gen_eta[hgcEvt_.ngen]      = p4.eta();
	  hgcEvt_.gen_phi[hgcEvt_.ngen]      = p4.phi();
	  hgcEvt_.gen_en[hgcEvt_.ngen]       = p4.energy();
	  hgcEvt_.ngen++;
	  //check if limits are respected
	  if(hgcEvt_.ngen>=MAXGENPART) break;
	}
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
  if(piTkPropagator_==0) initTrackPropagators(bField,hgcGeometries);
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
      std::map<int, std::vector<ReferenceCountingPointer<BoundDisk> > >::iterator itBegin( myTSOS.globalPosition().z()>0 ? plusSurface_.begin() : minusSurface_.begin() );
      std::map<int, std::vector<ReferenceCountingPointer<BoundDisk> > >::iterator itEnd  ( myTSOS.globalPosition().z()>0 ? plusSurface_.end()   : minusSurface_.end() );
      for(std::map<int, std::vector<ReferenceCountingPointer<BoundDisk> > >::iterator it = itBegin; it!=itEnd; it++)
	{
	  for(size_t ilayer=0; ilayer<it->second.size(); ilayer++)
	    {
	      TrajectoryStateOnSurface piStateAtSurface  = piTkPropagator_->propagate  (myTSOS, *((it->second)[ilayer]) );
	      if(piStateAtSurface.isValid())
		{
		  GlobalPoint pt=piStateAtSurface.globalPosition();
		  if(it->first==0)   { hgcEvt_.tk_eehit_x[hgcEvt_.ntk][ilayer]=pt.x();  hgcEvt_.tk_eehit_y[hgcEvt_.ntk][ilayer]=pt.y();  }
		  if(it->first==1)   { hgcEvt_.tk_hefhit_x[hgcEvt_.ntk][ilayer]=pt.x(); hgcEvt_.tk_hefhit_y[hgcEvt_.ntk][ilayer]=pt.y(); }
		  if(it->first==2)   { hgcEvt_.tk_hebhit_x[hgcEvt_.ntk][ilayer]=pt.x(); hgcEvt_.tk_hebhit_y[hgcEvt_.ntk][ilayer]=pt.y(); }
		}
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
