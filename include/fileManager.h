#include <vector>

#include "TFile.h"
#include "TTree.h"


#include "tekdaq.h"
#include "ScopeParameters.h"

using namespace std;

#ifndef FILEMANAGER_h
#define FILEMANAGER_h

class fileManager{

 public:
  fileManager(){}
  fileManager(ScopeParameters *Params){
    fname=Params->outfile;
    Parameters = Params;
  }

  void OpenFile();
  void CloseFile();

  void ParseMetaData(string Header);
  void addEvent(char Curve[4][CLEN], int ByteCount[4]);
  void parseHeader(char Header[CLEN], int ByteCount);
    

 private:
  string fname;
  TFile *f;
  TTree *t;

  vector<vector<double> > data;

  double xin;
  double CH1, CH2, CH3, CH4;
  double time;

  string xun, yun;
  double ymu[4], yze[4], yof[4];

  ScopeParameters *Parameters;
  
  

};

#endif
