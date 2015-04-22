#include "SusyNtuple/ElectronSelector.h"
#include "SusyNtuple/MuonSelector.h"
#include "SusyNtuple/SusyNt.h"

#include <algorithm>
#include <cassert>
#include <iostream>

using Susy::ElectronSelector;
using Susy::Electron;
using Susy::NtSys::SusyNtSys;
using std::cout;
using std::endl;

namespace Susy {

// ---------------------------------------------- 
void ElectronSelector::buildRequirements(const AnalysisType &a)
{
    switch(a) {
    //////////////////////////////////////
    // 2L-ANALYSIS
    //////////////////////////////////////
    case(Ana_2Lep) : { 
        m_2lep = true;

        m_removeLepsFromIso = false;
        m_doIPCut = true;
        m_doPtconeCut = true;
        m_doElEtconeCut = true;
        m_doMuEtconeCut = false;
    
        // electrons
        EL_MIN_PT                       = 10.0;
        EL_MAX_ETA                      = 2.47;
        EL_ISO_PT_THRS                  = 60.0;
        EL_PTCONE30_PT_CUT              = 0.16;
        EL_TOPOCONE30_SLOPE_DATA_CUT    = 0.02015;
        EL_TOPOCONE30_SLOPE_MC_CUT      = 0.01794;
        EL_TOPOCONE30_PT_CUT            = 0.18;
        EL_MAX_D0SIG_CUT                = 5.0; 

        break;
    } 
    //////////////////////////////////////
    // 3L-ANALYSIS
    //////////////////////////////////////
    case(Ana_3Lep) : {
        m_3lep = true;

        m_removeLepsFromIso = false;
        m_doIPCut = true;
        m_doPtconeCut = true;
        m_doElEtconeCut = true;
        m_doMuEtconeCut = false;

        // electrons
        EL_MIN_PT                       = 10.0;
        EL_MAX_ETA                      = 2.47;
        EL_ISO_PT_THRS                  = 60.0;
        EL_PTCONE30_PT_CUT              = 0.16;
        EL_TOPOCONE30_SLOPE_DATA_CUT    = 0.02015;
        EL_TOPOCONE30_SLOPE_MC_CUT      = 0.01794;
        EL_TOPOCONE30_PT_CUT            = 0.18;
        EL_MAX_D0SIG_CUT                = 5.0; 

        break;
    }
    //////////////////////////////////////
    // WH-ANALYSIS
    //////////////////////////////////////
    case(Ana_2LepWH) : {
        m_2lepWH = true;

        m_removeLepsFromIso = false;
        m_doIPCut = true;
        m_doPtconeCut = true;
        m_doElEtconeCut = true;
        m_doMuEtconeCut = false;

        // electrons
        EL_MIN_PT                       = 10.0;
        EL_MAX_ETA                      = 2.47;
        EL_ISO_PT_THRS                  = 60.0;
        EL_PTCONE30_PT_CUT              = 0.07;
        EL_TOPOCONE30_SLOPE_DATA_CUT    = 0.02015;
        EL_TOPOCONE30_SLOPE_MC_CUT      = 0.01794;
        EL_TOPOCONE30_PT_CUT            = 0.13;
        EL_MAX_D0SIG_CUT                = 3.0; 


        break;
    }
    //////////////////////////////////////
    // Gone fishin'
    //////////////////////////////////////
    case(Ana_N) : {
        cout << "ElectronSelector::buildRequirements() error: invalid analysis '" << a << "'" << endl;
        cout << "               will apply default electron selection (Ana_2Lep)." << endl; 
        m_analysis = Ana_2Lep;
        m_2lep = true;

        m_removeLepsFromIso = false;
        m_doIPCut = true;
        m_doPtconeCut = true;
        m_doElEtconeCut = true;
        m_doMuEtconeCut = false;
    
        // electrons
        EL_MIN_PT                       = 10.0;
        EL_MAX_ETA                      = 2.47;
        EL_ISO_PT_THRS                  = 60.0;
        EL_PTCONE30_PT_CUT              = 0.16;
        EL_TOPOCONE30_SLOPE_DATA_CUT    = 0.02015;
        EL_TOPOCONE30_SLOPE_MC_CUT      = 0.01794;
        EL_TOPOCONE30_PT_CUT            = 0.18;
        EL_MAX_D0SIG_CUT                = 5.0; 

        break;
    }

    } // end switch
}
// ------------------------------------------------------------------------------ //
// Constructor
// ------------------------------------------------------------------------------ //
ElectronSelector::ElectronSelector(): 
    m_systematic(NtSys::NOM),
    m_analysis(Ana_N),
    m_doIPCut(true),
    m_doPtconeCut(true),
    m_doElEtconeCut(true),
    m_doMuEtconeCut(false),
    m_2lep(false),
    m_3lep(false),
    m_2lepWH(false),
    m_verbose(false)
{
}
// ----------------------------------------------
ElectronSelector& ElectronSelector::setSystematic(const NtSys::SusyNtSys &s)
{
    m_systematic = s;
    return *this;
}
// ----------------------------------------------
ElectronSelector& ElectronSelector::setAnalysis(const AnalysisType &a)
{
    m_analysis = a;
    buildRequirements(a);
    return *this;
}
// ----------------------------------------------

// ------------------------------------------------------------------------------ //
//  ElectronSelector Methods
// ------------------------------------------------------------------------------ //
bool ElectronSelector::isSignalElectron(const Electron* ele,
                                        const ElectronVector& baseElectrons,
                                        const MuonVector& baseMuons,
                                        const unsigned int nVtx, bool isMC,
                                        bool removeLepsFromIso)
{
    /////////////////////////////
    // Electron ID
    /////////////////////////////
    if(m_2lep || m_3lep || m_2lepWH) {
        if(!ele->tightPP) return false;
    }
     
    /////////////////////////////
    // Impact parameter
    /////////////////////////////
    if (m_doIPCut) {
        if(fabs(ele->d0Sig()) >= EL_MAX_D0SIG_CUT) return false;
        if(fabs(ele->z0SinTheta()) >= EL_MAX_Z0_SINTHETA) return false;
    }
    /////////////////////////////
    // ptcone isolation
    /////////////////////////////
    float pt = ele->Pt();
    if(m_doPtconeCut) {
        float ptcone30 = elPtConeCorr(ele, baseElectrons, baseMuons, nVtx, isMC, removeLepsFromIso); 
        if(m_2lepWH){
            if(ptcone30/std::min(pt, EL_ISO_PT_THRS) >= EL_PTCONE30_PT_CUT) return false;
        }
        else if(m_2lep || m_3lep) 
            if (ptcone30/pt >= EL_PTCONE30_PT_CUT) return false;
    }
    /////////////////////////////
    // topo etcone isolation
    /////////////////////////////
    if(m_doElEtconeCut) { 
        float etcone = elEtTopoConeCorr(ele, baseElectrons, baseMuons, nVtx, isMC, removeLepsFromIso);
        if(m_2lepWH){
            if(etcone/std::min(pt,EL_ISO_PT_THRS) >= EL_TOPOCONE30_PT_CUT) return false;
        }
        else if(m_2lep || m_3lep)
            if(etcone/pt >= EL_TOPOCONE30_PT_CUT) return false;
    }
    return true;
}
// ---------------------------------------------- 
bool ElectronSelector::isSemiSignalElectron(const Electron* ele)
{
    /////////////////////////////
    // Electron ID
    /////////////////////////////
    if(m_2lep || m_3lep || m_2lepWH) {
        if(!ele->tightPP) return false;
    }

    /////////////////////////////
    // Impact parameter
    /////////////////////////////
    if(m_doIPCut) {
        if(fabs(ele->d0Sig()) >= EL_MAX_D0SIG_CUT) return false;
        if(fabs(ele->z0SinTheta()) >= EL_MAX_Z0_SINTHETA) return false;
    }
    return true;
}
// ------------------------------------------------------------------------------ // 
//  Isolation
// ------------------------------------------------------------------------------ //
float ElectronSelector::elPtConeCorr(const Electron* ele,
                                     const ElectronVector& baseElectrons,
                                     const MuonVector& baseMuons,
                                     unsigned int nVtx, bool isMC, bool removeLeps)
{
    float ptcone = ele->ptcone30;
    if(removeLeps) {
        for(unsigned int iEl = 0; iEl < baseElectrons.size(); iEl++){
            const Electron* e = baseElectrons[iEl];
            if(ele == e) continue;
            if(!isSemiSignalElectron(e)) continue;
            float dR = ele->DeltaR(*e);
            if(dR < 0.3) ptcone -= e->trackPt;
        }
        for(unsigned int iMu = 0; iMu < baseMuons.size(); iMu++){
            const Muon* mu = baseMuons[iMu];
            if(!MuonSelector()
                            .setSystematic(m_systematic)
                            .setAnalysis(m_analysis)
                            .isSemiSignalMuon(mu)) continue;
            float dR = ele->DeltaR(*mu);
            if(dR < 0.3) ptcone -= mu->idTrackPt;
        }
    }
    return ptcone;
}
// ---------------------------------------------- 
float ElectronSelector::elEtTopoConeCorr(const Electron* ele,
                                     const ElectronVector& baseElectrons,
                                     const MuonVector& baseMuons,
                                     unsigned int nVtx, bool isMC, bool removeLeps)
{
    float slope = isMC ? EL_TOPOCONE30_SLOPE_MC_CUT : EL_TOPOCONE30_SLOPE_DATA_CUT;
    float etcone = ele->topoEtcone30Corr - slope*nVtx;
    if(removeLeps) {
        for(unsigned int iEl = 0; iEl < baseElectrons.size(); iEl++) {
            const Electron* e2 = baseElectrons[iEl];
            if(ele==e2) continue;
            if(!isSemiSignalElectron(e2)) continue;
            float dR = ele->DeltaR(*e2);
            if(dR < 0.28) etcone -= e2->clusE / cosh(e2->clusEta);
        }
    }
    return etcone;
}

}; // namespace Susy
