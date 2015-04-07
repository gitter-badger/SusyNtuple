#ifndef SusyNtuple_SusyNt_h
#define SusyNtuple_SusyNt_h


///  SusyNt
/**
   This header file contains definitions for the SusyNt tree structure
   The classes here were adapted from UCINtuple classes but designed to
   be more streamlined for quick analysis.  They should hold the
   minimal content for doing analysis.  If an advanced study requires
   many additional variables, it might be preferred to write a new
   derived class rather than clutter theses classes up.
*/

#include <iostream>

#include "TLorentzVector.h"

#include "SusyNtuple/SusyDefs.h"
#include "SusyNtuple/SusyNtSys.h"

#include "TBits.h"

namespace Susy
{

  /// Event class
  class Event: public TObject
  {
  public:
    Event() :
    trigBits(m_nTriggerBits)
    {
      clear();
    }
    virtual ~Event(){};

    unsigned int run;         ///< run number 
    unsigned int event;       ///< event number
    unsigned int lb;          ///< lumi block number
    DataStream stream;        ///< DataStream enum, defined in SusyDefs.h

    bool isMC;                ///< is MC flag
    unsigned int mcChannel;   ///< MC channel ID number (mc run number)
    float w;                  ///< MC generator weight

    unsigned int larError;    ///< LAr error flag

    unsigned int nVtx;        ///< number of good vertices
    float avgMu;              ///< average interactions per bunch crossing
      
    int hfor;                 ///< heavy flavor overlap removal decision

    int susyFinalState;       ///< Susy process
    int hDecay;               ///< Higgs decay (see WhTruthExtractor)
    bool eventWithSusyProp;   ///< Event generated involving SUSY propagators 
    ///< (See Serhan's msg http://goo.gl/ucwl9)
    int susySpartId1;         ///< SUSY sparticle 1 pdg ID
    int susySpartId2;         ///< SUSY sparticle 2 pdg ID

    float mllMcTruth;         ///< mll from mcTruth (filled for Z->ll overlapping samples)
    bool passMllForAlpgen;    ///< computed from value above; see MultiLep/TruthTools for details

    //unsigned int trigFlags; ///< Event level trigger bits
    long long trigFlags;      ///< Event level trigger bits

    TBits               trigBits;
    static const size_t m_nTriggerBits=64;

    /// Check trigger firing
    /** provide the trigger chain via bit mask, e.g. TRIG_mu18 */
    bool passTrig(long long mask, bool requireAll=true) const {
      if(requireAll) return (trigFlags & mask) == mask;
      else return mask == 0 || (trigFlags & mask) != 0;
    }

    // Event Flag to check for LAr, bad jet, etc. List found in SusyDefs.h under EventCheck
    // Use cutFlags instead
    //int evtFlag[NtSys_N];

    /// Event cleaning cut flags. The bits are defined in SusyDefs as EventCleaningCuts
    unsigned int cutFlags[NtSys::SYSUNKNOWN];

    // Reweighting and scaling
    float wPileup;            ///< pileup weight for full dataset
    float wPileup_up;         ///< pileup weight shifted for systematic
    float wPileup_dn;         ///< pileup weight shifted for systematic
    float wPileupAB3;         ///< pileup weight for 2012 period A-B3 only
    float wPileupAB;          ///< pileup weight for 2012 period A-B only
    float wPileupIL;          ///< pileup weight for 2012 period I,L only (TEMPORARY)
    float wPileupAE;          ///< pileup weight for HCP dataset A-E
    float xsec;               ///< cross section * kfactor * efficiency, from SUSY db
    float errXsec;            ///< cross section uncertainty
    float sumw;               ///< Sum of generator weights 
    float pdfSF;              ///< PDF weight, for scaling 7TeV MC to 8TeV

    /// PDF Systematic information
    int pdf_id1;
    int pdf_id2;
    float pdf_x1;     
    float pdf_x2;
    float pdf_scale;

    float eventScale;
    float alphaQCD;
    float alphaQED;

    /// print event
    void print() const;

    /// Clear vars
    void clear(){
      run = event = lb = 0;
      stream = Stream_Unknown;
      isMC = false;
      mcChannel = w = 0;
      larError = 0;
      nVtx = avgMu = trigFlags = 0;
      hfor = -1;
      susyFinalState = 0;
      mllMcTruth = -1.0;
      hDecay = -1;
      eventWithSusyProp = false;
      susySpartId1 = susySpartId2 = 0;
      passMllForAlpgen = true;
      //memset(evtFlag,0,sizeof(evtFlag));
      memset(cutFlags,0,sizeof(cutFlags));
      wPileup = wPileup_up = wPileup_dn = wPileupAB3 = wPileupAB = wPileupIL = wPileupAE = 0;
      xsec = errXsec = sumw = pdfSF = 0;
      pdf_id1 = pdf_id2 = pdf_x1 = pdf_x2 = pdf_scale = 0;
      eventScale = alphaQCD = alphaQED = 0;
    }

    ClassDef(Event, 28);
  };

  /// Particle class, base class for other object types
  class Particle : public TLorentzVector
  {
  public:
    Particle(){ clear(); }
    virtual ~Particle(){};
    /** Copy constructor */
    Particle(const Particle &);
    /** Assignment operator */
    Particle& operator=(const Particle &);

    // Index in the D3PD; might not be necessary
    unsigned int idx;
      
    // Nominal pt, eta, phi, m, by request
    float pt;
    float eta;
    float phi;
    float m;
    void resetTLV(){ this->SetPtEtaPhiM(pt,eta,phi,m); };
	
    /// Systematic-shifted state for particles.
    /** Base class method simply resets */
    void setState(int sys){ resetTLV(); };

    void clear() { 
      TLorentzVector::Clear();
      idx = 999;      // This is not a very good choice...
      pt = eta = phi = m = 0;
    }

    /// Print method
    virtual void print() const {};

    /// Comparison operators for sorting, etc.
    inline bool operator > (const Particle & other) const {
      return Pt() > other.Pt();
    }
    inline bool operator < (const Particle & other) const {
      return Pt() < other.Pt();
    }

    ClassDef(Particle, 1);
  };

  /// Lepton class, common to electrons and muons 
  class Lepton : public Particle
  {
  public:
    Lepton(){ clear(); }
    virtual ~Lepton(){};
    /** Copy constructor */
    Lepton(const Lepton &);
    /** Assignment operator */
    Lepton& operator=(const Lepton &);

    // public member vars
    int q;                    ///< Charge
    bool isBaseline; ///< flag computed by SUSYTools_xAOD
    bool isSignal; ///< flag computed by SUSYTools_xAOD
    float etcone20;           ///< etcone20 for mu, topoEtcone20_corrected for el
    float ptcone20;           ///< ptcone20 isolation
    float ptcone30;           ///< ptcone30 isolation
    //\todo: move additional iso variables here??

    float d0;                 ///< d0 extrapolated to PV 
    float errD0;              ///< Uncertainty on d0
    float z0;                 ///< z0 extrapolated to PV 
    float errZ0;              ///< Uncertainty on z0

    unsigned int mcType;      ///< MCTruthClassifier particle type
    unsigned int mcOrigin;    ///< MCTruthClassifier particle origin

    bool matched2TruthLepton; ///< flag from RecoTruthMatch::Matched2TruthLepton
    int truthType;            ///< RecoTruthMatch::type

    float effSF;              ///< Efficiency scale factor
    float errEffSF;           ///< Uncertainty on the efficiency scale factor

    //unsigned int trigFlags; ///< Bit word representing matched trigger chains
    long long trigFlags;      ///< Bit word representing matched trigger chains

    /// Methods to return impact parameter variables
    /** Note that these are not absolute valued! */
    float d0Sig() const {
      return d0/errD0;
    }
    float z0SinTheta() const {
      return z0 * sin(Theta());
    }

    /// Trigger matching
    /** Provide the trigger chain via bit mask, e.g. TRIG_mu18 */
    bool matchTrig(unsigned int mask) const {
      return (trigFlags & mask) == mask;
    }

    // Polymorphism, baby!!
    virtual bool isEle() const { return false; }
    virtual bool isMu()  const { return false; }
    void setState(int sys){ resetTLV(); }

    /// Print method
    virtual void print() const {};

    /// Clear vars
    void clear(){
      q = etcone20 = ptcone20 = ptcone30 = 0;
      d0 = errD0 = z0 = errZ0 = 0;
      mcType = mcOrigin = 0;
      matched2TruthLepton = false;
      truthType = -1;
      effSF = 1; 
      errEffSF = 0;
      trigFlags = 0;
      isBaseline = isSignal = false;
      Particle::clear();
    }
      
    ClassDef(Lepton, 13);
  };

  /// Electron class
  class Electron : public Lepton
  {
  public:
    Electron(){ clear(); }
    virtual ~Electron(){};
    Electron(const Electron &);
    /** Assignment operator */
    Electron& operator=(const Electron &);

    // Cluster/track variables
    float clusE;              ///< CaloCluster energy
    float clusEta;            ///< CaloCluster eta
    float clusPhi;            ///< CaloCluster phi
    float trackPt;            ///< ID track pt

    // isEM quality flags
    bool mediumPP;            ///< isEM medium++
    bool tightPP;             ///< isEM tight++
    bool looseLLH;            ///< isEM looseLLH
    bool mediumLLH;           ///< isEM mediumLLH
    bool veryTightLLH;        ///< isEM veryTightLLH

    // New isolation variables, put them here for now
    float etcone30Corr;       ///< Pt and ED corrected etcone iso //AT:2014-10-28: obsolete
    float topoEtcone30Corr;   ///< Corrected topo clus based iso  //AT:2014-10-28: to rename topoEtcone30

    bool isChargeFlip;        ///< Charge flip flag from RecoTruthMatch

    float effSF_LLH;              ///< Efficiency scale factor for LLH electron
    float errEffSF_LLH;           ///< Uncertainty on the efficiency scale factor LLH electron

    // Systematic scale factors
    /*
      float ees_z_up;           ///< Energy Scale Z + sigma
      float ees_z_dn;           ///< Energy Scale Z - sigma
      float ees_mat_up;         ///< Energy Scale Material + sigma
      float ees_mat_dn;         ///< Energy Scale Material - sigma
      float ees_ps_up;          ///< Energy Scale Presampler + sigma
      float ees_ps_dn;          ///< Energy Scale Presampler - sigma
      float ees_low_up;         ///< Energy Scale Low Pt + sigma
      float ees_low_dn;         ///< Energy Scale Low Pt - sigma
      float eer_up;             ///< Energy Reso. + sigma
      float eer_dn;             ///< Energy Reso. - sigma
    */
    //AT: This is insane >50!
    float res_all_dn;
    float res_all_up;
    float res_matCalo_dn;
    float res_matCalo_up;
    float res_matCryo_dn;
    float res_matCryo_up;
    float res_matGap_dn;
    float res_matGap_up;
    float res_matId_dn;
    float res_matId_up;
    float res_nom;
    float res_none;
    float res_pileup_dn;
    float res_pileup_up;
    float res_sampTerm_dn;
    float res_sampTerm_up;
    float res_z_dn;
    float res_z_up;
    float scale_all_dn;
    float scale_all_up;
    float scale_G4_dn;
    float scale_G4_up;
    float scale_L1_dn;
    float scale_L1_up;
    float scale_L2_dn;
    float scale_L2_up;
    float scale_LArCalib_dn;
    float scale_LArCalib_up;
    float scale_LArECalib_dn;
    float scale_LArECalib_up;
    float scale_LArEunconv_dn;
    float scale_LArEunconv_up;
    float scale_LArUnconv_dn;
    float scale_LArUnconv_up;
    float scale_last;
    float scale_matCalo_dn;
    float scale_matCalo_up;
    float scale_matCryo_dn;
    float scale_matCryo_up;
    float scale_matId_dn;
    float scale_matId_up;
    float scale_nom;
    float scale_none;
    float scale_ped_dn;
    float scale_ped_up;
    float scale_ps_dn;
    float scale_ps_up;
    float scale_s12_dn;
    float scale_s12_up;
    float scale_ZeeStat_dn;
    float scale_ZeeStat_up;
    float scale_ZeeSys_dn;
    float scale_ZeeSys_up;
    float scale_mom_dn;
    float scale_mom_up;



    // Polymorphism, baby!!
    bool isEle() const { return true;  }
    bool isMu()  const { return false; } 

    /// Shift energy up/down for systematic
    void setState(int sys);

    /// Print method
    void print() const;

    /// Clear vars
    void clear(){
      clusE = clusEta = clusPhi = trackPt = 0;
      mediumPP = tightPP = false;
      looseLLH= mediumLLH = veryTightLLH = false;
      etcone30Corr = topoEtcone30Corr = 0;
      isChargeFlip = false;
      effSF_LLH = 1; 
      errEffSF_LLH = 0;
      //	ees_z_up = ees_z_dn = ees_mat_up = ees_mat_dn = 0;
      //ees_ps_up = ees_ps_dn = ees_low_up = ees_low_dn = 0;
      //	eer_up = eer_dn = 0;

      res_all_dn= res_all_up= res_matCalo_dn= res_matCalo_up=0;
      res_matCryo_dn= res_matCryo_up= res_matGap_dn= res_matGap_up=0;
      res_matId_dn= res_matId_up= res_nom= res_none=0;
      res_pileup_dn= res_pileup_up=0;
      res_sampTerm_dn=res_sampTerm_up= res_z_dn=res_z_up=0;
      scale_all_dn=scale_all_up=scale_G4_dn=scale_G4_up=0;
      scale_L1_dn=scale_L1_up=scale_L2_dn=scale_L2_up=0;
      scale_LArCalib_dn= scale_LArCalib_up=scale_LArECalib_dn=scale_LArECalib_up=0;
      scale_LArEunconv_dn=scale_LArEunconv_up=scale_LArUnconv_dn=scale_LArUnconv_up=0;
      scale_last=0;
      scale_matCalo_dn=scale_matCalo_up=scale_matCryo_dn=scale_matCryo_up=scale_matId_dn=0;
      scale_matId_up=scale_nom=scale_none=0;
      scale_ped_dn=scale_ped_up=scale_ps_dn=scale_ps_up=scale_s12_dn=scale_s12_up=0;
      scale_ZeeStat_dn=scale_ZeeStat_up=scale_ZeeSys_dn=scale_ZeeSys_up=scale_mom_dn=scale_mom_up=0;
       


      Lepton::clear();
    }

    ClassDef(Electron, 6);
  };

  /// Muon class
  class Muon : public Lepton
  {
  public:
    Muon(){ clear(); }
    virtual ~Muon(){};
    Muon(const Muon &);
    /** Assignment operator */
    Muon& operator=(const Muon &);

    bool isCombined;          ///< Is combined muon, otherwise segment tagged

    float idTrackPt;          ///< ID track pt
    float idTrackEta;         ///< ID track eta
    float idTrackPhi;         ///< ID track phi
    int idTrackQ;             ///< ID track charge

    float msTrackPt;          ///< MS track pt
    float msTrackEta;         ///< MS track eta
    float msTrackPhi;         ///< MS track phi
    int msTrackQ;             ///< ID track charge

    float thetaPV;            ///< Theta extrapolated to PV, by request.
    float etcone30;           ///< etcone iso
    float ptcone30ElStyle;    ///< ptcone with electron style tracks

    // Variables for charge misid
    // \TODO: Redundant variables?!
    float id_theta;
    float id_phi;
    float id_qoverp;
    float ms_theta;
    float ms_phi;
    float ms_qoverp;

    bool isBadMuon;           ///< Bad muon flag from SUSYTools
    bool isCosmic;            ///< Cosmic muon flag from SUSYTools
      
    // Systematic sf
    float ms_up;              ///< MS Pt + sigma
    float ms_dn;              ///< MS Pt - sigma
    float id_up;              ///< ID Pt + sigma
    float id_dn;              ///< ID Pt - sigma
    float scale_up;           ///< SCALE Pt + sigma
    float scale_dn;           ///< SCALE Pt - sigma

    // Polymorphism, baby!!
    bool isEle() const { return false; }
    bool isMu()  const { return true; }
    void setState(int sys);

    /// Print method
    void print() const;

    /// Clear vars
    void clear(){
      isCombined = 0;
      idTrackPt = idTrackEta = idTrackPhi = 0;
      msTrackPt = msTrackEta = msTrackPhi = 0;
      idTrackQ  = msTrackQ = 0;
      thetaPV = etcone30 = ptcone30ElStyle = 0;
      id_theta = id_phi = id_qoverp = 0;
      ms_theta = ms_phi = ms_qoverp = 0;
      isBadMuon = isCosmic = false;
      ms_up = ms_dn = id_up = id_dn = scale_up = scale_dn  = 0;

      Lepton::clear();
    }

    ClassDef(Muon, 7);
  };

  /// Tau class
  class Tau : public Particle
  {
  public:
    Tau() { clear(); }
    virtual ~Tau(){};
    Tau(const Tau &);
    /** Assignment operator */
    Tau& operator=(const Tau &);

    int q;                    ///< Charge
    int author;               ///< reconstruction author
    int nTrack;               ///< number of tracks
    float eleBDT;             ///< electron BDT score
    float jetBDT;             ///< jet BDT score
    int jetBDTSigLoose;       ///< jet BDT working point
    int jetBDTSigMedium;      ///< jet BDT working point
    int jetBDTSigTight;       ///< jet BDT working point
    int eleBDTLoose;          ///< ele BDT working point
    int eleBDTMedium;         ///< ele BDT working point
    int eleBDTTight;          ///< ele BDT working point
    int muonVeto;             ///< muon veto flag

    bool trueTau;             ///< trueTauAssocSmall_matched d3pd variable

    bool matched2TruthLepton; ///< flag from RecoTruthMatch::Matched2TruthLepton
    int truthType;            ///< RecoTauMatch::TauFakeType
    int detailedTruthType;    ///< RecoTauMatch::TauDetailedFakeType

    // Taus have a lot of SFs
    float looseEffSF;         ///< Loose efficiency scale factor
    float errLooseEffSF;      ///< Loose efficiency scale factor uncertaintiy
    float mediumEffSF;        ///< Medium efficiency scale factor
    float errMediumEffSF;     ///< Medium efficiency scale factor uncertainty
    float tightEffSF;         ///< Tight efficiency scale factor
    float errTightEffSF;      ///< Tight efficiency scale factor uncertainty
    float looseEVetoSF;       ///< Loose efficiency scale factor
    float errLooseEVetoSF;    ///< Loose efficiency scale factor uncertaintiy
    float mediumEVetoSF;      ///< Medium efficiency scale factor
    float errMediumEVetoSF;   ///< Medium efficiency scale factor uncertainty
    float tightEVetoSF;       ///< Tight efficiency scale factor
    float errTightEVetoSF;    ///< Tight efficiency scale factor uncertainty
    //float looseEleSF
    //float effSF;            ///< Efficiency scale factor
    //float errEffSF;         ///< Uncertainty on the efficiency scale factor

    // Systematic factors
    float sme_total_up;             ///< tau energy scale + sigma
    float sme_total_dn;             ///< tau energy scale - sigma

    long long trigFlags;      ///< Bit word representing matched trigger chains

    /// Trigger matching
    /** provide the trigger chain via bit mask, e.g. TRIG_mu18 */
    bool matchTrig(long long mask) const {
      return (trigFlags & mask) == mask;
    }

    /// Set systematic state
    void setState(int sys);

    /// Print method
    void print() const;

    void clear(){
      author = nTrack = 0;
      eleBDT = jetBDT = 0;
      jetBDTSigLoose = jetBDTSigMedium = jetBDTSigTight = 0;
      eleBDTLoose = eleBDTMedium = eleBDTTight = 0;
      muonVeto = 0;
      trueTau = false;
      matched2TruthLepton = false;
      truthType = detailedTruthType = -1;
      //effSF = errEffSF = 0;
      looseEffSF = mediumEffSF = tightEffSF = 1;
      errLooseEffSF = errMediumEffSF = errTightEffSF = 0;
      looseEVetoSF = mediumEVetoSF = tightEVetoSF = 1;
      errLooseEVetoSF = errMediumEVetoSF = errTightEVetoSF = 0;
      sme_total_up = sme_total_dn = 0;
      trigFlags = 0;
      Particle::clear();
    }

    ClassDef(Tau, 7);
  };

  /// Photon class
  class Photon : public Particle
  {
  public:
    Photon() { clear(); }
    virtual ~Photon(){};
    Photon(const Photon &);
    /** Assignment operator */
    Photon& operator=(const Photon &);

    /// Conversion Information
    bool isConv;

    //AT 2014-10-29: new v2
    bool  tight;       
    float clusE;              ///< CaloCluster energy
    float clusEta;            ///< CaloCluster eta
    float clusPhi;            ///< CaloCluster phi
    bool  OQ;                 ///< GoodOQ

    float topoEtcone40; 

    // Systematics - not current supported??
    //float pes_up;        // Photon Energy Scale up
    //float pes_dn;        // Photon Energy Scale down
    //float per_up;        // Photon Energy Resolution up
    //float per_dn;        // Photon Energy Resolution down
    //void setState(int sys){ resetTLV();};
      
    /// Print method
    void print() const {};

    /// Clear
    void clear(){
      //pes_up = pes_dn = per_up = per_dn = 0;
      isConv = false;
      tight = false;
      clusE = clusEta = clusPhi = 0;
      OQ = false;
      topoEtcone40=0;
      Particle::clear();
    };
      
    ClassDef(Photon, 2);
  };

  /// Jet class
  class Jet : public Particle
  {
  public:
    Jet(){ clear(); }
    virtual ~Jet(){};
    Jet(const Jet &);
    /** Assignment operator */
    Jet& operator=(const Jet &);

    float jvf;                ///< Jet vertex fraction
    float jvt;                ///< Jet vertex tagger
    float detEta;             ///< Detector eta
    float emfrac;             ///< EM fraction
    int truthLabel;           ///< Flavor truth label
    bool matchTruth;          ///< Matches truth jet

    // btagging
    float sv0;                ///< SV0 btag weight
    float combNN;             ///< JetFitterCombNN btag weight
    float mv1;                ///< MV1 btag weight
    float sv1plusip3d;        ///< SV1plusIP3D btag weight
    float jfit_mass;          ///< secondary vtx mass (dev DG Aug13, will remove when done)
    float sv0p_mass;          ///< ???
    float svp_mass;           ///< ???

    // Flags/variables for cleaning
    bool isBadVeryLoose;      ///< bad jet flag computed with SUSYTools
    bool isHotTile;           ///< tile hot spot flag computed with SUSYTools
    float bch_corr_jet;       ///< Needed for dead region veto
    float bch_corr_cell;      ///< For studying BCH cleaning prescription

    bool isBadMediumBCH;      ///< BCH cleaning flag, https://twiki.cern.ch/twiki/bin/view/AtlasProtected/BCHCleaningTool
    bool isBadMediumBCH_up;   ///< BCH cleaning flag
    bool isBadMediumBCH_dn;   ///< BCH cleaning flag
    bool isBadTightBCH;       ///< BCH cleaning flag

    // Systematics
    float jes_up;             ///< jet energy scale up  //AT obsolete?
    float jes_dn;             ///< jet energy scale down//AT obsolete?
    float jer;                ///< jet energy resolution

    //ADD SYS!!! 18x2 + JER
    std::vector<float> bjes;
    std::vector<float> effNp;
    std::vector<float> etaInter;
    std::vector<float> flavor;
    std::vector<float> pileup;
    std::vector<float> punchThrough;
    std::vector<float> relativeNC;
    std::vector<float> singlePart;

    // Jet-Met Weights
    float met_wpx;
    float met_wpy;

    // Shift energy for systematic
    void setState(int sys);

    // Print method
    void print() const;

    // Clear vars
    void clear(){
      jvf = jvt = truthLabel = 0;
      matchTruth = false;
      detEta = 0;
      emfrac = 0;
      sv0 = combNN = mv1 = sv1plusip3d = 0;
      jfit_mass = sv0p_mass = svp_mass = 0;
      isBadVeryLoose = isHotTile = false;
      bch_corr_jet = bch_corr_cell = 0;
      isBadMediumBCH = isBadMediumBCH_up = isBadMediumBCH_dn = isBadTightBCH = false;
      jer = jes_up = jes_dn = 0;
      met_wpx = met_wpy = 0;
	
      bjes.resize(2,0.0);
      effNp.resize(6*2,0.0);
      etaInter.resize(2*2,0.0);
      flavor.resize(2*2,0.0);
      pileup.resize(4*2,0.0);
      punchThrough.resize(2,0.0);
      relativeNC.resize(2,0.0);
      singlePart.resize(2,0.0);

      Particle::clear();
    }

    ClassDef(Jet, 13);
  };

  /// Met class
  /**
     No longer inherits from Particle or TLorentzVector
  */
  class Met : public TObject
  {
  public:
    Met(){ clear(); }
    virtual ~Met(){};
    /** Copy constructor */
    Met(const Met &);
    /** Assignment operator */
    Met& operator=(const Met &);
      
    float Et;
    float phi;
    float sumet;

    /// Build a TLorentzVector on the fly
    const TLorentzVector lv() const {
      TLorentzVector tlv;
      tlv.SetPtEtaPhiE(this->Et,0,this->phi,this->Et);
      return tlv;
    }

    // MET Composition info - do we want TLorentzVectors, TVector2, or just floats?
    // TODO: clean out the obsolete terms
    float refEle;             ///< Ref electron term magnitude
    float refEle_etx;         ///< Ref electron term
    float refEle_ety;         ///< Ref electron term
    float refEle_sumet;       ///< Ref electron sumet
    float refMuo;             ///< Ref muon term magnitude
    float refMuo_etx;         ///< Ref muon term
    float refMuo_ety;         ///< Ref muon term
    float refMuo_sumet;       ///< Ref muon sumet
    float refJet;             ///< Ref jet term
    float refJet_etx;         ///< Ref jet term
    float refJet_ety;         ///< Ref jet term
    float refJet_sumet;       ///< Ref jet sumet
    float softJet;            ///< Soft jet term
    float softJet_etx;        ///< Soft jet term
    float softJet_ety;        ///< Soft jet term
    //float softJet_sumet;       // No Need, this term obsolete
    float refGamma;           ///< Ref gamma term
    float refGamma_etx;       ///< Ref gamma term
    float refGamma_ety;       ///< Ref gamma term
    float refGamma_sumet;     ///< Ref gamma term
    float refCell;            ///< Cellout term
    float refCell_etx;        ///< Cellout term
    float refCell_ety;        ///< Cellout term
    //float refCell_sumet;       // No Need, this term obsolete
    float softTerm;           ///< Soft term
    float softTerm_etx;       ///< Soft term
    float softTerm_ety;       ///< Soft term
    float softTerm_sumet;     ///< Soft term sumet

    // I'm not a big fan of the way this is setup, would like to improve it someday
    int sys;                  ///< Met stored in vector for each sys shift, this identifies met for specific shift

    /// print vars
    void print() const;

    /// Clear vars
    void clear(){
      Et = phi = sumet = 0;
      refEle = refMuo = refJet = softJet = refGamma = refCell = softTerm = 0;
      refEle_etx = refMuo_etx = refJet_etx = softJet_etx = refGamma_etx = refCell_etx = softTerm_etx = 0;
      refEle_ety = refMuo_ety = refJet_ety = softJet_ety = refGamma_ety = refCell_ety = softTerm_ety = 0;
      sys = 0;
      refEle_sumet = refMuo_sumet = refJet_sumet = refGamma_sumet = softTerm_sumet = 0;
    }

    ClassDef(Met, 4);
  };

  /// TruthParticle
  class TruthParticle : public Particle
  {
  public:
    TruthParticle(){ clear(); }
    virtual ~TruthParticle(){};
    TruthParticle(const TruthParticle &);
    /** Assignment operator */
    TruthParticle& operator=(const TruthParticle &);

    int pdgId;
    int charge;
    int status;
    int motherPdgId;

    // Print method
    void print() const;

    void clear(){
      pdgId  = 0;
      charge = 0;
      status = 0;
      motherPdgId = 0;
      Particle::clear();
    }

    ClassDef(TruthParticle,1);
  };

  /// TruthJet
  class TruthJet : public Particle
  {
  public:
    TruthJet(){ clear(); }
    virtual ~TruthJet(){};
    TruthJet(const TruthJet &);
    /** Assignment operator */
    TruthJet& operator=(const TruthJet &);

    int flavor;

    // Print method
    void print() const;

    void clear(){
      flavor = 0;
      Particle::clear();
    }

    ClassDef(TruthJet,1);
  };

  /// TruthMet class
  class TruthMet : public TObject
  {
  public:
    TruthMet(){ clear(); }
    virtual ~TruthMet(){};
    /** Copy constructor */
    TruthMet(const TruthMet &);
    /** Assignment operator */
    TruthMet& operator=(const TruthMet &);
      
    float Et;
    float phi;

    /// Build a TLorentzVector on the fly
    const TLorentzVector lv() const {
      TLorentzVector tlv;
      tlv.SetPtEtaPhiE(this->Et,0,this->phi,this->Et);
      return tlv;
    }

    // print vars
    void print() const;

    // Clear vars
    void clear(){
      Et  = 0;
      phi = 0;
    }

    ClassDef(TruthMet, 1);
  }; 

    /// FatJet class
    class FatJet : public Jet
    {
    public:
        FatJet() { clear(); }
        virtual ~FatJet(){};
        FatJet(const FatJet &);
        /** Assignment operator **/
        FatJet& operator=(const FatJet &);

        float jetmass;          ///< Jet mass

        void print() const;

        // Clear vars
        void clear(){
            jetmass = 0;

            Particle::clear();
    }
    ClassDef(FatJet, 1);
    };

};

#endif
