#!/bin/bash

#configure
infile=$1
if [ -z "$infile" ]; then
    echo "No file was given to visualize...quitting";
    exit -1;
fi
fwc=${CMSSW_BASE}/src/RecoParticleFlow/PFClusterProducer/test/hgcal_rechits.fwc
hgcalgeom=/afs/cern.ch/user/l/lgray/work/public/xHGCAL/cmsRecoGeom1-HGCAL.root
simgeom=/afs/cern.ch/user/l/lgray/work/public/xHGCAL/cmsSimGeom-14-HGCAL.root

#run fireworks
cmsShow -c ${fwc} -g ${hgcalgeom} --sim-geom-file ${simgeom} ${infile}
