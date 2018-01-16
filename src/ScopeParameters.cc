#include <getopt.h>

using namespace std;

#include "ScopeParameters.h"




ScopeParameters::ScopeParameters(int argc, char *argv[]){
  /*
    Parse the command line options and arguments;
    should be the first call made from main.
  */

  int i, c, wavenum, trigsource;
  char chan[2];

  /*
	Program defaults ---
  */

  getopt_Verbose=0;				// getopt verbose flag
  quiet=0;					// quiet mode
  keep=0;					// use program defaults instead of existing scope settings
  TEK_Verbose=0;				// TEK scope verbose flag
  ipc_Verbose=0;				// signal verbose flag
  WriteToFile=1;				// write-to-file flag
  measureData=0;				// amplitude measurements
  passCuts=1;					// all cuts pass by default
  nevent=-1;					// maximum number events to measure
  prescale=1;					// number to prescale the logged events
  average=-1;                                   // number of events to average
  duration=-1;					// duration in seconds to acquire data
  interval=0;					// sleep time between data acquisitions
  aq_timeout=-1;				// aquistion timeout in seconds; negative => infinite
  lock=1;					// front panel lock flag
  TestMode=0;					// test mode flag
  ReportFreq=REPORT_FREQ;			// report frequency for number of events measured
  RecLen=RECLEN;				// waveform record length
  MinAmp[0] = -99;				// minimum amplitude to record event channel 1
  MinAmp[1] = -99;				// minimum amplitude to record event channel 2
  MinAmp[2] = -99;				// minimum amplitude to record event channel 3
  MinAmp[3] = -99;				// minimum amplitude to record event channel 4
  strcpy (hsamp, HSAMP);			// default sample rate
  strcpy (trsrc, TRSRC);			// default tirgger source
  strcpy (trlevl, TRLEV);			// default trigger level
  strcpy (trslop, SLOPE);			// default trigger edge slope
  strcpy (htrpos, HTRPOS);			// default pretrigger position
  CH1 = 0;					// channel 1 amplitude
  CH2 = 0;					// channel 2 amplitude
  CH3 = 0;					// channel 3 amplitude
  CH4 = 0;					// channel 4 amplitude
  logicCH1 = 0;					// logic trigger requires ch 1
  logicCH2 = 0;					// logic trigger requires ch 2
  logicCH3 = 0;					// logic trigger requires ch 3
  logicCH4 = 0;					// logic trigger requires ch 4
  logicAND = 0;					// logic condition is AND
  logicOR = 0;					// logic condition is OR
  logicLO = 0;					// logic input is LOW
  logicHI = 0;					// logic input is HIGH

  for (i=0; i<4; i++) 
    opt_N[i] = 0;
  
  opt_l = 0;
  opt_a = 0;
  opt_c = 0;
  opt_A = 0;
  opt_B = 0;
  opt_C = 0;
  opt_D = 0;
  opt_1 = 0;
  opt_2 = 0;
  opt_3 = 0;
  opt_4 = 0;
  opt_5 = 0;
  opt_6 = 0;
  opt_7 = 0;
  opt_8 = 0;
  opt_P = 0;
  opt_Q = 0;
  opt_R = 0;
  opt_S = 0;
  opt_t = 0;
  opt_s = 0;
  opt_p = 0;
  opt_b = 0;
  opt_X = 0;

  for (i=0; i<4; i++)  {
    sprintf (chan, "%d", i+1);

    strcpy (pos[i], "CH");			// vertical position (offset)
    strcat (pos[i], chan);
    strcat (pos[i], ":POS ");
    strcat (pos[i], POS);

    strcpy (vscal[i], "CH");			// vertical scale
    strcat (vscal[i], chan);
    strcat (vscal[i], ":SCA ");
    strcat (vscal[i], SCALE);

    strcpy (coupl[i], "CH");			// coupling
    strcat (coupl[i], chan);
    strcat (coupl[i], ":COUPL ");
    strcat (coupl[i], COUPL);

    strcpy (imped[i], "CH");			// impedance
    strcat (imped[i], chan);
#if defined DPO4104B || defined MDO3054
    strcat (imped[i], ":TERM ");
#elif defined TDS3054B
    strcat (imped[i], ":IMP ");
#endif
    strcat (imped[i], IMPED);

    strcpy (disp_wave[i], "SEL:CH");		// display wave (channel) flag
    strcat (disp_wave[i], chan);
    strcat (disp_wave[i], " ON");

    get_wave[i]=0;				// record wave flag
  }

  outfile = (char*)calloc (sizeof (OUTFILE), sizeof (char));
  strcpy (outfile, OUTFILE);
  strcpy (outfile, "tek.root");			// default output file name

  /*
	--- end of program defaults
  */

  while ( 1 ) {

    static struct option long_options[] = {
      {"help",		no_argument,       0, 'h'},	// help
      {"quiet",		no_argument,       0, 'q'},	// quiet mode
      {"defaults",	no_argument,	   0, 'Z'},	// defaults
      {"keep",		no_argument,	   0, 'k'},	// keep existing scope settings	
      {"verbose",	optional_argument, 0, 'v'},	// verbose mode
      {"report",	required_argument, 0, 'r'},	// report frequency
      {"output",	required_argument, 0, 'o'},	// output file name
      {"nodisp",	required_argument, 0, 'N'},	// inhibit display wave
      {"wave",		required_argument, 0, 'w'},	// record waveform
      {"length",	required_argument, 0, 'l'},	// waveform record length
      {"logic",		required_argument, 0, 'X'},	// logic trigger mode
      {"measure",	no_argument,       0, 'm'},	// enable amplitude measurements by scope
      {"nevent",	required_argument, 0, 'n'},	// maximum number events to measure
      {"prescale",	required_argument, 0, 'e'},	// number to prescale the logged events
      {"average",	required_argument, 0, 'a'},	// number events to average; sets mode to average...
      {"duration",	required_argument, 0, 'd'},	// maximum number of seconds to acquire data
      {"interval",	required_argument, 0, 'i'},	// sleep time between data acquisitions
      {"timeout",	required_argument, 0, 'K'},	// aquistion timeout in seconds
      {"nolock",	no_argument,	   0, 'L'},	// don't lock the scope front panel
      {"trsrc",		required_argument, 0, 'c'},	// trigger source
      {"trlevel",	required_argument, 0, 't'},	// trigger level
      {"trslope",	required_argument, 0, 's'},	// trigger slope
      {"amp1",		required_argument, 0, 'E'},	// minimum amplitude to record event channel 1
      {"amp2",		required_argument, 0, 'F'},	// minimum amplitude to record event channel 2
      {"amp3",		required_argument, 0, 'G'},	// minimum amplitude to record event channel 3
      {"amp4",		required_argument, 0, 'H'},	// minimum amplitude to record event channel 4
      {"pretrigger",	required_argument, 0, 'p'},	// pretrigger (%)
      {"pos1",		required_argument, 0, 'A'},	// position channel 1
      {"pos2",		required_argument, 0, 'B'},	// position channel 2
      {"pos3",		required_argument, 0, 'C'},	// position channel 3
      {"pos4",		required_argument, 0, 'D'},	// position channel 4
      {"vsca1",		required_argument, 0, '1'},	// vertical scale channel 1
      {"vsca2",		required_argument, 0, '2'},	// vertical scale channel 2
      {"vsca3",		required_argument, 0, '3'},	// vertical scale channel 3
      {"vsca4",		required_argument, 0, '4'},	// vertical scale channel 4
      {"coupl1",	required_argument, 0, 'P'},	// coupling channel 1
      {"coupl2",	required_argument, 0, 'Q'},	// coupling channel 2
      {"coupl3",	required_argument, 0, 'R'},	// coupling channel 3
      {"coupl4",	required_argument, 0, 'S'},	// coupling channel 4
      {"imped1",	required_argument, 0, '5'},	// impedance channel 1
      {"imped2",	required_argument, 0, '6'},	// impedance channel 2
      {"imped3",	required_argument, 0, '7'},	// impedance channel 3
      {"imped4",	required_argument, 0, '8'},	// impedance channel 4
      {"hsamp",		required_argument, 0, 'b'},	// sample rate
      {"tmode",     	required_argument, 0, 'T'},	// test mode
      {0, 0, 0, 0}
    };

    int option_index = 0;
    // leave case out of the list if no short option is to be allowed
    c = getopt_long (argc, argv, "hqZkv:r:o:N:w:l:X:m:n:e:a:d:i:c:t:s:p:b:T:", long_options, &option_index);
    
    if (c == -1) break;					// end of the options
    
    

    switch (c) {
    case 0:
      if (long_options[option_index].flag != 0) break;
      printf ("option %s", long_options[option_index].name);
      if (optarg) printf (" with arg %s", optarg);
      printf ("\n");
      break;
    case 'h':
      usage (argv[0]);
      break;
    case 'q':
      quiet = 1;
      break;
    case 'Z':
      Defaults ();
      break;
    case 'k':
      keep = 1;
      break;
    case 'v':
      if ( optarg == NULL || ! strcmp (optarg, "all" ) )        {
	getopt_Verbose = 1;
	TEK_Verbose = 1;
	ipc_Verbose=1;
      }
      else if ( ! strcmp (optarg, "opt" ) ) getopt_Verbose = 1;
      else if ( ! strcmp (optarg, "tek" ) ) TEK_Verbose = 1;
      else if ( ! strcmp (optarg, "sig" ) ) ipc_Verbose = 1;
      else        {
	printf ("Unrecognized verbose argument: %s\n", optarg);
	usage (argv[0]);
      }
      break;
    case 'r':
      ReportFreq = atoi (optarg);
      break;
    case 'o':
      if ( ! strcmp (optarg, "none") ) WriteToFile = 0;
      else        {
	outfile = (char*)calloc (strlen(optarg)+1,sizeof (char));
	strcpy (outfile, optarg);
      }
      break;
    case 'N':
      wavenum = atoi (optarg);
      if ( wavenum >= 1 && wavenum <= 4 )        {
	strcpy (disp_wave[wavenum-1], "SEL:CH");
	strcat (disp_wave[wavenum-1], optarg);
	strcat (disp_wave[wavenum-1], " OFF");
	opt_N[wavenum-1] = 1;
      }
      else        {
	printf ("Unrecognized inhibit display wave argument: %s\n", optarg);
	usage (argv[0]);
      }
      break;
    case 'w':
      wavenum = atoi (optarg);
      if ( wavenum >= 1 && wavenum <= 4 ) get_wave[wavenum-1] = 1;
      else if ( ! strcmp (optarg, "a" ))
	for (i=0;i<4;i++)        
	  get_wave[i] = 1;
        
      
      else {
	printf ("Unrecognized get wave argument: %s\n", optarg);
	usage (argv[0]);
      }
      break;
    case 'l':
      RecLen = (int) atof (optarg);
      opt_l = 1;
      if ( RecLen <= 0 || RecLen > 20000000 ) {
	printf ("Invalid waveform recordlength argument: %s\n", optarg);
	usage (argv[0]);
      }
      break;
    case 'X':
      if (index(optarg,'1')) logicCH1 = 1;
      if (index(optarg,'2')) logicCH2 = 1;
      if (index(optarg,'3')) logicCH3 = 1;
      if (index(optarg,'4')) logicCH4 = 1;
      if (strstr(optarg,"AND")) logicAND = 1;
      if (strstr(optarg,"OR")) logicOR = 1;
      if (strstr(optarg,"LO")) logicLO = 1;
      if (strstr(optarg,"HI")) logicHI = 1;
      opt_X = 1;
      break;
    case 'm':
      measureData = atoi (optarg);
      passCuts = 0;
      break;
    case 'n':
      nevent = atoi (optarg);
      // note that atoi will return '0' if no integer conversion can be made
      break;
    case 'e':
      prescale = atoi (optarg);
      // note that atoi will return '0' if no integer conversion can be made
      break;
    case 'a':
      average = atoi (optarg);
      opt_a = 1;
      // note that atoi will return '0' if no integer conversion can be made
      break;
    case 'd':
      duration = atoi (optarg);
      // note that atoi will return '0' if no integer conversion can be made
      break;
    case 'i':
      interval = atoi (optarg);
      // note that atoi will return '0' if no integer conversion can be made
      break;
    case 'K':
      aq_timeout = atoi (optarg);
      // note that atoi will return '0' if no integer conversion can be made
      break;
    case 'L':
      lock = 0;
      break;
    case 'c':
      trigsource = atoi (optarg);
      opt_c = 1;
      if ( trigsource>0 && trigsource<10 )  {
	strcpy (trsrc, "TRIG:A:EDGE:SOU CH");	// trigger source channel 1, 2, 3, or 4
	strcat (trsrc, optarg);
      }
#if defined DPO4104B
      else
	if ( trigsource==0 ) 
	  strcpy (trsrc, "TRIG:A:EDGE:SOU AUX");	// aux trigger source
      
      
#elif defined TDS3054B
        else
          if ( trigsource==0 ) 
            strcpy (trsrc, "TRIG:A:EDGE:SOU EXT");	// ext trigger source
	  else if ( trigsource==10 ) 
            strcpy (trsrc, "TRIG:A:EDGE:SOU EXT10");	// ext10 trigger source
      
      
#endif
      break;
    case 'A':
      strcpy (pos[0], "CH1:POS ");
      strcat (pos[0], optarg);
      opt_A = 1;
      break;
    case 'B':
      strcpy (pos[1], "CH2:POS ");
      strcat (pos[1], optarg);
      opt_B = 1;
      break;
    case 'C':
      strcpy (pos[2], "CH3:POS ");
      strcat (pos[2], optarg);
      opt_C = 1;
      break;
    case 'D':
      strcpy (pos[3], "CH4:POS ");
      strcat (pos[3], optarg);
      opt_D = 1;
      break;
    case '1':
      strcpy (vscal[0], "CH1:SCA ");
      strcat (vscal[0], optarg);
      opt_1 = 1;
      break;
    case '2':
      strcpy (vscal[1], "CH2:SCA ");
      strcat (vscal[1], optarg);
      opt_2 = 1;
      break;
    case '3':
      strcpy (vscal[2], "CH3:SCA ");
      strcat (vscal[2], optarg);
      opt_3 = 1;
      break;
    case '4':
      strcpy (vscal[3], "CH4:SCA ");
      strcat (vscal[3], optarg);
      opt_4 = 1;
      break;
    case '5':
#if defined DPO4104B || defined MDO3054
      strcpy (imped[0], "CH1:TERM ");
#elif defined TDS3054B
      strcpy (imped[0], "CH1:IMP ");
#endif
    
      if(
#if defined DPO4104B || defined TDS3054B
	 strncmp(optarg,"FIF",3) && strncmp(optarg,"MEG",3)
#elif defined MDO3054
      strncmp(optarg,"FIF",3) && strncmp(optarg,"75",2) && strncmp(optarg,"MEG",3)
#endif
	 ){
	printf ("Error! Invalid impedance: %s\n", optarg);
	usage (argv[0]);
      }
      strcat (imped[0], optarg);
      opt_5 = 1;
      break;
    case '6':
#if defined DPO4104B || defined MDO3054
        strcpy (imped[1], "CH2:TERM ");
#elif defined TDS3054B
        strcpy (imped[1], "CH2:IMP ");
#endif
	if(
#if defined DPO4104B || defined TDS3054B
	   strncmp(optarg,"FIF",3) && strncmp(optarg,"MEG",3)
#elif defined MDO3054
	   strncmp(optarg,"FIF",3) && strncmp(optarg,"75",2) && strncmp(optarg,"MEG",3)
#endif
	   ){
	printf ("Error! Invalid impedance: %s\n", optarg);
	usage (argv[0]);
      }
      strcat (imped[1], optarg);
      opt_6 = 1;
      break;
    case '7':
#if defined DPO4104B || defined MDO3054
      strcpy (imped[2], "CH3:TERM ");
#elif defined TDS3054B
      strcpy (imped[2], "CH3:IMP ");
#endif
      if(
#if defined DPO4104B || defined TDS3054B
	 strncmp(optarg,"FIF",3) && strncmp(optarg,"MEG",3) 
#elif defined MDO3054
	 strncmp(optarg,"FIF",3) && strncmp(optarg,"75",2) && strncmp(optarg,"MEG",3) 
#endif
	 ){
	printf ("Error! Invalid impedance: %s\n", optarg);
	usage (argv[0]);
      }
      strcat (imped[2], optarg);
      opt_7 = 1;
      break;
    case '8':
#if defined DPO4104B || defined MDO3054
      strcpy (imped[3], "CH4:TERM ");
#elif defined TDS3054B
      strcpy (imped[3], "CH4:IMP ");
#endif
#if defined DPO4104B || defined TDS3054B
      if ( strncmp(optarg,"FIF",3) && strncmp(optarg,"MEG",3) ) {
#elif defined MDO3054
	if ( strncmp(optarg,"FIF",3) && strncmp(optarg,"75",2) && strncmp(optarg,"MEG",3) ) {
#endif
	  printf ("Error! Invalid impedance: %s\n", optarg);
	  usage (argv[0]);
	}
	strcat (imped[3], optarg);
	opt_8 = 1;
	break;
      
    case 'P':
      strcpy (coupl[0], "CH1:COUPL ");
      strcat (coupl[0], optarg);
      opt_P = 1;
      if ( (strncmp (optarg, "AC", 2)) && (strncmp (optarg, "DC", 2)) )
	{
	  printf ("Unrecognized coupling argument: %s\n", optarg);
	  usage (argv[0]);
	}
      break;
      case 'Q':
	strcpy (coupl[1], "CH2:COUPL ");
	strcat (coupl[1], optarg);
        opt_Q = 1;
        if ( (strncmp (optarg, "AC", 2)) && (strncmp (optarg, "DC", 2)) )
	  {
	    printf ("Unrecognized coupling argument: %s\n", optarg);
	    usage (argv[0]);
	  }
        break;
      case 'R':
        strcpy (coupl[2], "CH3:COUPL ");
        strcat (coupl[2], optarg);
        opt_R = 1;
        if ( (strncmp (optarg, "AC", 2)) && (strncmp (optarg, "DC", 2)) )
	  {
	    printf ("Unrecognized coupling argument: %s\n", optarg);
	    usage (argv[0]);
	  }
        break;
      case 'S':
        strcpy (coupl[3], "CH4:COUPL ");
        strcat (coupl[3], optarg);
        opt_S = 1;
        if ( (strncmp (optarg, "AC", 2)) && (strncmp (optarg, "DC", 2)) )
        {
          printf ("Unrecognized coupling argument: %s\n", optarg);
          usage (argv[0]);
        }
        break;
      case 't':
        strcpy (trlevl, optarg);
        opt_t = 1;
        break;
      case 's':
        strcpy (trslop, "TRIG:A:EDGE:SLO ");
        strcat (trslop, optarg);
        opt_s = 1;
        if ( (strncmp (optarg, "FALL", 4)) && (strncmp (optarg, "RISE", 4)) )
	  {
	    printf ("Unrecognized trigger slope argument: %s\n", optarg);
	    usage (argv[0]);
	  }
        break;
      case 'E':
        MinAmp[0] = atof (optarg);
        break;
      case 'F':
        MinAmp[1] = atof (optarg);
        break;
      case 'G':
        MinAmp[2] = atof (optarg);
        break;
      case 'H':
        MinAmp[3] = atof (optarg);
        break;
      case 'p':
#if defined DPO4104B || defined MDO3054
        strcpy (htrpos, "HOR:POS ");
#elif defined TDS3054B
        strcpy (htrpos, "HOR:TRIG:POS ");
#endif
        strcat (htrpos, optarg);
        opt_p = 1;
        break;
      case 'b':
        strcpy (hsamp, "HOR:SCA ");
        strcat (hsamp, optarg);
        opt_b = 1;
        break;
      case 'T':
        TestMode = atoi (optarg);
        break;
      case '?':
        if (isprint (optopt))
        {
	  printf ("Unknown option `-%c'.\n", optopt);
	  usage (argv[0]);
        }
        else
        {
	  printf ("Unknown option character `\\x%x'.\n", optopt);
	  usage (argv[0]);
        }
        break;
      default:
        usage (argv[0]);
    }
  }

  if (argv[optind] == NULL)
  {
    // no more arguments left to parse
  }
  else
  {
    printf ("Unknown argument: %s\n", argv[optind]);
    usage (argv[0]);
  }

#if defined DPO4104B || MDO3054
  for (i=0; i<4; i++)
  {
    if ( (strstr(coupl[i],"AC")) && (strstr(imped[i],"FIF")) )
    {
      printf ("\nERROR! AC coupling and 50 Ohm impedance are incompatible!\n\n");
      usage (argv[0]);
    }
  }
#endif

  if (quiet)
  {
    getopt_Verbose=0;
    TEK_Verbose=0;
    ipc_Verbose=0;
  }
  

}

  
void ScopeParameters::usage (char *prog)
{
  /*
    Print out a usage message and then exit.
  */

  char *bname;

  bname = basename (prog);

  printf ("Usage: %s [OPTION]...\n\n", bname);
  printf ("DAQ program for the tektronix scope.\n\n");
  printf ("Pressing any key terminates data acquistion and closes the program.\n\n");
  printf ("Mandatory arguments to long options are mandatory for short options too.\n\n");
  printf ("-h,\t--help\t\t\tPrint this message and then exit.\n\n");
  printf ("-q,\t--quiet\t\t\tQuiet mode.\n\n");
  printf ("-Z,\t--defaults\t\tPrint default scope settings and then exit.\n\n");
  printf ("-k,\t--keep\t\t\tKeep existing scope settings not explicitly overridden by command line options.\n\n");
  printf ("-r,\t--report=FREQ\t\tSpecify report frequency for number of measurements.\n\n");
  printf ("-v,\t--verbose=MODE\t\tVerbose output; MODE may be:\n\n");
  printf ("\t\t\t\t\t'opt'\tfor getopt command line parsing\n");
  printf ("\t\t\t\t\t'tek'\tfor Tektronix scope commands\n");
  printf ("\t\t\t\t\t'sig'\tfor interprocess signal commands\n");
  printf ("\t\t\t\t\t'all'\tfor all verbose\n\n");
  printf ("-o,\t--output=FILE\t\tSpecify output data file; default is \"tek.dat\";\n");
  printf ("\t\t\t\tif FILE is \"none\", then no output data file is written.\n\n");
  printf ("-w,\t--wave=CH\t\tRecord waveform data for channel CH; specify 'a' for all channels.\n\n");
  printf ("-l,\t--length=LENGTH\t\tSpecify the waveform recordlength; not independent of the time base...\n");
#if defined DPO4104B
  printf ("\t\t\t\tAllowed values are: 1.E3, 1.E4, 1.E5, 1.E6, 5.E6, 1.E7, and 2.E7\n\n");
#elif defined MDO3054
  printf ("\t\t\t\tAllowed values are: 1.E3, 1.E4, 1.E5, 1.E6, 5.E6, and 1.E7\n\n");
#elif defined TDS3054B
  printf ("\t\t\t\tAllowed values are: 5.E2 and 1.E4\n\n");
#endif
  printf ("\t--logic=PATTERN\t\tLogic trigger.\n");
  printf ("\t\t\t\tLogic pattern 3AND4LO would require channels 3 AND 4 LOw (eg, NIM) for trigger.\n\n");
  printf ("-m,\t--measure\t\tEnable (1) or disable (0) amplitude measurements by scope.\n\n");
  printf ("-n,\t--nevent=NUM\t\tSpecify Number of events to measure; default is unlimited.\n\n");
  printf ("-a,\t--average=NUM\t\tSpecify number of triggers to Average for each event.\n");
  printf ("\t\t\t\tNote that if a prescale value is set, nevent will be the number\n");
  printf ("\t\t\t\tof triggers, not the number of events written to file.\n\n");
  printf ("-e,\t--prescale=NUM\t\tSpecify number to prescale logged events.\n\n");
  printf ("-d,\t--duration=SEC\t\tSpecify duration in seconds to acquire data; default is unlimited.\n\n");
  printf ("-i,\t--interval=SEC\t\tSpecify interval in seconds to sleep between acquisitions; default is zero.\n\n");
  printf ("\t--timeout=SEC\t\tSpecify the acquisition timeout period in seconds; default is unlimited.\n\n");
  printf ("-N,\t--nodisplay=CH\t\tInhibit display for wave (channel) CH.\n\n");
  printf ("\t--nolock\t\tDon't lock out the front panel controls.\n\n");
#if defined DPO4104B
  printf ("-c,\t--trsrc=CH\t\tSpecify the trigger channel; specify '0' for AUX IN.\n\n");
#elif defined MDO3054
  printf ("-c,\t--trsrc=CH\t\tSpecify the trigger channel.\n\n");
#elif defined TDS3054B
  printf ("-c,\t--trsrc=CH\t\tSpecify the trigger channel; specify '0' for 'EXT', '10' for 'EXT10'.\n\n");
#endif
  printf ("-t,\t--trlevel=TRIG_LEVEL\tSpecify trigger level (in volts).\n");
  printf ("\t\t\t\tExamples: 1.E+0, 1.E-2, -2.3E-3\n\n");
  printf ("-s,\t--trslope=TRIG_SLOPE\tSpecify the trigger edge slope - FALL or RISE.\n\n");
  printf ("\t--amp<N>=AMPLITUDE\tSpecify the minimum channel <N> signal amplitude (in volts) to record event.\n\n");
  printf ("\t--pos<N>=POSITION\tSpecify vertical position (in divisions) for channel <N>.\n\n");
  printf ("\t--vsca<N>=VSCALE\tSpecify vertical scale (in volts) for channel <N>.\n\n");
  printf ("\t--coupl<N>=COUPL\tSpecify coupling for channel <N>, 'AC' or 'DC'; default is 'DC'.\n\n");
#if defined DPO4104B || defined TDS3054B
  printf ("\t--imped<N>=IMPED\tSpecify impedance for channel <N>, 'FIF' or 'MEG'; default is 'MEG'.\n\n");
#elif defined MDO3054
  printf ("\t--imped<N>=IMPED\tSpecify impedance for channel <N>, 'FIF', '75', or 'MEG'; default is 'MEG'.\n\n");
#endif
  printf ("-b,\t--hsamp=HSAMP\t\tSpecify the horizontal scale (in seconds);\n");
  printf ("\t\t\t\tnote that this can effect the sample rate.\n\n");
  printf ("-p,\t--pretrigger=PRE_TRIG\tSpecify the amount of pretrigger (percent).\n\n");
  printf ("-T,\t--tmode=NUM\t\tTest mode; tmode = 1 bybasses all scope commands\n\n");

  exit (-1);
}

 void ScopeParameters::Defaults ()
{
  printf ("\n\t\t\t\tDefault Scope Settings\n\n");
  printf ("vertical scale channel 1:\t\t\t\t%s\n", SCALE);
  printf ("vertical scale channel 2:\t\t\t\t%s\n", SCALE);
  printf ("vertical scale channel 3:\t\t\t\t%s\n", SCALE);
  printf ("vertical scale channel 4:\t\t\t\t%s\n", SCALE);
  printf ("sample rate:\t\t\t\t\t\t%s\n", HSAMP);
  printf ("trigger edge slope:\t\t\t\t\t%s\n", SLOPE);
  printf ("pretrigger amount (percent):\t\t\t\t%s\n", HTRPOS);
  printf ("trigger level:\t\t\t\t\t\t%s\n", TRLEV);
  printf ("position channel 1:\t\t\t\t\t%s\n", POS);
  printf ("position channel 2:\t\t\t\t\t%s\n", POS);
  printf ("position channel 3:\t\t\t\t\t%s\n", POS);
  printf ("position channel 4:\t\t\t\t\t%s\n", POS);
  printf ("waveform record length:\t\t\t\t\t%d\n", RECLEN);
  printf ("minimum signal amplitude channel 1 to record event:\t%f\n", MinAmp[0]);
  printf ("minimum signal amplitude channel 2 to record event:\t%f\n", MinAmp[1]);
  printf ("minimum signal amplitude channel 3 to record event:\t%f\n", MinAmp[2]);
  printf ("minimum signal amplitude channel 4 to record event:\t%f\n", MinAmp[3]);
  printf ("\n");

  exit (0);
}
 
 void ScopeParameters::printVerbose(){
   printf ("Output file = \t\t\t%s\n", outfile);
    printf ("Report frequency = \t\t%d\n", ReportFreq);
    printf ("WriteToFile flag = \t\t%d\n", WriteToFile);
    printf ("Record channel 1 waveform = \t%d\n", get_wave[0]);
    printf ("Record channel 2 waveform = \t%d\n", get_wave[1]);
    printf ("Record channel 3 waveform = \t%d\n", get_wave[2]);
    printf ("Record channel 4 waveform = \t%d\n", get_wave[3]);
    printf ("Measure flag = \t\t\t%d\n", measureData);
    printf ("Number of events to measure = \t%d\n", nevent);
    printf ("Number of seconds to acquire data = \t\t%d\n", duration);
    printf ("Number of seconds between data acquisitions = \t%d\n", interval);
    printf ("Scope lock flag = \t\t%d\n", lock);
    printf ("Trigger channel = \t\t%s\n",trsrc);
    printf ("Trigger level = \t\t%s\n", trlevl);
    printf ("Trigger slope = \t\t%s\n", trslop);
    printf ("Pretrigger amount = \t\t%s\n", htrpos);
    printf ("CH1 position = \t\t\t%s\n", pos[0]);
    printf ("CH2 position = \t\t\t%s\n", pos[1]);
    printf ("CH3 position = \t\t\t%s\n", pos[2]);
    printf ("CH4 position = \t\t\t%s\n", pos[3]);
    printf ("CH1 vertical scale = \t\t%s\n", vscal[0]);
    printf ("CH2 vertical scale = \t\t%s\n", vscal[1]);
    printf ("CH3 vertical scale = \t\t%s\n", vscal[2]);
    printf ("CH4 vertical scale = \t\t%s\n", vscal[3]);
    printf ("CH1 coupling = \t\t%s\n", coupl[0]);
    printf ("CH2 coupling = \t\t%s\n", coupl[1]);
    printf ("CH3 coupling = \t\t%s\n", coupl[2]);
    printf ("CH4 coupling = \t\t%s\n", coupl[3]);
    printf ("CH1 impedance = \t\t%s\n", imped[0]);
    printf ("CH2 impedance = \t\t%s\n", imped[1]);
    printf ("CH3 impedance = \t\t%s\n", imped[2]);
    printf ("CH4 impedance = \t\t%s\n", imped[3]);
    printf ("CH1 minimum amplitude = \t%f\n", MinAmp[0]);
    printf ("CH2 minimum amplitude = \t%f\n", MinAmp[1]);
    printf ("CH3 minimum amplitude = \t%f\n", MinAmp[2]);
    printf ("CH4 minimum amplitude = \t%f\n", MinAmp[3]);
    printf ("Horizontal Sample Rate = \t%s\n", hsamp);

 }

