import FWCore.ParameterSet.Config as cms

import math

minEn=5
maxEn=100
etaToShoot=2.0

generator = cms.EDProducer("FlatRandomPtGunProducer",
			    PGunParameters = cms.PSet( MaxPt = cms.double(maxEn/math.cosh(etaToShoot)),
                               			       MinPt = cms.double(minEn/math.cosh(etaToShoot)),
			                               PartID = cms.vint32(111),
                        			       MaxEta = cms.double(etaToShoot*0.99),
                               			       MinEta = cms.double(etaToShoot*1.01),
                               			       MaxPhi = cms.double(3.14159265359),
                               			       MinPhi = cms.double(-3.14159265359)
                               			      ),
                           Verbosity = cms.untracked.int32(0),
                           psethack = cms.string('single paticle flat 1 to 50'),
                           AddAntiParticle = cms.bool(False),
                           firstRun = cms.untracked.uint32(1)
                           )
