#include "SusyNtuple/OverlapTools.h"

#include <set>
#include <iostream>

using Susy::OverlapTools;
using Susy::Electron;
using Susy::Muon;
using std::cout;
using std::endl;


namespace Susy {


// --------------------------------------------------------------------------------------------- //
// Constructor
// --------------------------------------------------------------------------------------------- //
OverlapTools::OverlapTools() :
    m_doHarmonization(false),
    m_verbose(false)
{
}
// --------------------------------------------------------------------------------------------- //
OverlapTools& OverlapTools::setHarmonization()
{
    m_doHarmonization = true;
    return *this;
}
// --------------------------------------------------------------------------------------------- //
bool OverlapTools::doHarmonization()
{
    return m_doHarmonization;
}
// --------------------------------------------------------------------------------------------- //
// Main overlap removal function
// --------------------------------------------------------------------------------------------- //
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
}

// --------------------------------------------------------------------------------------------- //
// Components of Overlap Removal
// --------------------------------------------------------------------------------------------- //

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
// Remove electrons and muontsoverlapping 
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



}; // namespace Susy
