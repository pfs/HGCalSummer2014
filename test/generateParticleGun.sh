#!/bin/bash

#configure
PIDTOGENERATE=$1
NEVENTS=$2
BASEDIR=${CMSSW_BASE}/src/
TEMPLATEFILE=UserCode/HGCalSummer2014/python/SingleParticleFlat_cfi.py.templ
CFIFILE=${TEMPLATEFILE/".templ"/""}
if [ -z "$PIDTOGENERATE" ]; then
    echo "No PID to generate received as input... quitting";
    exit -1;
fi
if [ -z "$NEVENTS" ]; then
    NEVENTS=10;
fi

#generate the cfg
echo "Generating ${NEVENTS} events for pdgId=${PIDTOGENERATE} from the following configuration file ${TEMPLATEFILE}"
cat ${BASEDIR}/${TEMPLATEFILE} | sed 's/@PID/'${PIDTOGENERATE}'/' > ${BASEDIR}/${CFIFILE}
echo "cfi snippet can be found in ${CFIFILE}"

#run the simulation
scram b
cmsDriver.py ${CFIFILE} -n ${NEVENTS} --conditions auto:upgradePLS3 --eventcontent FEVTDEBUG --relval 10000,100 -s GEN,SIM --datatier GEN-SIM --beamspot Gauss --customise SLHCUpgradeSimulations/Configuration/combinedCustoms.cust_2023HGCalMuon --geometry Extended2023HGCalMuon,Extended2023HGCalMuonReco --magField 38T_PostLS1 --no_exec --fileout file:step1.root 

echo "process.g4SimHits.StackingAction.SaveFirstLevelSecondary = True" >> SingleParticleFlat_cfi_py_GEN_SIM.py
cmsRun SingleParticleFlat_cfi_py_GEN_SIM.py

cmsDriver.py step2 --conditions auto:upgradePLS3 -n -1 --eventcontent FEVTDEBUGHLT -s DIGI:pdigi_valid,L1,DIGI2RAW --datatier GEN-SIM-DIGI-RAW --customise SLHCUpgradeSimulations/Configuration/combinedCustoms.cust_2023HGCalMuon --geometry Extended2023HGCalMuon,Extended2023HGCalMuonReco --magField 38T_PostLS1 --filein file:step1.root --fileout file:step2.root
cmsDriver.py step3 --conditions auto:upgradePLS3 -n -1 --eventcontent FEVTDEBUGHLT,DQM -s RAW2DIGI,L1Reco,RECO,VALIDATION,DQM --datatier GEN-SIM-RECO,DQM --customise SLHCUpgradeSimulations/Configuration/combinedCustoms.cust_2023HGCalMuon --geometry Extended2023HGCalMuon,Extended2023HGCalMuonReco --magField 38T_PostLS1 --filein file:step2.root --fileout file:ParticleGun_${PIDTOGENERATE}_RECO.root     

#remove unnecessary files
rm step*.root
rm step*.py
rm SingleParticleFlat_cfi_py_GEN_SIM.py
rm ParticleGun_211_RECO_inDQM.root

echo "You can find ${NEVENTS} events for pdgId=${PIDTOGENERATE} reconstructed in ParticleGun_${PIDTOGENERATE}_RECO.root"