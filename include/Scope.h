/*
 * This class represents the oscilloscope
 *
 * by Sam de Jong
 * with lots of code by Paul Poffenberger
 */

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


  void OpenScope();   /*open connection to scope*/
  void InitScope();   /*initalize scope with user settings*/
  void CloseScope();  /*close scope connection*/ 
  void ShutDown();    /*shutdown*/

  void TekCmd(string cmd);   /*sent command to scope*/
  void TekQry(string, char *); /*query scope*/
  int WavQry(string cmd, char *);/*Read waveform from scope*/

  int AcquireData();   /*acquire waveform amplitudes*/
  void AcquireWaves(); /*acqure waveforms*/
  void Counter(int n); /*counter*/

  void BldHead(string qry); /*add an entry to the header*/
  void MakeHeader();   /*Generate the run header*/

  void RunComplete();  /*called when run is finished*/

  int rewrite;   // flag to write full report text


 private:
 
  ViSession defaultRM;
  ViSession scope;
  ViStatus status;
  ViUInt32 retCount;
  ViChar desc[SLEN];
  char MyString[SLEN];     // generic string variable
  int pid;                 // child process id
  string Header;           // run header

  ScopeParameters *Parameters; //object contains the parameters of the run

  char logilevlcmd[TEKSTR];       // logic trigger level command string
  char trlevlcmd[TEKSTR];         // trigger level command string
  
  int ByteCount[4];     // byte count for curve plus header
  char Curve[4][CLEN];  // waveform  
  int event;            //event counter

  fileManager fMan; //file manager
  
};



#endif
