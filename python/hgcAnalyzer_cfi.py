import FWCore.ParameterSet.Config as cms

hgcAnalyzer = cms.EDAnalyzer("HGCAnalyzer",
                             genSource=cms.string('genParticles'),
                             trackSource=cms.string('generalTracks'),
                             hitCollections=cms.vstring('particleFlowRecHitHGCEE','particleFlowRecHitHGCHEF','particleFlowRecHitHGCHEB')
                             )
