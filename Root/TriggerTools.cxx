// ------------------------------------------- //
// Class/Tool to handle trigger information    //
// stored in SusyNt                            //
//                                             //
// author: Daniel Antrim                       //
// date  : April 6 2015                        //
// ------------------------------------------- //


#include "TFile.h"
#include "TChain.h"


#include "SusyNtuple/TriggerTools.h"

using namespace std;

using namespace Susy;

/////////////////////////////////////////////////
/////////////////////////////////////////////////
/////////////////////////////////////////////////

/* Trigger "containers"                        */
/*   Used at the SusyNt writing stage to set   */
/*   which triggers are stored in the final    */
/*   output ntuple.                            */

/////////////////////////////////////////////////
/////////////////////////////////////////////////
/////////////////////////////////////////////////


//////////////////////////////////////
// MUON TRIGGERS
//////////////////////////////////////

//
// Single muon triggers
//
const std::vector<std::string> single_muon_trig = {
    "HLT_mu26_imedium",
    "HLT_mu24_imedium",
    "HLT_mu24_iloose_L1MU15",
    "HLT_mu20_iloose_L1MU15",
    "HLT_mu50",
    "HLT_mu60_0eta105_msonly"
};

//
// Dimuon triggers
//
const std::vector<std::string> dimuon_trig = {
    "HLT_2mu14",
    "HLT_2mu10",
    "HLT_mu24_mu8noL1",
    "HLT_mu22_mu8noL1",
    "HLT_mu20_mu8noL1",
    "HLT_mu18_mu8noL1"
};

//
// Muon triggers supported by trigger SF tool (as of 
//
const std::vector<std::string> muon_SF_trig = {
    "HLT_mu14",
    "HLT_mu50",
    "HLT_mu20_iloose_L1MU15",
    "HLT_mu24_iloose_L1MU15",
    "HLT_mu24_imedium",
    "HLT_mu26_imedium",
    "HLT_mu20_iloose_L1MU15_OR_HLT_mu50",
    "HLT_mu24_iloose_L1MU15_OR_HLT_mu50",
    "HLT_mu24_imedium_OR_HLT_mu50",
    "HLT_mu26_imedium_OR_HLT_mu50"
};

//////////////////////////////////////
// ELECTRON TRIGGERS
//////////////////////////////////////

//
// Single electron triggers
//
const std::vector<std::string> single_electron_trig = {
    // cut-based
    "HLT_e26_tight_iloose",
    "HLT_e60_medium",
    "HLT_e24_tight_iloose",
    "HLT_e24_medium_iloose_L1EM20VH",
    "HLT_e24_medium_iloose_L1EM18VH",

    // LH-based
    "HLT_e26_lhtight_iloose",
    "HLT_e60_lhmedium",
    "HLT_e24_lhtight_iloose",
    "HLT_e24_lhmedium_iloose_L1EM20VH",
    "HLT_e24_lhmedium_iloose_L1EM18VH"
};

//
// Di-electron triggers
//
const std::vector<std::string> dielectron_trig = {
    // cut-based 
    "HLT_2e17_loose",
    "HLT_2e15_loose_L12EM13VH",
    "HLT_2e12_loose_L12EM10VH",

    // LH-based
    "HLT_2e17_lhloose",
    "HLT_2e15_lhloose_L12EM13VH",
    "HLT_2e12_lhloose_L12EM10VH"
};

//////////////////////////////////////
// ELE/MUON TRIGGERS
//////////////////////////////////////
const std::vector<std::string> ele_muon_trig = {
    // cut-based
    "HLT_e17_loose_mu14",
    "HLT_e7_medium_mu24",
    "HLT_e26_medium_L1EM22VHI_mu8noL1",
    "HLT_e24_medium_L1EM20VHI_mu8noL1",

    // LH-based
    "HLT_e17_lhloose_mu24",
    "HLT_e7_lhmedium_mu24",
    "HLT_e26_lhmedium_L1EM22VHI_mu8noL1",
    "HLT_e24_lhmedium_L1EM20VHI_mu8noL1"
};

//////////////////////////////////////
// MET TRIGGERS
//////////////////////////////////////
const std::vector<std::string> met_trig = {
    "HLT_xe35",
    "HLT_xe35_pueta",
    "HLT_xe50",
    "HLT_xe50_pueta",
    "HLT_xe70",
    "HLT_xe70_pueta",
    "HLT_xe100",
    "HLT_xe100_pueta"
};


// Trigger list last update:
//     dantrim July 30 2015
std::vector<std::string> TriggerTools::buildTrigList(std::string set)
{

    std::vector<std::string> out_trig;
    if(set.compare("run2")==0){
        out_trig.insert(out_trig.end(), single_muon_trig.begin(), single_muon_trig.end());
        out_trig.insert(out_trig.end(), dimuon_trig.begin(), dimuon_trig.end());
        out_trig.insert(out_trig.end(), single_electron_trig.begin(), single_electron_trig.end());
        out_trig.insert(out_trig.end(), dielectron_trig.begin(), dielectron_trig.end());
        out_trig.insert(out_trig.end(), ele_muon_trig.begin(), ele_muon_trig.end());
        out_trig.insert(out_trig.end(), met_trig.begin(), met_trig.end());
    }
    else if(set.compare("muSF")==0){
        out_trig.insert(out_trig.end(), muon_SF_trig.begin(), muon_SF_trig.end());
    }
    return out_trig;
}


//const std::vector<std::string> all_trig = single_muon_trig;
//all_trig.push_back(dimuon_trig);
//all_trig.insert(all_trig.end(), dimuon_trig.begin(), dimuon_trig.end());
//all_trig.insert(all_trig.end(), single_electron_trig.begin(), single_electron_trig.end());
//all_trig.insert(all_trig.end(), dielectron_trig.begin(), dielectron_trig.end());
//all_trig.insert(all_trig.end(), ele_muon_trig.begin(), ele_muon_trig.end());
//all_trig.insert(all_trig.end(), met_trig.begin(), met_trig.end());


/*
    ////////////////////////////
    // MUON
    ////////////////////////////

    // muon trig
    "HLT_mu26_imedium",
    "HLT_mu24_imedium",
    "HLT_mu24_iloose_L1MU15",
    "HLT_mu20_iloose_L1MU15",
    "HLT_mu50",
    "HLT_mu60_0eta105_msonly",

    // dimuon trig
    "HLT_2mu14",
    "HLT_2mu10",
    "HLT_mu24_mu8noL1",
    "HLT_mu22_mu8noL1",
    "HLT_mu20_mu8noL1",
    "HLT_mu18_mu8noL1",

    ////////////////////////////
    // ELECTRON
    ////////////////////////////

    // electronTrigCut
    "HLT_e26_tight_iloose",
    "HLT_e60_medium",
    "HLT_e24_tight_iloose",
    "HLT_e24_medium_iloose_L1EM20VH",
    "HLT_e24_medium_iloose_L1EM18VH",

    // electronTrigLH
    "HLT_e26_lhtight_iloose",
    "HLT_e60_lhmedium",
    "HLT_e24_lhtight_iloose",
    "HLT_e24_lhmedium_iloose_L1EM20VH",
    "HLT_e24_lhmedium_iloose_L1EM18VH",

    // dielectronTrigCut
    "HLT_2e17_loose",
    "HLT_2e15_loose_L12EM13VH",
    "HLT_2e12_loose_L12EM10VH",

    // dielectronTrigLH
    "HLT_2e17_lhloose",
    "HLT_2e15_lhloose_L12EM13VH",
    "HLT_2e12_lhloose_L12EM10VH",

    ////////////////////////////
    // ELE/MUON
    ////////////////////////////

    // elemuonTrigCut
    "HLT_e17_loose_mu14",
    "HLT_e7_medium_mu24",
    "HLT_e26_medium_L1EM22VHI_mu8noL1",
    "HLT_e24_medium_L1EM20VHI_mu8noL1",

    // elemuonTrigLH
    "HLT_e17_lhloose_mu14",
    "HLT_e7_lhmedium_mu24",
    "HLT_e26_lhmedium_L1EM22VHI_mu8noL1",
    "HLT_e24_lhmedium_L1EM20VHI_mu8noL1",

    ////////////////////////////
    // MET
    ////////////////////////////

    "HLT_xe35",
    "HLT_xe35_pueta",
    "HLT_xe50",
    "HLT_xe50_pueta",
    "HLT_xe70",
    "HLT_xe70_pueta",
    "HLT_xe100",
    "HLT_xe100_pueta"

*/
///};

std::vector<std::string> TriggerTools::getTrigNames(string set)
{ 
    std::vector<std::string> out_trig_list;

    if((set.compare("run2")==0) || (set.compare("muSF")==0)) {
        std::cout << std::endl;
        std::cout << " ------------------------ " << std::endl;
        std::cout << " Getting " << set << " trigger set " << std::endl;
        std::cout << " ------------------------ " << std::endl;
        std::cout << std::endl;
        out_trig_list = TriggerTools::buildTrigList(set);
    }
    else {
        std::cout << "TriggerTools::getTrigNames ERROR    Requested set of triggers ("<< set << ") not available." << std::endl;
        std::cout << "TriggerTools::getTrigNames ERROR    >>> Exiting." << std::endl;
        exit(1);
    }

    return out_trig_list;
}

std::map<std::string, int> TriggerTools::getTriggerMap(std::string set)
{
    std::map<std::string, int> out_map;
    std::vector<std::string> trigs = TriggerTools::buildTrigList(set);
    for(uint iTrig = 0; iTrig<trigs.size(); iTrig++){
        out_map[trigs[iTrig]] = (int)iTrig;
    }
    return out_map;
}


// ---------- !! PARADIGM SHIFT !! ----------- //
// ---------- !! PARADIGM SHIFT !! ----------- //
// ---------- !! PARADIGM SHIFT !! ----------- //



/////////////////////////////////////////////////
/////////////////////////////////////////////////
/////////////////////////////////////////////////

/* Trigger tool                                */
/*    To be used at the analysis level of      */
/*    SusyNt                                   */

/////////////////////////////////////////////////
/////////////////////////////////////////////////
/////////////////////////////////////////////////

// ------------------------------------------- //
// Constructor                                 //
// ------------------------------------------- //
TriggerTools::TriggerTools(TChain* input_chain, bool dbg) :
    m_trigHisto(NULL)
{
    cout << " ---------------------------- " << endl;
    cout << "  Initializing TriggerTools" << endl;
    cout << " ---------------------------- " << endl;
    if(!input_chain) {
        cout << "TriggerTools::TriggerTools ERROR    Cannot initialize TriggerTools! Input TChain is null pointer: " << input_chain << " !" << endl;
        cout << "TriggerTools::TriggerTools ERROR    >>> Exiting." << endl;
        exit(1);
    }
    m_trigHisto = static_cast<TH1F*>(input_chain->GetFile()->Get("trig"));
    m_triggerMap.clear();
    buildTriggerMap();
    cout << " ------------------ " << endl;
    m_dbg = dbg;
}
// ------------------------------------------- //
// Build trigger-map 
// ------------------------------------------- //
// BinLabels <---> trigger name
// BinNumber <---> trigger bit number

void TriggerTools::buildTriggerMap()
{
    for(int trigBin = 1; trigBin < m_trigHisto->GetNbinsX(); trigBin++) {
        string triggerChainName = m_trigHisto->GetXaxis()->GetBinLabel(trigBin);
        m_triggerMap[triggerChainName] = trigBin-1;
        
        if(m_dbg) {
            cout << "Trigger " << triggerChainName << " at bit " << m_triggerMap[triggerChainName] << endl;
        }
    }
}

// ------------------------------------------- //
// Test whether a given trigger has fired
// ------------------------------------------- //
bool TriggerTools::passTrigger(TBits& triggerbits, std::string triggerName)
{
    if(m_triggerMap.find(triggerName)!=m_triggerMap.end()){
        return triggerbits.TestBitNumber(m_triggerMap[triggerName]);
    }
    else {
        std::cout << "Trigger " << triggerName << " not available!!" << std::endl;
        std::cout << "Dumping available triggers and exitting." << std::endl;
        dumpTriggerInfo();
        exit(1);
    }
}

// ------------------------------------------- //
// Dump information about what triggers are
// stored in the SusyNt
// ------------------------------------------- //
void TriggerTools::dumpTriggerInfo()
{
   
    // remember: bit is stored as (bin # - 1) 
    cout << " // ---------------------------------- // " << endl;
    cout << "    Available triggers                    " << endl;
    cout << "    Name : Bit                            " << endl;
    for(int trigBin = 1; trigBin < m_trigHisto->GetNbinsX(); trigBin++) {
        string triggerChainName = m_trigHisto->GetXaxis()->GetBinLabel(trigBin);
        cout << "    " << m_trigHisto->GetXaxis()->GetBinLabel(trigBin) << " : " << trigBin-1 << endl;
    }
    cout << " // ---------------------------------- // " << endl;
}
        
    
    





