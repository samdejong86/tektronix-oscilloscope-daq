/*
 * This class represents the oscilloscope
 *
 * by Sam de Jong
 * with lots of code by Paul Poffenberger
 */

//#include 

#include <visa.h>

using namespace std;

#include "fileManager.h"

#ifndef SCOPE_h
#define SCOPE_h 1

class Scope{

 public:

  Scope(){}
  Scope(ScopeParameters *Params){
    Parameters = Params;
    fMan=fileManager(Params);
    fMan.OpenFile();
  }


  void OpenScope();
  void InitScope();
  void CloseScope();
  void ShutDown();

  void TekCmd(string cmd);
  void TekQry(string, char *);
  int WavQry(string cmd, char *);

  int AcquireData();
  void AcquireWaves();
  void Counter(int n);

  void BldHead(string qry);
  void MakeHeader();

  void RunComplete();

  int rewrite;   // flag to write full report text


 private:
 
  ViSession defaultRM;
  ViSession scope;
  ViStatus status;
  ViUInt32 retCount;
  ViChar desc[SLEN];
  char MyString[SLEN];     // generic string variable

  ScopeParameters *Parameters;

  char logilevlcmd[TEKSTR];// logic trigger level command string
  char trlevlcmd[TEKSTR];  // trigger level command string
 
  int ByteCount[4];  // byte count for curve plus header
  char Curve[4][CLEN];  // waveform
  
  
  int pid;      // child process id

  string Header;

  fileManager fMan;
  int event;
  
};



#endif
