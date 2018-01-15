#include <vector>

#include "TFile.h"
#include "TTree.h"


#include "tekdaq.h"

using namespace std;

#ifndef FILEMANAGER_h
#define FILEMANAGER_h

class fileManager{

 public:
  fileManager(){}
  fileManager(string filename){
    fname=filename;
  }

  void OpenFile();
  void CloseFile();

  //void initalizeTTree();

  void addEvent(char Curve[4][CLEN], int ByteCount[4]);

  void BldHead(char *qrt);
  void LogIt(int event);

  void parseHeader(char Header[CLEN]);


  void setRecLen(int recordLength){reclen=recordLength;}

  

 private:
  string fname;
  TFile *f;
  TTree *t;

  vector<vector<double> > data;


  int reclen;
  double xin;
  
  string xun, yun;
  double ymu[4], yze[4], yof[4];

  
  
  


};

#endif
