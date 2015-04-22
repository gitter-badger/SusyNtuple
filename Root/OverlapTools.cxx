#include "SusyNtuple/OverlapTools.h"

#include <set>
#include <iostream>

using Susy::OverlapTools;
using Susy::Electron;
using Susy::Muon;
using std::cout;
using std::endl;


namespace Susy {


// ------------------------------------------------------------------------------ //
// Constructor
// ------------------------------------------------------------------------------ //
OverlapTools::OverlapTools() :
    m_doHarmonization(false),
    m_verbose(false)
{
}
// ------------------------------------------------------------------------------ //
OverlapTools& OverlapTools::setAnalysis(const AnalysisType& a)
{
    switch(a) {
    ///////////////////////////////////
    // Contrarian analyses
    ///////////////////////////////////
    case(Ana_2Lep) : 
    case(Ana_3Lep) : 
    case(Ana_2LepWH) : {
        m_doHarmonization = false;
        break;
    }
    ///////////////////////////////////
    // Analyses using harmonized OR
    ///////////////////////////////////
    // None yet!
    ///////////////////////////////////
    // Too far, set default to Run-I
    ///////////////////////////////////
    case(Ana_N) : {
        cout << "OverlapTools::setAnalysis() error: invalid analysis '" << a << "'" << endl;
        cout << "           will apply default overlap procedure (Run-I)." << endl;
#warning We should set the defaullt selections ffor tools to be a consistent AnaType
        m_doHarmonization = false;
        break;
    }
    } // switch

    return *this;
}
// ------------------------------------------------------------------------------ //
bool OverlapTools::doHarmonization()
{
    return m_doHarmonization;
}
// ------------------------------------------------------------------------------ //
// Main overlap removal function
// ------------------------------------------------------------------------------ //

void OverlapTools::performOverlap(ElectronVector& electrons, MuonVector& muons,
                                    TauVector& taus, JetVector& jets)
{

    /** Following the prescription descibed in:
    https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/SusyObjectDefinitionsr1913TeV#Overlap_Removals
    */

    ///////////////////////////////////////////
    // Remove jets overlapping with electrons
    ///////////////////////////////////////////
    j_e_overlap(electrons, jets);

    ///////////////////////////////////////////
    // Remove electrons overlapping with jets
    ///////////////////////////////////////////
    e_j_overlap(electrons, jets);

    ///////////////////////////////////////////
    // Remove muons overlapping with jets
    ///////////////////////////////////////////
    m_j_overlap(muons, jets);

    ///////////////////////////////////////////
    // Remove electrons and muons that overlap
    ///////////////////////////////////////////
    e_m_overlap(electrons, muons);

    ///////////////////////////////////////////
    // Remove electrons that overlap with 
    // each other
    ///////////////////////////////////////////
    e_e_overlap(electrons);

    // TODO: Add Tau overlaps
    // if(!m_doHarmonization){
    //    --> do tau overlap
    //    --> harmonized OR doesn't include taus
    //    }
}

// ------------------------------------------------------------------------------ //
// Components of Overlap Removal
// -----------------------------------------------------------------------------  //

////////////////////////////////////////////
// Remove jets overlapping with electrons
//  J_E_DR = 0.2
///////////////////////////////////////////
void OverlapTools::j_e_overlap(ElectronVector& electrons, JetVector& jets)
{
    if(electrons.size()==0 || jets.size()==0) return;
    if(m_doHarmonization || !m_doHarmonization) // just making it explicit that it is same for both
    {
        for(uint iEl=electrons.size()-1; iEl>=0; iEl--){
            const Electron* e = electrons.at(iEl);
            for(uint iJ=jets.size()-1; iJ>=0; iJ--){
                const Jet* j = jets.at(iJ);
                if(e->DeltaR(*j) > J_E_DR) continue;
                jets.erase(jets.begin()+iJ);
            } // iJ
        } // iEl
    } // if doHarm || not doHarm
}
////////////////////////////////////////////
// Remove electrons overlapping with jets
//  E_J_DR = 0.4
////////////////////////////////////////////
void OverlapTools::e_j_overlap(ElectronVector& electrons, JetVector& jets)
{
    if(electrons.size()==0 || jets.size()==0) return;
    if(m_doHarmonization || !m_doHarmonization) // just making it explicit that it is same for both
    {
        for(uint iEl=electrons.size()-1; iEl>=0; iEl--){
            const Electron* e = electrons.at(iEl);
            for(uint iJ=jets.size()-1; iJ>=0; iJ--){
                const Jet* j = jets.at(iJ);
                if(e->DeltaR(*j) > E_J_DR) continue;
                electrons.erase(electrons.begin()+iEl);
                break; // move to next electron since iEl no longer exists!
            } // iJ
        } // iEl
    } // if doHarm || not doHarm (that is the question)
}
////////////////////////////////////////////
// Remove muons overlapping with jets
//  M_J_DR = 0.4
////////////////////////////////////////////
void OverlapTools::m_j_overlap(MuonVector& muons, JetVector& jets)
{
    if(muons.size()==0 || jets.size()==0) return;
    if(!m_doHarmonization){
        for(uint iMu=muons.size()-1; iMu>=0; iMu--){
            const Muon* mu = muons.at(iMu);
            for(uint iJ=jets.size()-1; iJ>=0; iJ--){
                const Jet* j = jets.at(iJ);
                if(mu->DeltaR(*j) > M_J_DR) continue;
                muons.erase(muons.begin()+iMu);
                break; // move to next muon since iMu no longer exists!
            } // iJ
        } // iMu
    } // !doHarm
    else if(m_doHarmonization){
        #warning Need to store number of jet tracks to perform Run-II overlap!
        cout << "OverlapTools::m_j_overlap() warning: Unable to perform Run-2 m_j_overlap." << endl;
        cout << "Need to store jet track information first!" << endl;
//        for(uint iMu=muons.size()-1; iMu>=0; iMu--){
//            const Muon* mu = muons.at(iMu);
//            for(uint iJ=jets.size()-1; iJ>=0; iJ--){
//                const Jet* j = jets.at(iJ);
//                int jet_nTrk = jets.at(iJ)->nTracks;
//                if(jet_nTrk<3) jets.erase(jets.begin()+iJ);
//                else{
//                    muons.erase(muons.begin()+iMu);
//                    break; // move to next muon since iMu no longer exists!
//                }
//            } // iJ
//        } // iMu
    } // doHarm
}

////////////////////////////////////////////
// Remove electrons and muons overlapping 
// with one another
//  E_M_DR = 0.01
////////////////////////////////////////////
void OverlapTools::e_m_overlap(ElectronVector& electrons, MuonVector& muons)
{
    uint nEl = electrons.size();
    uint nMu = muons.size();
    if(nEl==0 || nMu==0) return;
    
    static std::set<const Electron*> electronsToRemove;
    static std::set<const Muon*> muonsToRemove;
    electronsToRemove.clear();
    muonsToRemove.clear();
    
    for(uint iEl=0; iEl<nEl; iEl++){
        const Electron* e = electrons.at(iEl);
        for(uint iMu=0; iMu<nMu; iMu++){
            const Muon* mu = muons.at(iMu);
            if(e->DeltaR(*mu) < E_M_DR){
                electronsToRemove.insert(e);
                muonsToRemove.insert(mu);
            } // dR match
        } // iMu
    } // iEl
    
    // remove the flagged electrons
    if(electronsToRemove.size()){
        for(uint iEl=nEl-1; iEl>=0; iEl--){
            if(electronsToRemove.find(electrons.at(iEl)) != electronsToRemove.end()){
                electrons.erase(electrons.begin()+iEl);
            } // found one
        } // iEl
    } // el2remove

    // remove the flagged muons
    if(muonsToRemove.size()){
        for(uint iMu=nMu-1; iMu>=0; iMu--){
            if(muonsToRemove.find(muons.at(iMu)) != muonsToRemove.end()){
                muons.erase(muons.begin()+iMu);
            } // found one
        } // iMu
    } // mu2remove
}
    
////////////////////////////////////////////
// Remove (sub-lead) electron overlapping
// with (lead) lepton
//  E_E_DR = 0.05
////////////////////////////////////////////
void OverlapTools::e_e_overlap(ElectronVector& electrons)
{
    uint nEl=electrons.size();
    if(nEl<2) return;
    
    // Find all possible e-e pairings
    static std::set<const Electron*> electronsToRemove;
    electronsToRemove.clear();
    for(uint iEl=0; iEl<nEl; iEl++){
        const Electron* ei = electrons.at(iEl);
        for(uint jEl=iEl+1; jEl<nEl; jEl++){
            const Electron* ej = electrons.at(jEl);
            if(ei->DeltaR(*ej) < E_E_DR){
                if(ei->Pt() < ej->Pt()){
                    electronsToRemove.insert(ei);
                    break; // ei no longer exists for looping!
                } else{
                    electronsToRemove.insert(ej);
                }
            } // dR match
        } // jEl
    } // iEl
    
    // remove the flagged electrons
    for(uint iEl=nEl-1; iEl>=0; iEl--){
        if(electronsToRemove.find(electrons.at(iEl)) != electronsToRemove.end()){
            electrons.erase(electrons.begin()+iEl);
        } // found one
    }
}
////////////////////////////////////////////
// Remove muons overlapping with one another
//  M_M_DR = 0.05
////////////////////////////////////////////
void OverlapTools::m_m_overlap(MuonVector& muons)
{
    uint nMu = muons.size();
    if(nMu < 2) return;
    
    // if 2 muons overlap, flag both for removal
    static std::set<const Muon*> muonsToRemove;
    muonsToRemove.clear();
    for(uint iMu=0; iMu<nMu; iMu++){
        const Muon* imu = muons.at(iMu);
        for(uint jMu=iMu+1; jMu<nMu; jMu++){
            const Muon* jmu = muons.at(jMu);
            if(imu->DeltaR(*jmu) < M_M_DR){
                muonsToRemove.insert(imu);
                muonsToRemove.insert(jmu);
            } // if OR
        } // jMu
    } // iMu
    // remove flagged muons
    for(int iMu=nMu-1; iMu>=0; iMu--){
        const Muon* mu = muons.at(iMu);
        if(muonsToRemove.find(mu) != muonsToRemove.end()) {
            muons.erase(muons.begin()+iMu);
        } // remove
    } // iMu
}
////////////////////////////////////////////
// Remove taus overlapping with electrons
//  T_E_DR = 0.2
////////////////////////////////////////////
void OverlapTools::t_e_overlap(TauVector& taus, ElectronVector& electrons)
{
    uint nTau = taus.size();
    uint nEle = electrons.size();
    if(nTau==0 || nEle==0) return;
    for(int iTau=nTau-1; iTau>=0; iTau--){
        const Tau* tau = taus.at(iTau);
        for(int iEl=nEle-1; iEl>=0; iEl--){
            const Electron* e = electrons.at(iEl);
            if(tau->DeltaR(*e) < T_E_DR){
                taus.erase(taus.begin()+iTau);
                break; // loop tau doesn't exist anymore!
            } // dR match
        } // iEl
    } // iTau
}
////////////////////////////////////////////
// Remove taus overlapping with muons
//  T_M_DR = 0.2
////////////////////////////////////////////
void OverlapTools::t_m_overlap(TauVector& taus, MuonVector& muons)
{
    uint nTau = taus.size();
    uint nMuo = muons.size();
    if(nTau==0 || nMuo==0) return;
    for(int iTau=nTau-1; iTau>=0; iTau--){
        const Tau* tau = taus.at(iTau);
        for(int iMu=nMuo-1; iMu>=0; iMu--){
            const Muon* mu = muons.at(iMu);
            if(tau->DeltaR(*mu) < T_M_DR){
                taus.erase(taus.begin()+iTau);
                break; // loop tau doesn't exist anymore!
            } // dR match
        } // iMu
    } // iTau
}
////////////////////////////////////////////
// Remove jets overlapping with taus
//  J_T_DR = 0.2
////////////////////////////////////////////
void OverlapTools::j_t_overlap(TauVector& taus, JetVector& jets)
{
    uint nTau=taus.size();
    uint nJet=jets.size();
    if(nTau==0 || nJet==0) return;
    for(int iTau=nTau-1; iTau>=0; iTau--){
        const Tau* tau = taus.at(iTau);
        for(int iJet=nJet-1; iJet>=0; iJet--){
            const Jet* jet=jets.at(iJet);
            // dantrim Apr 21 : previously, by default, we removed the jet over the tau --> do we want to have the option of removing the tau over the jet?
            if(tau->DeltaR(*jet) < J_T_DR) {
                jets.erase(jets.begin()+iJet); 
            } // dR match
        } // iJet
    } // iTau
}

/*
 if(tau->DeltaR(*jet) < J_T_DR) {
    if(removeJets) jets.erase(jets.begin()+iJet);
 else {
    taus.erase(taus.begin()+iTau_;
    break;
  }
*/



}; // namespace Susy
