/*
 *  This file contains the main method for the tekdaq program
 */

#include <iostream>
#include "tekdaq.h"

using namespace std;

#include "fileManager.h"
#include "ScopeParameters.h"
#include "Scope.h"
#include "xmlParser.h"


volatile sig_atomic_t flag=0;

void my_function(int sig){ // can be called asynchronously
  flag = 1; // set flag
}



int main (int argc, char *argv[]){
  
  
  signal(SIGINT, my_function); 

  //flag=0;

  //initalize ScopeParameters object
  ScopeParameters *Params = new ScopeParameters(argc, argv);

  if(Params->getopt_Verbose&&!Params->keep) Params->printVerbose();

  //initalize scope object
  Scope tekScope(Params);

  if( !Params->TestMode ){
    
    tekScope.OpenScope();
    tekScope.InitScope();

  }

  //write settings to xml file
  if(Params->writeXml)
    tekScope.GenerateXml(Params->xmlOutFile);

  tekScope.rewrite = 1;            // flag to write full report text

  if(!Params->TestMode)
    tekScope.AcquireWaves();

  tekScope.ShutDown();

  return 0;

}
