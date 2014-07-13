import FWCore.ParameterSet.Config as cms

hgcAnalyzer = cms.EDAnalyzer("HGCAnalyzer",
                             g4Tracks       = cms.string('g4SimHits'),
                             g4Vertices     = cms.string('g4SimHits'),
                             trackSource    = cms.string('generalTracks'),
                             geometrySource = cms.vstring('HGCalEESensitive','HGCalHESiliconSensitive','HGCalHEScintillatorSensitive'),
                             hitCollections = cms.vstring('HGCalRecHit:HGCEERecHits','HGCalRecHit:HGCHEFRecHits','HGCalRecHit:HGCHEBRecHits'),
                             hitThresholds  = cms.vdouble(55.1*3/4, 85.0*3/4, 1498.4*3/4)
                             )
