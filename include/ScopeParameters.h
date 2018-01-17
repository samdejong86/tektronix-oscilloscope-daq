/*
 *  This class contains all the scope parameters, as well as code to parse them
 *  from the command line arguments
 */

using namespace std;

#include "tekdaq.h"
#include "xmlParser.h"

#ifndef SCOPEPARAMS_h
#define SCOPEPARAMS_h


class ScopeParameters{

 public:

  ScopeParameters(){}

  ScopeParameters(int argc, char *argv[]);  

  void usage (char *prog);  //print usage
  void  Defaults ();        //print defaults
  void printVerbose();      //print verbose

  void xmlSettings();


  //Parameters are all public, since I didn't want to write 65 get/set methods...
  
  int getopt_Verbose;   			// getopt verbose flag
  int quiet; 					// quiet mode
  int keep;				        // use program defaults instead of existing scope settings
  int TEK_Verbose;				// TEK scope verbose flag
  int ipc_Verbose;				// ipc signal verbose flag
  int WriteToFile;				// write-to-file flag
  int measureData;				// amplitude measurements will be made by the scope
  int passCuts;					// flag that measurements pass cuts
  int nevent;					// maximum number events to measure
  int prescale;					// number to prescale the logged events
  int average;                                    // number of events to average
  int duration;					// maximum number of seconds to aquire data
  int interval;					// sleep time between data acquisitions
  int aq_timeout;					// aquistion timeout in seconds
  int lock;					// front panel lock flag
  int TestMode;					// test mode flag
  int ReportFreq;					// report number events measured frequency
  int RecLen;					// waveform record length
  float MinAmp[4];				// minimum amplitudes required to record event
  
  char hsamp[TEKSTR];				// sample rate
  char trsrc[TEKSTR];				// trigger source command string
  char trlevl[TEKSTR];				// trigger level value
  char trslop[TEKSTR];				// trigger slope command string
  char htrpos[TEKSTR];				// pretrigger amount
  
  double CH1, CH2, CH3, CH4;			// measured signal amplitudes for each channel
  
  int logicCH1;					// logic trigger requires ch 1
  int logicCH2;					// logic trigger requires ch 2
  int logicCH3;					// logic trigger requires ch 3
  int logicCH4;					// logic trigger requires ch 4
  int logicAND;					// logic condition is AND
  int logicOR;					// logic condition is OR
  int logicLO;					// logic input is LOW
  int logicHI;					// logic input is HIGH
  
  int opt_N[4];					// getopt select flags...
  int opt_l;
  int opt_a;
  int opt_c;
  int opt_A;
  int opt_B;
  int opt_C;
  int opt_D;
  int opt_1;
  int opt_2;
  int opt_3;
  int opt_4;
  int opt_5;
  int opt_6;
  int opt_7;
  int opt_8;
  int opt_P;
  int opt_Q;
  int opt_R;
  int opt_S;
  int opt_t;
  int opt_s;
  int opt_p;
  int opt_b;
  int opt_X;
  
  char pos[4][TEKSTR];   			// vertical position
  char vscal[4][TEKSTR]; 			// vertical scale command string
  char coupl[4][TEKSTR]; 			// coupling command string
  char imped[4][TEKSTR];			// impedance command string
  char disp_wave[4][SLEN];			// display wave flag
  int get_wave[4];				// record wave flag
  int got_wave[4];				// successful wave read flag
  char *outfile;				// output file name

  string xmlFile;

};

#endif
