import FWCore.ParameterSet.Config as cms

process = cms.Process("HGCSimHitsAnalysis")

process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')    
process.load('FWCore.MessageService.MessageLogger_cfi') 
process.load('Configuration.Geometry.GeometryExtended2023HGCalMuonReco_cff')
process.load('Configuration.Geometry.GeometryExtended2023HGCalMuon_cff')

#configure from command line
import sys
try:
    inFile=sys.argv[2]
except:
    print 'Input file is missing... quitting'
    sys.exit(0)

process.TFileService = cms.Service("TFileService", fileName = cms.string('HGCHits.root'))
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) ) 
process.source = cms.Source("PoolSource",fileNames=cms.untracked.vstring(inFile))
process.source.duplicateCheckMode = cms.untracked.string('noDuplicateCheck')
process.load('UserCode.HGCalSummer2014.hgcAnalyzer_cfi')

process.p = cms.Path(process.hgcAnalyzer)

