/*
 *  This file contains the main method for the tekdaq program
 */

#include <iostream>
#include "tekdaq.h"

using namespace std;

#include "fileManager.h"
#include "ScopeParameters.h"
#include "Scope.h"


int main (int argc, char *argv[]){

  //initalize ScopeParameters object
  ScopeParameters *Params = new ScopeParameters(argc, argv);

  if(Params->getopt_Verbose&&!Params->keep) Params->printVerbose();

  //initalize scope object
  Scope tekScope(Params);

  if( !Params->TestMode ){
    
    tekScope.OpenScope();
    tekScope.InitScope();

  }

  tekScope.rewrite = 1;            // flag to write full report text

  if(!Params->TestMode)
    tekScope.AcquireWaves();

  tekScope.ShutDown();

  return 0;

}
