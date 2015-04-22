#ifndef SusyNtuple_SusyNtTools_h
#define SusyNtuple_SusyNtTools_h


#include <iostream>

#include "SusyNtuple/SusyDefs.h"
#include "SusyNtuple/SusyNt.h"
#include "SusyNtuple/SusyNtObject.h"
#include "SusyNtuple/MCWeighter.h"
#include "SusyNtuple/JetSelector.h"
#include "SusyNtuple/MuonSelector.h"
#include "SusyNtuple/ElectronSelector.h"
#include "SusyNtuple/OverlapTools.h"
#include "SUSYTools/SUSYCrossSection.h"

using namespace Susy;
using namespace NtSys;

/// A class of useful tools for working with SusyNt
class SusyNtTools {
public:

    /// Constructor and destructor
    SusyNtTools();
    virtual ~SusyNtTools()
    {
    };

    /// Set Analysis type to determine selection
    void setAnaType(AnalysisType A, bool verbose = false)
    {
        m_anaType = A;
        m_electronSelector.setAnalysis(A);
        m_muonSelector.setAnalysis(A);
        m_jetSelector.setAnalysis(A);
        m_overlapTool.setAnalysis(A);
        if (verbose) std::cout << ">>> Setting analysis type to " << SusyNtAnalysisType[A] << std::endl;
    };

    //
    // Methods to grab objects based on systematic shift desired
    //

    /// 'Pre' Objects. Keep same naming convention as I saw in SusyD3PDAna
    ElectronVector getPreElectrons(Susy::SusyNtObject* susyNt, SusyNtSys sys);
    MuonVector     getPreMuons(Susy::SusyNtObject* susyNt, SusyNtSys sys);
    TauVector      getPreTaus(Susy::SusyNtObject* susyNt, SusyNtSys sys);
    JetVector      getPreJets(Susy::SusyNtObject* susyNt, SusyNtSys sys);

    /// Get Baseline objects. Pre + overlap removal.
    /** First method provides the pre-selected objects before OR and baseline objects after OR. */
    void getBaselineObjects(Susy::SusyNtObject* susyNt,
                            ElectronVector& preElecs, MuonVector& preMuons, JetVector& preJets,
                            ElectronVector& elecs, MuonVector& muons, TauVector& taus, JetVector& jets,
                            SusyNtSys sys, bool selectTaus = false);
    /// Second method only provides the baseline objects after OR.
    void getBaselineObjects(Susy::SusyNtObject* susyNt, ElectronVector& elecs,
                            MuonVector& muons, TauVector& taus, JetVector& jets,
                            SusyNtSys sys, bool selectTaus = false);

    /// Signal objects
    ElectronVector getSignalElectrons(const ElectronVector& baseElecs, const MuonVector& baseMuons,
                                      uint nVtx, bool isMC, bool removeLepsFromIso = false);
    MuonVector     getSignalMuons(const MuonVector& baseMuons, const ElectronVector& baseElecs,
                                  uint nVtx, bool isMC, bool removeLepsFromIso = false);
    PhotonVector   getSignalPhotons(Susy::SusyNtObject* susyNt);
    TauVector      getSignalTaus(const TauVector& baseTaus, TauID tauJetID = TauID_medium,
                                 TauID tauEleID = TauID_loose, TauID tauMuoID = TauID_medium);
    JetVector      getSignalJets(const JetVector& baseJets, SusyNtSys sys = NtSys::NOM);
    JetVector      getSignalJets2Lep(const JetVector& baseJets, SusyNtSys sys = NtSys::NOM);

    /// Get the signal objects
    void getSignalObjects(const ElectronVector& baseElecs, const MuonVector& baseMuons,
                          const TauVector& baseTaus, const JetVector& baseJets,
                          ElectronVector& sigElecs, MuonVector& sigMuons,
                          TauVector& sigTaus, JetVector& sigJets, JetVector& sigJets2Lep,
                          uint nVtx, bool isMC, bool removeLepsFromIso = false,
                          TauID tauJetID = TauID_medium, TauID tauEleID = TauID_loose, TauID tauMuoID = TauID_medium,
                          SusyNtSys sys = NtSys::NOM);
    // This method cannot be used anymore because it doesn't provide the baseline objects after OR.
    // Analyzers need these baseline objects for cleaning cuts.
    //void getSignalObjects(Susy::SusyNtObject* susyNt, ElectronVector& sigElecs, 
    //                      MuonVector& sigMuons, TauVector& sigTaus, JetVector& sigJets, 
    //                      JetVector& sigJets2Lep, SusyNtSys sys, uint nVtx, bool isMC, 
    //                      bool selectTaus=false, bool removeLepsFromIso=false,
    //                      TauID tauID=TauID_medium);

    /// New signal tau prescription, fill both ID levels at once
    // These will replace the methods above
    void getSignalTaus(const TauVector& baseTaus, TauVector& mediumTaus, TauVector& tightTaus);
    void getSignalObjects(const ElectronVector& baseElecs, const MuonVector& baseMuons,
                          const TauVector& baseTaus, const JetVector& baseJets,
                          ElectronVector& sigElecs, MuonVector& sigMuons,
                          TauVector& mediumTaus, TauVector& tightTaus,
                          JetVector& sigJets, JetVector& sigJets2Lep,
                          uint nVtx, bool isMC, bool removeLepsFromIso = false,
                          SusyNtSys sys = NtSys::NOM);

    /// Check if selected object
    bool isTauBDT(const Susy::Tau* tau, TauID tauJetID = TauID_medium,
                  TauID tauEleID = TauID_loose, TauID tauMuoID = TauID_medium);
    // TODO: add new selection methods for light leptons and jets
    //bool isSelectLepton()
    //virtual bool isSelectTau(const Susy::Tau* tau, TauID id=TauID_medium);
    virtual bool isSelectTau(const Susy::Tau* tau, TauID tauJetID = TauID_medium,
                             TauID tauEleID = TauID_loose, TauID tauMuoID = TauID_medium);

    /// Check if signal object
    /** Signal lepton definitions include pileup and near-by lepton corrected isolation cuts */
    bool isSignalLepton(const Susy::Lepton* l, const ElectronVector& baseElectrons, const MuonVector& baseMuons,
                        uint nVtx, bool isMC, bool removeLepsFromIso = false);
    bool isSignalElectron(const Susy::Electron* ele, const ElectronVector& baseElectrons, const MuonVector& baseMuons,
                          uint nVtx, bool isMC, bool removeLepsFromIso = false);
    bool isSignalMuon(const Susy::Muon* mu, const ElectronVector& baseElectrons, const MuonVector& baseMuons,
                      uint nVtx, bool isMC, bool removeLepsFromIso = false);
    bool isSignalTau(const Susy::Tau* tau, TauID tauJetID = TauID_medium,
                     TauID tauEleID = TauID_loose, TauID tauMuoID = TauID_medium);
    bool isSemiSignalElectron(const Susy::Electron* ele);
    bool isSemiSignalMuon(const Susy::Muon* mu);

    /// Build Lepton vector - we should probably sort them here
    void buildLeptons(LeptonVector &lep, ElectronVector& ele, MuonVector& muo)
    {
        for (uint ie = 0; ie < ele.size(); ie++)
            lep.push_back(ele[ie]);
        for (uint im = 0; im < muo.size(); im++)
            lep.push_back(muo[im]);
    };

    /// Electron, Muon isolation correction for pileup
    float elPtConeCorr(const Susy::Electron* e, const ElectronVector& baseElectrons, const MuonVector& baseMuons,
                       uint nVtx, bool isMC, bool removeLeps = false);
    float elEtTopoConeCorr(const Susy::Electron* e, const ElectronVector& baseElectrons, const MuonVector& baseMuons,
                           uint nVtx, bool isMC, bool removeLeps = false);
    float muPtConeCorr(const Susy::Muon* mu, const ElectronVector& baseElectrons, const MuonVector& baseMuons,
                       uint nVtx, bool isMC, bool removeLeps = false);
    float muEtConeCorr(const Susy::Muon* mu, const ElectronVector& baseElectrons, const MuonVector& baseMuons,
                       uint nVtx, bool isMC, bool removeLeps = false);

    /// Get the Met, for the appropriate systematic
    Susy::Met* getMet(Susy::SusyNtObject* susyNt, SusyNtSys sys);//, bool useNomPhiForMetSys = true);
    Susy::MetTrack* getMetTrack(Susy::SusyNtObject* susyNt, SusyNtSys sys);//, bool useNomPhiForMetSys = true);

    //
    // Methods for performing overlap removal
    //

    /// Msfos cuts now applied along with overlap removal
    void removeSFOSPair(ElectronVector& elecs, float MllCut);
    void removeSFOSPair(MuonVector& muons, float MllCut);
    void removeSFOSPair(TauVector& taus, float MllCut);

    //
    // Event level checks
    //

    /// No electron or jet in the LAr hole - shouldn't be used anymore
    static bool passLAr(int flag) { return true; }

    /// Pass Tile hot spot veto
    static bool passHotSpot(int flag) { return (flag & ECut_HotSpot); }

    /// Pass the Bad Jet requirement
    static bool passBadJet(int flag) { return (flag & ECut_BadJet); }

    /// Pass the Bad Muon requirement
    static bool passBadMuon(int flag) { return (flag & ECut_BadMuon); }

    /// No cosmic muons
    static bool passCosmic(int flag) { return (flag & ECut_Cosmic); }

    /// Pass Smart Veto
    static bool passSmartVeto(int flag) { return (flag & ECut_SmartVeto); }

    /// Pass All the above, incase you don't care about cut flow
    static bool passAll(int flag)
    {
        int mask = ECut_HotSpot || ECut_BadJet || ECut_BadMuon || ECut_Cosmic || ECut_SmartVeto;
        return (flag & mask) == mask;
    }


    // Additional event level checks added
    // in the event filtering has been turned off
    // NOTE: Filtering on by default!

    /// pass GRL
    static bool passGRL(int flag) { return (flag & ECut_GRL); }

    /// pass LArErr
    static bool passLarErr(int flag) { return (flag & ECut_LarErr); }

    /// pass Tile Err
    static bool passTileErr(int flag) { return (flag & ECut_TileErr); }

    /// Pass TTC veto
    static bool passTTCVeto(int flag) { return (flag & ECut_TTC); }

    /// pass primary vertex
    static bool passGoodVtx(int flag) { return (flag & ECut_GoodVtx); }

    /// pass tile trip cut
    static bool passTileTripCut(int flag) { return (flag & ECut_TileTrip); }

    /// look at the MC truth record and determine whether SUSY propagators were involved
    static bool eventHasSusyPropagators(const std::vector< int > &pdgs,
                                        const std::vector< std::vector< int > > &parentIndices);

    /// Object level cleaning cut methods
    int cleaningCutFlags(int ntCutFlag,
                         const MuonVector& preMuons, const MuonVector& baseMuons,
                         const JetVector& preJets, const JetVector& baseJets);

    /// Bad muon
    bool hasBadMuon(const MuonVector& preMuons);
    /// Cosmic muons
    bool hasCosmicMuon(const MuonVector& baseMuons);
    /// Tile hot spot
    bool hasHotSpotJet(const JetVector& preJets);
    /// Bad jet
    bool hasBadJet(const JetVector& baseJets);

    /// Pass FEB dead region check
    bool passDeadRegions(const JetVector& preJets, const Susy::Met* met, int RunNumber, bool isMC);

    /// Object selection control toggles
    /** Currently all are on by default except muon etcone */
    void setDoPtcone(bool doPtcone = true) { m_doPtconeCut = doPtcone; }
    void setDoElEtcone(bool doElEtcone = true) { m_doElEtconeCut = doElEtcone; }
    void setDoMuEtcone(bool doMuEtcone = false) { m_doMuEtconeCut = doMuEtcone; }
    void setDoIP(bool doIP = true) { m_doIPCut = doIP; }


    //
    // Methods to get useful quantities for event, leptons, or jets
    // Moving global functions from SusyDefs here
    // 

    // Lepton flavor checks
    static bool isSameFlav(const Susy::Lepton* l1, const Susy::Lepton* l2);
    static bool isOppSign(const Susy::Lepton* l1, const Susy::Lepton* l2);
    static bool isOppSign(const Susy::Tau* tau1, const Susy::Tau* tau2);
    static bool isOppSign(const Susy::Lepton* lep, const Susy::Tau* tau);
    static bool isSFOS(const Susy::Lepton* l1, const Susy::Lepton* l2);
    static bool isSFSS(const Susy::Lepton* l1, const Susy::Lepton* l2);
    static bool isOFOS(const Susy::Lepton* l1, const Susy::Lepton* l2);
    static bool hasSFOS(const LeptonVector& leps);
    static bool hasSFSS(const LeptonVector& leps);
    static bool hasOFOS(const LeptonVector& leps);
    static bool hasOS(const LeptonVector& leps);
    static bool hasSS(const LeptonVector& leps);
    static bool hasOS(const TauVector& taus);

    // Mass calculation methods
    static float Mll(const Susy::Particle* l1, const Susy::Particle* l2);
    static float Mlll(const Susy::Lepton* l1, const Susy::Lepton* l2, const Susy::Lepton* l3);
    static float Mllll(const Susy::Lepton* l1, const Susy::Lepton* l2,
                       const Susy::Lepton* l3, const Susy::Lepton* l4);
    static float Mjj(const Susy::Jet* j1, const Susy::Jet* j2);
    static float Mlljj(const Susy::Lepton* l1, const Susy::Lepton* l2,
                       const Susy::Jet* j1, const Susy::Jet* j2);
    //static float Mt(const Susy::Lepton* lep, const Susy::Met* met);
    static float Mt(const TLorentzVector* lep, const Susy::Met* met);
    static float Meff(const JetVector& jets, const Susy::Met* met, float jetPtCut = 40.);
    static float Meff(const LeptonVector& leps, const JetVector& jets, const Susy::Met* met, float jetPtCut = 40.);
    static float Meff(const LeptonVector& leps, const TauVector& taus, const JetVector& jets,
                      const Susy::Met* met, float jetPtCut = 40.);

    // Z selection methods
    static bool isZ(const Susy::Lepton* l1, const Susy::Lepton* l2, float massWindow = 10.);
    static bool isZ(const Susy::Lepton* l1, const Susy::Lepton* l2, const Susy::Lepton* l3,
                    float massWindow = 10.);
    static bool isZ(const Susy::Lepton* l1, const Susy::Lepton* l2, const Susy::Lepton* l3,
                    const Susy::Lepton* l4, float massWindow = 10.);
    static bool isZWindow(const Susy::Lepton* l1, const Susy::Lepton* l2,
                          float minMll = MZ - 10, float maxMll = MZ + 10);
    // NEW Argument useMultiLep will check for mlll and mllll in Z peak
    static bool hasZ(const LeptonVector& leps, float massWindow = 10., bool useMultiLep = false);
    static bool hasZ(const LeptonVector& leps, uint* Zl1, uint* Zl2, float massWindow = 10., bool useMultiLep = false);
    static bool hasZlll(const LeptonVector& leps, float massWindow = 10.);
    static bool hasZllll(const LeptonVector& leps, float massWindow = 10.);
    static bool hasZllZll(const LeptonVector& leps, uint* Z1l1, uint* Z1l2, uint* Z2l1, uint* Z2l2, float massWindow = 10.);
    static bool hasZWindow(const LeptonVector& leps, float minMll = MZ - 10, float maxMll = MZ + 10);
    //void bestZ(uint& l1, uint& l2, const LeptonVector& leps, bool ignoreTau=true);

    // Safer functions, to replace the one above, which return false if no candidates found
    static bool findBestZ(uint& l1, uint& l2, const LeptonVector& leps);
    static bool findBestZ(uint& j1, uint& j2, const JetVector& jets);
    static bool findBestW(uint& j1, uint& j2, const JetVector& jets);

    // B jets
    int numBJets(const JetVector& jets);
    bool hasBJet(const JetVector& jets);
    JetVector getBJets(const JetVector& jets);

    static JetVector getBTagSFJets2Lep(const JetVector& baseJets);
    float bTagSF(const Susy::Event*, const JetVector& jets, int mcID, BTagSys sys = BTag_NOM);

    int numberOfCLJets(const JetVector& jets);
    int numberOfCBJets(const JetVector& jets);
    int numberOfFJets(const JetVector& jets);
    void getNumberOf2LepJets(const JetVector& jets, int& Ncl, int& Ncb, int& Nf,
                             SusyNtSys sys, AnalysisType anaType);

    /// MET Rel
    /**
       Usually built without forward jets; filter jet collection if necessary.
     */
    static float getMetRel(const Susy::Met* met, const LeptonVector& leptons, const JetVector& jets);

    /// MT2
    static float getMT2(const LeptonVector& leptons, const Susy::Met* met);
    static float getMT2(const TLorentzVector* lep1, const TLorentzVector* lep2, const Susy::Met* met);
    static float getMT2(const TLorentzVector* p1, const TLorentzVector* p2, const Susy::Met* met, bool zeroMass, float lspMass = 0);

    /// HT
    static float getHT(const JetVector& jets);


    /// Mljj. Returns mlj if only 1-jet
    static float mljj(const LeptonVector& leptons, const JetVector& jets);


    /// Transverse thrust
    static float getThrT(const LeptonVector& leptons);

    /// Top Tagger methods
    bool passTopTag(const LeptonVector& leptons, const JetVector& jets, const Susy::Met* met,
                    int opt = 0, float ptJetCut = 0, float mEffCut = 100);

    bool toptag0j(TLorentzVector v1l, TLorentzVector v2l, TVector2 met);

    bool toptag2j(double ptjcut, double meffcut, TLorentzVector v1l, TLorentzVector v2l,
                  TLorentzVector v1j, TLorentzVector v2j, TVector2 met, int iopt1);

    float calcMCT(TLorentzVector v1, TLorentzVector v2);


    /// Razor http://arxiv.org/abs/1310.4827
    void superRazor(const LeptonVector& leptons, const Susy::Met* met,
                    TVector3& vBETA_z, TVector3& pT_CM,
                    TVector3& vBETA_T_CMtoR, TVector3& vBETA_R,
                    double& SHATR, double& dphi_LL_vBETA_T, double& dphi_L1_L2,
                    double& gamma_R, double&  dphi_vBETA_R_vBETA_T,
                    double& MDELTAR, double& costhetaRp1);




    //
    // Object printing
    //

    /// Call print on all objects in a vector
    template<class T> void dumpObjects(const std::vector<T>& objects)
    {
        for (unsigned int i = 0; i < objects.size(); i++) {
            std::cout << i << " ";
            objects[i].print();
        }
    }
    template<class T> void dumpObjects(const std::vector<T*>& objects)
    {
        for (unsigned int i = 0; i < objects.size(); i++) {
            std::cout << i << " ";
            objects[i]->print();
        }
    }

    //
    // Misc methods
    //

    /// Sherpa sample check
    bool isSherpaSample(unsigned int mcID);

    ElectronSelector m_electronSelector;
    MuonSelector m_muonSelector;
    JetSelector m_jetSelector; ///< select jets according to the current analysis settings
    OverlapTools m_overlapTool;

protected:

    AnalysisType m_anaType;             ///< Analysis type. currently 2-lep or 3-lep

    // For analyzing different signal lepton cut scenarios, adding some toggles
    // This might only be temporary

    bool m_doPtconeCut;                 ///< ptcone isolation cuts
    bool m_doElEtconeCut;               ///< etcone isolation cuts for electrons
    bool m_doMuEtconeCut;               ///< etcone isolation cuts for muons
    bool m_doIPCut;                     ///< impact parameter cuts
};

#endif
