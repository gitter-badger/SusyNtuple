#ifndef SusyNtuple_ChainHelper_h
#define SusyNtuple_ChainHelper_h

#include <fstream>

#include "TFile.h"
#include "TChain.h"

/**
   Static helper methods to build a TChain from input root files

   The recommended method is ChainHelper::addInput().
   The input can be one of the following
   - single input root file
   - input directory of root files
   - a file with list of root files
   - comma-separated list of any of the above
*/

class ChainHelper
{

  public:

    //ChainHelper(std::string treeName = "");
    //~ChainHelper(){};

    enum Status {
      GOOD = 0,
      BAD = 1
    };

    /// Add a file - not very useful (obsolete, use addInput() instead)
    static Status addFile(TChain* chain, std::string file);

    /// Add a fileList (obsolete, use addInput() instead)
    static Status addFileList(TChain* chain, std::string fileListName);

    // Add all files in a directory (obsolete, use addInput() instead)
    static Status addFileDir(TChain* chain, std::string fileDir);

    /// add generic input
    /**
       Determine internally whether it's a file, filelist, or
       directory. Also accepts comma-separated list of inputs.
     */
    static Status addInput(TChain* chain, const std::string &input, bool verbose=false);
    /// input files are expected to have the '.root' extension
    static bool inputIsFile(const std::string &input);
    /// input filelists are expected to have the '.txt' extension
    static bool inputIsList(const std::string &input);
    /// input directories are expected to end with '/'
    static bool inputIsDir(const std::string &input);

};



#endif
