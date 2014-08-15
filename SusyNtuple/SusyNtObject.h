#ifndef SusyCommon_SusyNtInterface_h
#define SusyCommon_SusyNtInterface_h

#include "TTree.h"

#include "SusyNtuple/VarHandle.h"
#include "SusyNtuple/SusyNt.h"


namespace Susy
{
  /// An interface class for accessing SusNt objects
  class SusyNtObject : public TObject
  {
    public:
  
      /// Constructor for writing only
      SusyNtObject();
      /// Constructor for reading and writing
      SusyNtObject(const Long64_t& entry);
  
      /// Set branches active for writing
      // I will later add flags here for controlling systematics
      void SetActive();
      /// Connect the objects to an output tree
      void WriteTo( TTree* tree );
      /// Connect the objects to an input tree
      void ReadFrom( TTree* tree );
      /// Clear variables when in read mode
      void clear();

      //
      // SusyNt variables
      // This may change to a map based usage later for systematics
      //

      /// Event variables
      D3PDReader::VarHandle< Event* >                   evt;
      /// Electron variables
      D3PDReader::VarHandle< std::vector<Susy::Electron>* >   ele;
      /// Muon variables
      D3PDReader::VarHandle< std::vector<Susy::Muon>* >       muo;
      /// Jet variables
      D3PDReader::VarHandle< std::vector<Susy::Jet>* >        jet;
      /// Photon variables
      D3PDReader::VarHandle< std::vector<Susy::Photon>* >     pho;
      /// Tau variables
      D3PDReader::VarHandle< std::vector<Susy::Tau>* >        tau;
      /// Met variables
      D3PDReader::VarHandle< std::vector<Susy::Met>* >        met;  
      /// TruthParticle variables
      D3PDReader::VarHandle< std::vector<Susy::TruthParticle>* > tpr;
      /// TruthJet variables
      D3PDReader::VarHandle< std::vector<Susy::TruthJet>* >   tjt;
      /// TruthMet variables
      D3PDReader::VarHandle< std::vector<Susy::TruthMet>* >   tmt;

      ClassDef(SusyNtObject, 3);

    protected:

  
  };


};


#endif
