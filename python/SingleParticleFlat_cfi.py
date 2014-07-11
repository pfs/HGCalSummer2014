import FWCore.ParameterSet.Config as cms

generator = cms.EDProducer("FlatRandomPtGunProducer",
			    PGunParameters = cms.PSet( MaxPt = cms.double(50.),
                               			       MinPt = cms.double(1.),
			                               PartID = cms.vint32(111),
                        			       MaxEta = cms.double(1.5),
                               			       MinEta = cms.double(3.0),
                               			       MaxPhi = cms.double(3.14159265359),
                               			       MinPhi = cms.double(-3.14159265359)
                               			      ),
                           Verbosity = cms.untracked.int32(0),
                           psethack = cms.string('single paticle flat 1 to 50'),
                           AddAntiParticle = cms.bool(False),
                           firstRun = cms.untracked.uint32(1)
                           )
