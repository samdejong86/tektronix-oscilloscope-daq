/*
 *   A file manager class for the tekdaq program. This class manages writing to a ROOT file.
 *   This class could be replaced with one that writes in a different file format
 *
 */

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
  fileManager(ScopeParameters *Params){  /*pass the scope parameters to the filemanager*/
    fname=Params->outfile;               /*get the output filename*/
    Parameters = Params;
  }

  void OpenFile();                       /*Open the file*/
  void CloseFile();                      /*Close the file*/
 
  void ParseMetaData(string Header);                    /*Parse meta data for run (scope settings)*/
  void addEvent(char Curve[4][CLEN], int ByteCount[4]); /*add an event to the file*/
  void parseHeader(char Header[CLEN], int ByteCount);   /*parse the header containing x&y offsets and scale factors*/
    

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
