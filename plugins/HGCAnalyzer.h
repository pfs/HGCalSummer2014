#ifndef _HGCAnalyzer_h_
#define _HGCAnalyzer_h_

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESTransientHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "DataFormats/Candidate/interface/Candidate.h"
#include "TrackingTools/MaterialEffects/interface/PropagatorWithMaterial.h"
#include "DataFormats/GeometrySurface/interface/BoundDisk.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "MagneticField/Engine/interface/MagneticField.h"

#include "Geometry/FCalGeometry/interface/HGCalGeometry.h"

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

  void initTrackPropagators(edm::ESHandle<MagneticField> &bField,std::map<int,const HGCalGeometry *> &hgcGeometries);

  std::string genSource_,trackSource_;
  std::vector<std::string> geometrySource_,hitCollections_;
  std::vector<double> hitThresholds_;

  TTree *t_;
  HGCEvent::HGCEvent_t hgcEvt_;

  PropagatorWithMaterial *piTkPropagator_;
  std::map<int, std::vector<ReferenceCountingPointer<BoundDisk> > > plusSurface_, minusSurface_;
};
 

#endif
