#include "THnSparse.h"

#include "SusyNtuple/TrilTrigLogic.h"

using namespace std;
using namespace Susy;



/*--------------------------------------------------------------------------------*/
// Constructor
/*--------------------------------------------------------------------------------*/
TrilTrigLogic::TrilTrigLogic()
{
  m_accOnly = false;
  m_dbg = 0;
}

/*--------------------------------------------------------------------------------*/
// Destructor
/*--------------------------------------------------------------------------------*/
TrilTrigLogic::~TrilTrigLogic()
{
}

/*--------------------------------------------------------------------------------*/
// Trigger reweight maps
/*--------------------------------------------------------------------------------*/
void TrilTrigLogic::loadTriggerMaps()
{
}

/*--------------------------------------------------------------------------------*/
APReweightND* TrilTrigLogic::loadTrigWeighter(TFile* f, TString name)
{
  TString numName = "ths_"+name+"_num";
  TString denName = "ths_"+name+"_den";
  // muon file currently contains a typo
  if (name.Contains("mu")) numName = "ths_"+name+"_nom";

  // Does this memory get cleaned up when the file closes?
  THnSparseD* num = (THnSparseD*) f->Get( numName );
  THnSparseD* den = (THnSparseD*) f->Get( denName );
  if(!num || !den){
    cout << "ERROR loading trig maps for chain " << name << endl;
    cout << "num " << num << endl;
    cout << "dun " << den << endl;
    abort();
  }
  return new APReweightND( den, num, true );
}

/*--------------------------------------------------------------------------------*/
// Event trigger requirement without object matching
/*--------------------------------------------------------------------------------*/
bool TrilTrigLogic::passEventTrigger(const Event* evt)
{
  // Take logical OR of the following triggers
  unsigned int mask = TRIG_e24vhi_medium1 |
                      TRIG_2e12Tvh_loose1 |
                      TRIG_e24vh_medium1_e7_medium1 |
                      TRIG_mu24i_tight |
                      TRIG_2mu13 |
                      TRIG_mu18_tight_mu8_EFFS |
                      TRIG_e12Tvh_medium1_mu8 |
                      TRIG_mu18_tight_e7_medium1;
  return evt->passTrig(mask, false);
}
/*--------------------------------------------------------------------------------*/
// Trigger matching for data
/*--------------------------------------------------------------------------------*/
bool TrilTrigLogic::passTriggerMatching(const LeptonVector& leptons, const TauVector& taus, 
                                        const Event* evt, bool useDilepTrigs)
{
  DataStream stream = evt->stream;

  // Temporary 2012 prescription
  // Take the inclusive OR of all triggers for which the plateau is satisfied
  // For the single lepton triggers, only consider the leading lepton (safe for MM)

  // Need to keep track of how many leptons match to each trigger
  uint n1E = 0;         // number of electrons matching to single electron trigger
  uint n1M = 0;         // number of muons matching to single muon trigger

  // We have 2 di-electron triggers and 2 di-muon triggers...
  // For the symmetric dilepton triggers, it's easy
  uint nSym2E = 0;      // number of electrons matching to symm 2 electron trigger
  uint nSym2M = 0;      // number of muons matching to symm 2 muon trigger

  // For the asymmetric dilepton triggers, need to make sure to match both legs.
  // Or at least, match the best we can with the information available!
  uint nAsym2E     = 0; // number of electrons matching e24vh_medium1_e7_medium1
  uint nAsym2E_e24 = 0; // number of electrons matching e24 piece of e24vh_medium1_e7_medium1
  uint nAsym2M     = 0; // number of muons matching mu18_tight_mu8_EFFS
  uint nAsym2M_m18 = 0; // number of muons matching mu18 piece of mu18_tight_mu8_EFFS

  // For the e-mu and mu-e triggers, need to make sure to match both legs
  // Or at least, match the best we can with the information available!
  uint nEM_e = 0;       // number of electrons matching e12 piece of e12Tvh_medium1_mu8
  uint nEM_m = 0;       // number of muons matching mu8 piece of e12Tvh_medium1_mu8
  uint nME_e = 0;       // number of electrons matching e7 piece of mu18_tight_e7_medium1
  uint nME_m = 0;       // number of muons matching mu18 piece of mu18_tight_e7_medium1

  // loop over leptons
  for(uint i=0; i < leptons.size(); i++){

    const Lepton* lep = leptons[i];

    // Preprocessor shorthand for feature matching with eta cuts folded in
    #define matchEle(mask, ptMin) matchLepTrigger(lep, mask, ptMin, 2.47, m_accOnly)
    #define matchMu(mask, ptMin) matchLepTrigger(lep, mask, ptMin, 2.4, m_accOnly)

    // Electrons
    if(lep->isEle()){
      // If leading lepton, check isolated trigger
      if(i==0 && matchEle(TRIG_e24vhi_medium1, 25)) n1E++;
      if(useDilepTrigs){
        // 2e symmetric trigger
        if(matchEle(TRIG_2e12Tvh_loose1, 14)) nSym2E++;
        // 2e asymmetric trigger
        if(matchEle(TRIG_e24vh_medium1_e7_medium1, 10)){
          nAsym2E++;
          if(matchEle(TRIG_e24vh_medium1, 25)) nAsym2E_e24++;
        }
        // e-mu trigger
        if(matchEle(TRIG_e12Tvh_medium1_mu8, 14)) nEM_e++;
        // mu-e trigger (this is the funny one)
        if(matchEle(TRIG_e7_medium1, 10)) nME_e++;
      }
    }

    // Muons
    else{
      // If leading lepton, check isolated trigger
      if(i==0 && matchMu(TRIG_mu24i_tight, 25)) n1M++;
      if(useDilepTrigs){
        // 2m symmetric trigger
        if(matchMu(TRIG_2mu13, 14)) nSym2M++;
        // 2m asymmetric trigger
        if(matchMu(TRIG_mu18_tight_mu8_EFFS, 8)){
          nAsym2M++;
          if(matchMu(TRIG_mu18_tight, 18)) nAsym2M_m18++;
        }
        // mu-e trigger
        if(matchMu(TRIG_mu18_tight_e7_medium1, 18)) nME_m++;
        // e-mu trigger
        if(matchMu(TRIG_mu8, 8)) nEM_m++;
      }
    }

    // Cleanup preprocessor shorthand
    #undef matchEle
    #undef matchMu

  } // lepton loop

  bool pass1E = n1E>0;
  bool passSym2E = nSym2E>1;
  bool passAsym2E = nAsym2E>1 && nAsym2E_e24>0;
  bool passEM = nEM_e>0 && nEM_m>0;
  bool pass1M = n1M>0;
  bool passSym2M = nSym2M>1;
  bool passAsym2M = nAsym2M>1 && nAsym2M_m18>0;
  bool passME = nME_e>0 && nME_m>0;

  bool passEgamma = pass1E || passSym2E || passAsym2E || passEM;
  bool passMuons  = pass1M || passSym2M || passAsym2M || passME;

  /*
  cout << "n1E         " << n1E << endl;
  cout << "nSym2E      " << nSym2E << endl;
  cout << "nAsym2E_e24 " << nAsym2E_e24 << endl;
  cout << "nAsym2E     " << nAsym2E << endl;
  cout << "nEM_e       " << nEM_e << endl;
  cout << "nEM_m       " << nEM_m << endl;
  cout << endl;
  cout << "n1M         " << n1M << endl;
  cout << "nSym2M      " << nSym2M << endl;
  cout << "nAsym2M_m18 " << nAsym2M_m18 << endl;
  cout << "nAsym2M     " << nAsym2M << endl;
  cout << "nME_e       " << nME_e << endl;
  cout << "nME_m       " << nME_m << endl;
  cout << endl;
  cout << "pass1E      " << pass1E << endl;
  cout << "passSym2E   " << passSym2E << endl;
  cout << "passAsym2E  " << passAsym2E << endl;
  cout << "passEM      " << passEM << endl;
  cout << endl;
  cout << "pass1M      " << pass1M << endl;
  cout << "passSym2M   " << passSym2M << endl;
  cout << "passAsym2M  " << passAsym2M << endl;
  cout << "passME      " << passME << endl;
  cout << endl;
  */

  // Stream dependence to avoid double counting
  if(stream==Stream_Egamma && !passEgamma) return false;
  if(stream==Stream_Muons && (!passMuons || passEgamma)) return false;
  if(stream==Stream_MC && !passEgamma && !passMuons) return false;

  // Event passes trigger!
  //cout << endl << "EVENT PASSES TRIGGER" << endl;

  return true;
}
/*--------------------------------------------------------------------------------*/
bool TrilTrigLogic::matchLepTrigger(const Lepton* lep, int trigMask,
                                    float ptMin, float etaMax, bool accOnly)
{
  if(lep->Pt() < ptMin) return false;
  if(etaMax > 0 && fabs(lep->Eta()) > etaMax) return false;
  return accOnly || lep->matchTrig(trigMask);
}
/*--------------------------------------------------------------------------------*/
bool TrilTrigLogic::matchTauTrigger(const Tau* tau, int trigMask, float ptMin, bool accOnly)
{
  if(tau->Pt() < ptMin) return false;
  return accOnly || tau->matchTrig(trigMask);
}

/*--------------------------------------------------------------------------------*/
// Trigger reweighting for MC - not currently used
/*--------------------------------------------------------------------------------*/
float TrilTrigLogic::getTriggerWeight(const LeptonVector& leptons, const Event* evt)
{
  DataStream stream = evt->stream;
  if(stream!=Stream_MC) return 1;

  // for now...
  return 1;
} 

