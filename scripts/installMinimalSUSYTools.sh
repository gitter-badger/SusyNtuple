#!/bin/bash

#
# This script will checkout and compile the minimal content from SUSYTools
# needed for reading the SusyNt. The functionality needed:
#   - BTagCalib
#   - SUSYCrossSection
# 
# This script is currently for testing, so things will probably get moved around later
#

# Run this from your work dir, not within this package
asetup 17.2.9.1,here,slc5

#SVNOFF="svn+ssh://svn.cern.ch/reps/atlasoff/"
susyURL="$SVNOFF/PhysicsAnalysis/SUSYPhys/SUSYTools/tags/SUSYTools-00-03-14"
rootCoreURL="$SVNOFF/PhysicsAnalysis/D3PDTools/RootCore/tags/RootCore-00-02-99"
bTagURL="$SVNOFF/PhysicsAnalysis/JetTagging/JetTagPerformanceCalibration/CalibrationDataInterface/tags/CalibrationDataInterface-00-03-06"
mt2URL="$SVNPHYS/Physics/SUSY/Analyses/WeakProduction/Mt2/tags/Mt2-00-00-01"
trigURL="$SVNPHYS/Physics/SUSY/Analyses/WeakProduction/DGTriggerReweight/tags/DGTriggerReweight-00-00-29"
reweightUtilsURL="$SVNOFF/PhysicsAnalysis/AnalysisCommon/ReweightUtils/tags/ReweightUtils-00-02-13"

# Install fresh RootCore
svn co $rootCoreURL RootCore

# Checkout CalibrationDataInterface
svn co $bTagURL/cmt $bTagURL/CalibrationDataInterface $bTagURL/Root $bTagURL/src CalibrationDataInterface

# Additional checkouts for 2-lepton
svn co $mt2URL Mt2
svn co $trigURL DGTriggerReweight
svn co $reweightUtilsURL ReweightUtils

# Checkout minimal SUSYTools
mkdir -p SUSYTools/SUSYTools SUSYTools/Root SUSYTools/cmt SUSYTools/data
svn export $susyURL/SUSYTools/SUSYCrossSection.h SUSYTools/SUSYTools/SUSYCrossSection.h
svn export $susyURL/SUSYTools/BTagCalib.h SUSYTools/SUSYTools/BTagCalib.h
svn export $susyURL/Root/SUSYCrossSection.cxx SUSYTools/Root/SUSYCrossSection.cxx
svn export $susyURL/Root/BTagCalib.cxx SUSYTools/Root/BTagCalib.cxx
svn export $susyURL/cmt/Makefile.RootCore SUSYTools/cmt/Makefile.RootCore
svn co $susyURL/data SUSYTools/data

# Modify the SUSYTools Makefile dependencies
sed -i "s/^PACKAGE_DEP.*/PACKAGE_DEP = CalibrationDataInterface/" SUSYTools/cmt/Makefile.RootCore

# Configure RootCore
cd RootCore
./configure
source scripts/setup.sh
cd ..

# Compile everything
$ROOTCOREDIR/scripts/build.sh
