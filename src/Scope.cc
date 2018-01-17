#include <sstream>
#include <vector>
#include <string>

using namespace std;

#include "ScopeParameters.h"
#include "tekdaq.h"
#include "Scope.h"
#include "xmlParser.h"

/*
 *  The guts of these methods were written by Paul Poffenburger
 */


void Scope::OpenScope()
{
  /*
     Get the Default Resource Manager and connect to scope.
  */


  char statdsc[100];
  defaultRM = 0;
  scope = 0;

  // Open session to VISA Resource Manager.
  status = viOpenDefaultRM (&defaultRM);
  if ( status < VI_SUCCESS )
  {
    cout<<"Cannot open session to VISA Resource Manager"<<endl;
    viStatusDesc(defaultRM,status,statdsc);
    cout<<"Error on viOpenDefaultRM: "<<statdsc<<endl;
    exit (-1);
  }

  // Connect to scope.
  char address[] = SCOPE_ADDR;
  status = viOpen (defaultRM, (ViChar*)address, VI_NULL, VI_NULL, &scope);
  if ( status < VI_SUCCESS )
  {
    cout<<"Cannot open session to scope."<<endl;
    viStatusDesc(defaultRM,status,statdsc);
    cout<<"Error on viOpen: "<<statdsc<<endl;
    cout<<"Cycle the scope power and try again."<<endl;
    exit (-1);
  }

  // Set timeout value (milliseconds).
  status = viSetAttribute (scope, VI_ATTR_TMO_VALUE, TOUT);
  if ( status < VI_SUCCESS )
  {
    cout<<"Cannot set timeout value."<<endl;
    viStatusDesc(defaultRM,status,statdsc);
    cout<<"Error on viSetAttribute: "<<statdsc<<endl;;
    ShutDown ();
  }

  // Clear the scope.
  status = viClear (scope);
  if (status < VI_SUCCESS)
  {
    cout<<"Error clearing scope."<<endl;
    viStatusDesc(defaultRM,status,statdsc);
    cout<<"Error on viClear: %s \n"<<statdsc<<endl;;
    ShutDown ();
  }

  // Flush the read buffers
  status = viFlush (scope, VI_READ_BUF);
  if (status < VI_SUCCESS)
  {
    cout<<"Error flushing scope."<<endl;
    viStatusDesc(defaultRM,status,statdsc);
    cout<<"Error on viFlush: "<<statdsc<<endl;
    ShutDown ();
  }

  return;
}

void Scope::CloseScope()
{
  TekCmd ("ACQ:STATE OFF");	// acquire state set to OFF

  TekCmd ("LOCK NONE");		// unlock the front panel control

  if ( Parameters->TEK_Verbose )
  {
    cout<<"Closing down the scope..."<<endl;
  }

  //viClear (scope);		// this seems to sometimes leave the scope in a locked state...

  if ( scope )
  {
    viClose (scope);
  }
  if ( defaultRM )
  {
    viClose (defaultRM);
  }

  fMan.CloseFile();

  return;
}

void Scope::TekCmd(string cmd)
{
  /*
    Generic scope command writer.
  */

  if ( Parameters->TEK_Verbose )
  {
    cout<<"Command sent to scope: "<<cmd<<endl;
  }

  status = viWrite (scope, (ViBuf) cmd.c_str(), strlen (cmd.c_str()), &retCount);

  if (status < VI_SUCCESS)
  {
    viStatusDesc (scope, status, desc);
    cout<<"Error writing to scope: "<<cmd<<endl;
    cout<<"Status description: "<<desc<<endl;
    ShutDown ();
  }
  return;
}

void Scope::TekQry(string cmd, char rtn[SLEN])
{
  /*
    Generic scope query.
  */

  TekCmd (cmd);
  memset(rtn, 0, SLEN*sizeof(char));			// clear the read buffer
  status = viRead (scope, (ViBuf) rtn, SLEN, &retCount);
  if (status < VI_SUCCESS)
  {
    viStatusDesc (scope, status, desc);

    cout<<"Error writing to scope: "<<cmd<<endl;
    cout<<"Status description: "<<desc<<endl;
    ShutDown ();
  }
  else
  {
    rtn[retCount] = '\0';
    if ( Parameters->TEK_Verbose )
    {
      sscanf (rtn, "%[^\n]", rtn);
      cout<<"Query response read from scope: "<<rtn<<" ("<<(int) retCount<<" bytes).\n"<<endl;
    }
  }
  return;
}


int Scope::WavQry(string cmd, char rtn[CLEN])
{
  /*
    Read waveform from scope.
  */

  TekCmd (cmd);
  memset(rtn, 0, CLEN*sizeof(char));			// clear the read buffer
  status = viRead (scope, (ViBuf) rtn, CLEN, &retCount);
  if (status < VI_SUCCESS)
  {
    viStatusDesc (scope, status, desc);
    cout<<"Error writing to scope: "<<cmd<<endl;
    cout<<"Status description: "<<desc<<endl;
    ShutDown ();
  }
  else
  {
    if ( Parameters->TEK_Verbose )
    {
      cout<<"Waveform bytes read from scope: "<< (int) retCount<<endl;
    }
  }
  return (int) retCount;
}

void Scope::InitScope()
{
  /*
    Initialize the scope.
  */


  int ScopeRecLen;
  char buf[SLEN];

  TekCmd ("*CLS");				// clear some registers...
  TekQry ("ALLEV?", MyString);			// remove all events
  TekCmd ("ACQ:STATE OFF");			// acquire state set to OFF

  TekCmd ("HEAD ON");
  TekQry ("*IDN?", MyString);			// get the scope ID
  cout<<MyString<<endl;
  TekCmd ("HEAD OFF");

  if ( Parameters->lock )
  {
    TekCmd ("LOC All");				// lock out front panel control
  }

  if (Parameters->opt_N[0] || !Parameters->keep) TekCmd (Parameters->disp_wave[0]);		// display channel 1
  if (Parameters->opt_N[1] || !Parameters->keep) TekCmd (Parameters->disp_wave[1]);		// display channel 2
  if (Parameters->opt_N[2] || !Parameters->keep) TekCmd (Parameters->disp_wave[2]);		// display channel 3
  if (Parameters->opt_N[3] || !Parameters->keep) TekCmd (Parameters->disp_wave[3]);		// display channel 4

  if (Parameters->opt_1 || !Parameters->keep) TekCmd (Parameters->vscal[0]);		// vertical scale channel 1
  if (Parameters->opt_2 || !Parameters->keep) TekCmd (Parameters->vscal[1]);		// vertical scale channel 2
  if (Parameters->opt_3 || !Parameters->keep) TekCmd (Parameters->vscal[2]);		// vertical scale channel 3
  if (Parameters->opt_4 || !Parameters->keep) TekCmd (Parameters->vscal[3]);		// vertical scale channel 4

  if (Parameters->opt_5 || !Parameters->keep) TekCmd (Parameters->imped[0]);		// impedance channel 1
  if (Parameters->opt_6 || !Parameters->keep) TekCmd (Parameters->imped[1]);		// impedance channel 2
  if (Parameters->opt_7 || !Parameters->keep) TekCmd (Parameters->imped[2]);		// impedance channel 3
  if (Parameters->opt_8 || !Parameters->keep) TekCmd (Parameters->imped[3]);		// impedance channel 4

  TekCmd ("CH1:BAN FUL");				// bandwidth channel 1
  TekCmd ("CH2:BAN FUL");				// bandwidth channel 2
  TekCmd ("CH3:BAN FUL");				// bandwidth channel 3
  TekCmd ("CH4:BAN FUL");				// bandwidth channel 4

  if (Parameters->opt_P || !Parameters->keep) TekCmd (Parameters->coupl[0]);		// coupling channel 1
  if (Parameters->opt_Q || !Parameters->keep) TekCmd (Parameters->coupl[1]);		// coupling channel 2
  if (Parameters->opt_R || !Parameters->keep) TekCmd (Parameters->coupl[2]);		// coupling channel 3
  if (Parameters->opt_S || !Parameters->keep) TekCmd (Parameters->coupl[3]);		// coupling channel 4

  if (Parameters->opt_A || !Parameters->keep) TekCmd (Parameters->pos[0]);			// position channel 1
  if (Parameters->opt_B || !Parameters->keep) TekCmd (Parameters->pos[1]);			// position channel 2
  if (Parameters->opt_C || !Parameters->keep) TekCmd (Parameters->pos[2]);			// position channel 3
  if (Parameters->opt_D || !Parameters->keep) TekCmd (Parameters->pos[3]);			// position channel 4

#if defined DPO4104B || defined MDO3054
  TekCmd ("HOR:DEL:MODE OFF");				// horizontal delay mode off
#elif defined TDS3054B
  TekCmd ("HOR:DEL:STATE OFF");				// horizontal delay mode off
#endif
  if (Parameters->opt_p || !Parameters->keep) TekCmd (Parameters->htrpos);			// horizontal trigger position

  if (Parameters->opt_b || !Parameters->keep) TekCmd (Parameters->hsamp);			// sample rate

  if (Parameters->opt_l || !Parameters->keep) {
    sprintf (buf, "HOR:RECORDLENGTH %d", Parameters->RecLen);	// recordlength
    TekCmd (buf);
    TekCmd ("HEAD OFF");				// no header for the following query
    TekQry ("HORIZONTAL:RECORDLENGTH?", MyString);
    ScopeRecLen = atoi (MyString);			// actual waveform recordlenth recorded by scope
    if ( ScopeRecLen != Parameters->RecLen ) {
      cout<<"Warning!  Requested waveform recordlength "<<Parameters->RecLen<<",";
      cout<<"but scope is recording recordlength "<<ScopeRecLen<<endl;
    }
    if ( ScopeRecLen == 0 ) {
      ShutDown ();
    }
    TekCmd ("DATA:START 1");
    sprintf (buf, "DAT:STOP %d", ScopeRecLen);	// set the number of waveform points to transfer
    TekCmd (buf);
  }

  // if 'Parameters->keep' flag is set, need to make sure DATA:START and DAT:STOP are set to transfer the 
  // full waveform (already done above if 'opt_l' flag is set)
  if (!Parameters->opt_l && Parameters->keep) {
    TekQry ("HORIZONTAL:RECORDLENGTH?", MyString);
    ScopeRecLen = atoi (MyString);			// actual waveform recordlenth recorded by scope
    TekCmd ("DATA:START 1");
    sprintf (buf, "DAT:STOP %d", ScopeRecLen);		// set the number of waveform points to transfer
    TekCmd (buf);
  }

  TekCmd ("VERB ON");				// verbose mode
#if defined DPO4104B || defined MDO3054
  TekCmd ("DIS:XY OFF");			// display voltage vs time (YT)
#elif defined TDS3054B
  TekCmd ("DIS:XY:MODE OFF");			// display voltage vs time (YT)
#endif
  TekCmd ("DIS:PERS AUTO");			// display persistence time

  if (Parameters->opt_c || !Parameters->keep) TekCmd (Parameters->trsrc);		// trigger source

#if defined DPO4104B || defined MDO3054
  strcpy (trlevlcmd, "TRIG:A:LEV:");
  if ( strstr(Parameters->trsrc,"CH1") ) {
    strcat (trlevlcmd, "CH1 ");
  } else if ( strstr(Parameters->trsrc,"CH2") ) {
    strcat (trlevlcmd, "CH2 ");
  } else if ( strstr(Parameters->trsrc,"CH3") ) {
    strcat (trlevlcmd, "CH3 ");
  } else if ( strstr(Parameters->trsrc,"CH4") ) {
    strcat (trlevlcmd, "CH4 ");
#if defined DPO4104B
  } else if ( strstr(Parameters->trsrc,"AUX") ) {
    strcat (Parameters->trlevlcmd, "AUX ");
#endif
  }
#elif defined TDS3054B
  strcpy (trlevlcmd, "TRIG:A:LEV ");
#endif
  strcat (trlevlcmd, Parameters->trlevl);
  if (Parameters->opt_t || !Parameters->keep) TekCmd (trlevlcmd);	// trigger level

  if (Parameters->opt_s || !Parameters->keep) TekCmd (Parameters->trslop);		// trigger slope
  TekCmd ("TRIG:A:MOD NORM");			// trigger normal mode

  if (Parameters->opt_X) {

    TekCmd ("TRIG:A:TYPE LOGIC");
    TekCmd ("TRIG:A:LOGI:CLASS LOGIC");
    TekCmd ("TRIG:A:LOGI:PAT:WHE TRUE");

    if (Parameters->logicAND) {
      TekCmd ("TRIG:A:LOGI:FUNC AND");
    } else if (Parameters->logicOR) {
      TekCmd ("TRIG:A:LOGI:FUNC OR");
    }

    if (Parameters->logicCH1) {
      if (Parameters->logicLO) {
        TekCmd ("TRIG:A:LOGI:INP:CH1 LOW");
      } else if (Parameters->logicHI) {
        TekCmd ("TRIG:A:LOGI:INP:CH1 HIGH");
      }
      strcpy (logilevlcmd, "TRIG:A:LOGI:THR:CH1 ");
      strcat (logilevlcmd, Parameters->trlevl);
      TekCmd (logilevlcmd);
    } else {
      TekCmd ("TRIG:A:LOGI:INP:CH1 X");
    }

    if (Parameters->logicCH2) {
      if (Parameters->logicLO) {
        TekCmd ("TRIG:A:LOGI:INP:CH2 LOW");
      } else if (Parameters->logicHI) {
        TekCmd ("TRIG:A:LOGI:INP:CH2 HIGH");
      }
      strcpy (logilevlcmd, "TRIG:A:LOGI:THR:CH2 ");
      strcat (logilevlcmd, Parameters->trlevl);
      TekCmd (logilevlcmd);
    } else {
      TekCmd ("TRIG:A:LOGI:INP:CH2 X");
    }

    if (Parameters->logicCH3) {
      if (Parameters->logicLO) {
        TekCmd ("TRIG:A:LOGI:INP:CH3 LOW");
      } else if (Parameters->logicHI) {
        TekCmd ("TRIG:A:LOGI:INP:CH3 HIGH");
      }
      strcpy (logilevlcmd, "TRIG:A:LOGI:THR:CH3 ");
      strcat (logilevlcmd, Parameters->trlevl);
      TekCmd (logilevlcmd);
    } else {
      TekCmd ("TRIG:A:LOGI:INP:CH3 X");
    }

    if (Parameters->logicCH4) {
      if (Parameters->logicLO) {
        TekCmd ("TRIG:A:LOGI:INP:CH4 LOW");
      } else if (Parameters->logicHI) {
        TekCmd ("TRIG:A:LOGI:INP:CH4 HIGH");
      }
      strcpy (logilevlcmd, "TRIG:A:LOGI:THR:CH4 ");
      strcat (logilevlcmd, Parameters->trlevl);
      TekCmd (logilevlcmd);
    } else {
      TekCmd ("TRIG:A:LOGI:INP:CH4 X");
    }
  }	// end opt_X

  if (Parameters->opt_a || !Parameters->keep) {
    if ( Parameters->average <= 0 )
    {
      TekCmd ("ACQ:MOD SAM");			// acquisition mode sample
    } 
    else
    {
      sprintf (buf, "ACQ:NUMAV %d", Parameters->average);	// set the number of events to average
      TekCmd (buf);
      TekCmd ("ACQ:MOD AVE");			// acquisition mode average 
    }
  }

  TekCmd ("ACQ:STATE OFF");			// acquire state set to OFF

  TekCmd ("MEASU:IMM:TYPE AMP");		// amplitude measurement

  if ( Parameters->get_wave[0] || Parameters->get_wave[1] || Parameters->get_wave[2] || Parameters->get_wave[3] )
  {
    TekCmd ("DAT:ENC RPB");			// RPBinary data format
#if defined DPO4104B || defined MDO3054
    TekCmd ("WFMO:BYT_N 1");			// one byte per datum
#elif defined TDS3054B
    TekCmd ("DAT:WID 1");			// one byte per datum
#endif
  }

  if ( Parameters->TEK_Verbose )
  {
    cout<<"Finished initializing Tektronix TDS7404B scope..."<<endl;
  }
  


  MakeHeader();
  fMan.ParseMetaData(Header);


  return;
}


int Scope::AcquireData()
{
  /*
    Note that the AcquireData() routine adds a significant time overhead.
  */

  TekCmd ("HEAD OFF");

  TekCmd ("MEASU:IMMED:SOURCE CH1");
  TekQry ("MEASU:IMMED:VAL?", MyString);
  Parameters->CH1 = atof (MyString);
  if (Parameters->CH1 > 100) Parameters->CH1 = 0;

  TekCmd ("MEASU:IMMED:SOURCE CH2");
  TekQry ("MEASU:IMMED:VAL?", MyString);
  Parameters->CH2 = atof (MyString);
  if (Parameters->CH2 > 100) Parameters->CH2 = 0;

  TekCmd ("MEASU:IMMED:SOURCE CH3");
  TekQry ("MEASU:IMMED:VAL?", MyString);
  Parameters->CH3 = atof (MyString);
  if (Parameters->CH3 > 100) Parameters->CH3 = 0;

  TekCmd ("MEASU:IMMED:SOURCE CH4");
  TekQry ("MEASU:IMMED:VAL?", MyString);
  Parameters->CH4 = atof (MyString);
  if (Parameters->CH4 > 100) Parameters->CH4 = 0;

  TekCmd ("HEAD ON");

  return 0;
}

void Scope::AcquireWaves()
{
  /*
    The data in units of YUN is (d-YOF)*YMU + YZE, where d is a value returned by "CURV?".
    The sample rate is given by XIN.
  */

  int i, j, rc;
  char buf[SLEN];
  char chan[2];
  time_t t0, tdur;

  TekCmd ("ACQ:STOPA SEQ");			        // acquire mode single sequence

  /*
    The first trigger is used just to get waveform header information; the 
    first waveform IS NOT LOGGED.
  */

  TekCmd ("HEADER ON");
  TekCmd ("VERB ON");
  TekCmd ("ACQ:STATE ON");			        // turn on acquisition

  tdur = time (NULL);					// initialize run timer
  t0 = time (NULL);				        // initialize aq_timeout timer
  while ( 1 ) {
    if ( Parameters->aq_timeout >= 0 ) {			        // acquistion timeout must have been set...
      if ( time (NULL) - t0 >= Parameters->aq_timeout ) {
        cout<<"Acquisition Timeout!"<<endl;
        ShutDown ();
      }
    }

    if ( Parameters->duration >= 0 ) {
      if ( time (NULL) - tdur >= Parameters->duration ) break;	// break out of loop and quit
    }

    TekQry ("ACQ:STATE?", MyString);
    rc = strncmp (MyString, ":ACQUIRE:STATE 1", 16);
    if ( rc ) break;
    usleep (1000);
  }

  for (i=0; i<4; i++) {                               	// loop over channels
    strcpy (buf, "DAT:SOU CH");
    if (Parameters->get_wave[i])
    {
      sprintf (chan, "%d", i+1); 
      strcat (buf, chan);
    }
    else
    {
      continue;
    }
    TekCmd (buf);			                // data source

    Parameters->got_wave[i] = 0;					// waveforms are all lumped together...
    for (j=0; j<NTRIES; j++)
    {
#if defined DPO4104B || defined MDO3054
      ByteCount[0] = WavQry ("WFMOutpre?", Curve[0]);	// need to know how many bytes to write out...
#elif defined TDS3054B
      ByteCount[0] = WavQry ("WFMPRE?", Curve[0]);	// need to know how many bytes to write out...
#endif
      //cout<<Curve[0]<<endl;
      TekQry ("*ESR?", MyString);			// are we done yet??? (no time overhead...)
      rc = atoi (MyString);
      if ( rc == 0 )					// might need more exception handling here...
      {
        Parameters->got_wave[i] = 1;
        break;
      }
    }
    if ( ! Parameters->got_wave[i] )
    {
      cout<<"\nWarning! Couldn't read header channel "<<i+1<<endl;
      rewrite = 1;
    }
    else
    {
      //LogIt (fp, -1);	
      fMan.parseHeader(Curve[0],ByteCount[0]);				// log the waveform headers
    }
  }

  /* Start loop over triggers for waveforms to be logged */

  event = 0;						// initialize event counter

  TekCmd ("HEADER ON");					// this turns on the ":CURVE" header
  TekCmd ("VERB ON");					// ":CURVE" rather than ":CURV"...

  while ( 1 )
  {
    if ( event == Parameters->nevent )  {
      break;						// break out of loop and quit
    }

    if ( Parameters->duration >= 0 ) {
      if ( time (NULL) - tdur >= Parameters->duration ) break;	// break out of loop and quit
    }

    TekCmd ("ACQ:STATE ON");			        // turn on acquisition

    t0 = time (NULL);				        // initialize aq_timeout timer
    while ( 1 ) {
      if ( Parameters->aq_timeout >= 0 ) {			        // acquistion timeout must have been set...
        if ( time (NULL) - t0 >= Parameters->aq_timeout ) {
          cout<<"Acquisition Timeout!"<<endl;
          ShutDown ();
        }
      }
      TekQry ("ACQ:STATE?", MyString);
      rc = strncmp (MyString, ":ACQUIRE:STATE 1", 16);
      if ( rc ) break;
      usleep (1000);
    }

    if (Parameters->measureData) {
      AcquireData ();		        		// get the amplitude data
      if ( Parameters->CH1<Parameters->MinAmp[0] || Parameters->CH2<Parameters->MinAmp[1] ||
           Parameters->CH3<Parameters->MinAmp[2] || Parameters->CH4<Parameters->MinAmp[3] ) continue;	// exclude small signals
    }

    if ( event%Parameters->prescale ) {
      event++;
      continue;						// don't log the prescaled trigger
    }

    for (i=0; i<4; i++) {                               // loop over channels
      strcpy (buf, "DAT:SOU CH");
      if (Parameters->get_wave[i])
      {
        sprintf (chan, "%d", i+1); 
        strcat (buf, chan);
      }
      else
      {
        continue;
      }
      TekCmd (buf);			                // data source

      Parameters->got_wave[i] = 0;					// waveforms are all lumped together...
      for (j=0; j<NTRIES; j++)
      {
        ByteCount[i] = WavQry ("CURVE?", Curve[i]);	// need to know how many bytes to write out...
	TekQry ("*ESR?", MyString);			// are we done yet??? (no time overhead...)
        rc = atoi (MyString);
        if ( rc == 0 )					// might need more exception handling here...
        {
          Parameters->got_wave[i] = 1;
  	  break;
        }
      }
      if ( ! Parameters->got_wave[i] )
      {
        cout<<"\nWarning! Couldn't read waveform channel "<<i+1<<" for event "<<event+1<<endl;
        rewrite = 1;
      }
    }

    if (Parameters->WriteToFile) {
      //LogIt (fp, event);
      fMan.addEvent(Curve, ByteCount);
    }

    event++;

    if ( ! (event%Parameters->ReportFreq) )
    {
      Counter (event);
    }
    sleep (Parameters->interval);
  }

  RunComplete();


  return;
}


void Scope::ShutDown()
{
  /*
    Close the scope session.
  */

  if ( ! Parameters->TestMode )
  {
    CloseScope ();
  }
#if IPC==1
  kill (pid, SIGKILL);                          // kill the child
#endif
  exit (0);
}


void Scope::Counter(int n)
{
  /*
    Counter; n must be >= 0
  */

  static int d=0;
  int i;

  if ( rewrite )
  {
    cout<<"number of events so far = ";
    rewrite = 0;
  }

  for (i=0; i<d; i++)
  {
    cout<<"\b";
  }

  d = log10 (n);
  if ( d < 0 )
  {
    d = 1;
  }
  else
  {
    d = d + 1;
  }

  cout<<n<<endl;
  fflush (stdout);

  return;
}

void Scope::MakeHeader(){
  /*
    Build the data header.
  */

  Header="";

  TekCmd ("HEADER ON");

  BldHead ("CH1:SCALE?");
  BldHead ("CH2:SCALE?");
  BldHead ("CH3:SCALE?");
  BldHead ("CH4:SCALE?");
  BldHead ("CH1:BANDWIDTH?");
  BldHead ("CH2:BANDWIDTH?");
  BldHead ("CH3:BANDWIDTH?");
  BldHead ("CH4:BANDWIDTH?");
  BldHead ("CH1:COUPLING?");
  BldHead ("CH2:COUPLING?");
  BldHead ("CH3:COUPLING?");
  BldHead ("CH4:COUPLING?");
#if defined DPO4104B || defined MDO3054
  BldHead ("CH1:TERM?");
  BldHead ("CH2:TERM?");
  BldHead ("CH3:TERM?");
  BldHead ("CH4:TERM?");
#elif defined TDS3054B
  BldHead ("CH1:IMP?");
  BldHead ("CH2:IMP?");
  BldHead ("CH3:IMP?");
  BldHead ("CH4:IMP?");
#endif
  BldHead ("CH1:POSITION?");
  BldHead ("CH2:POSITION?");
  BldHead ("CH3:POSITION?");
  BldHead ("CH4:POSITION?");
  BldHead ("HORIZONTAL?");
  BldHead ("DISPLAY:XY?");
  BldHead ("TRIGGER:A:EDGE:SOURCE?");
  BldHead ("TRIGGER:A:EDGE:SLOPE?");
  BldHead ("TRIGGER:A:LEVEL?");
  BldHead ("TRIGGER:A:MODE?");
  BldHead ("ACQUIRE:MODE?");
  TekQry ("ACQUIRE:MODE?", MyString);
  if (strstr(MyString,"AVERAGE")) {
    BldHead ("ACQUIRE:NUMAVG?");
  }
  BldHead ("MEASU:IMM:TYPE?");

  

  TekCmd ("HEADER OFF");

  return;
}

void Scope::BldHead (string qry)
{
  int i;
  string StrHead=":";

  StrHead = StrHead+qry.substr(0,3);
  


  for (i=0; i<NTRIES; i++)
  {
    TekQry (qry, MyString);
    if ( ! strncmp (StrHead.c_str(), MyString, 4) )
    {
      sscanf (MyString, "%[^\n]", MyString);
      Header = Header+MyString+"\t";
      return;
    }
    else
    {
      sleep (1);
    }
  }

  cout<<"Error! Problem reading run header."<<endl;
  ShutDown ();

  return;
}


void Scope::RunComplete(){

  cout<<"\nFinished measuring "<<event<<" events!\n";


  /*
   *This sends a notification to the top right of the screen.
   *The version of libnotify available with SL6 is not compatible.
   */
#ifdef NOTIFY

  stringstream ss;
  ss<<event;
  string numEvents = ss.str();


  char *cd;
  cd = getenv("PWD");
  string pwd = cd;

  string subMessage = "\"Finished measuring "+numEvents+" events!\"";
  string icon = pwd+"/icon/tek.png";

  string command = "notify-send "+subMessage+" -i "+icon;
  system(command.c_str());

#endif


}



vector<string> split(const char *str, char c = ' ')
{
    vector<string> result;

    do
    {
        const char *begin = str;

        while(*str != c && *str)
            str++;

        result.push_back(string(begin, str));
    } while (0 != *str++);

    return result;
}


//generate an xml file of the scope settings for this run
void Scope::GenerateXml(string filename){

  stringstream ss;
  string retVal;
  XmlParser outSettings;
  string ch;
  

  for(int i=0; i<4; i++){
    ss<<i+1;
    ch=ss.str();
    ss.str("");
    
    //which channels are on
    ss<<Parameters->get_wave[i];  
    outSettings.addValue("ch"+ch, ss.str());
    ss.str("");
    
    if(Parameters->get_wave[i]){
      
      //scale for each channel
      TekQry("CH"+ch+":SCALE?", MyString);
      sscanf (MyString, "%[^\n]", MyString);    
      retVal=MyString;
      outSettings.addValue("vsca"+ch, retVal);
      
      //position of each channel
      TekQry("CH"+ch+":POSITION?", MyString);
      sscanf (MyString, "%[^\n]", MyString);    
      retVal=MyString; 
      outSettings.addValue("pos"+ch, retVal);
      
      //coupling of each channel
      TekQry("CH"+ch+":COUPLING?", MyString);
      sscanf (MyString, "%[^\n]", MyString);    
      retVal=MyString;
      outSettings.addValue("coupl"+ch, retVal);  
      
      //input impedance for each channel
#if defined DPO4104B || defined MDO3054
      TekQry("CH"+ch+":TERM?", MyString);
#elif defined TDS3054B
      TekQry("CH"+ch+":IMP?", MyString);
#endif
      sscanf (MyString, "%[^\n]", MyString);  
      retVal=MyString;
      outSettings.addValue("imped"+ch, retVal);  
      
      //minimum amplitude for each channel
      ss<<Parameters->MinAmp[i];
      outSettings.addValue("amp"+ch, ss.str());   

      ss.str("");


    }

  }

  //trigger source
  TekQry("TRIGGER:A:EDGE:SOURCE?", MyString);
  sscanf (MyString, "%[^\n]", MyString);  
  retVal=MyString;
  string trgsrc;
#if defined DPO4104B
  if(retVal.find("AUX") != std::string::npos)
    trgsrc=0;    
#elif defined TDS3054B    
  if(retVal.find("EXT10") != std::string::npos)
    trgsrc="10";
  else if(retVal.find("EXT10") != std::string::npos)
    trgsrc="0";
#endif
  if(retVal.find("CH") != std::string::npos) 
    trgsrc=retVal.substr(2);  
  outSettings.addValue("trsrc", trgsrc);
  
  //trigger source  
  TekQry("TRIGGER:A:EDGE:SLOPE?", MyString);
  sscanf (MyString, "%[^\n]", MyString);   
  retVal=MyString;
  outSettings.addValue("trslope", retVal);
  
  //trigger level
  TekQry("TRIGGER:A:LEVEL?", MyString);
  sscanf (MyString, "%[^\n]", MyString);   
  retVal=MyString;
  outSettings.addValue("trlevel", retVal);

  //pretrigger and horizontal sampling
  TekQry("HORIZONTAL?", MyString);
  sscanf (MyString, "%[^\n]", MyString);   
  retVal=MyString;
    
  char token=';';
  vector<string> s = split(retVal.c_str(), token);
  
  outSettings.addValue("pretrigger", s[1]);
  outSettings.addValue("hsamp", s[2]);
    
  //record length
  TekQry("HOR:RECORDLENGTH?", MyString);
  sscanf (MyString, "%[^\n]", MyString);   
  retVal=MyString;
  outSettings.addValue("length", retVal);




  outSettings.writeXml(filename);

}
