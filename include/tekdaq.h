/*
 *  This header defines some compiler variables
 */

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <sched.h>
#include <errno.h>
#include <time.h>
#include <ctype.h>
#include <signal.h>
#include <termios.h>
#include <libgen.h>
#include <sys/types.h>

#include <sys/io.h>
#include <sys/perm.h>


#include <sys/stat.h>
#include <sys/time.h>



#if defined DPO4104B || defined MDO3054
#define RECLEN 1000				// default waveform record length; actual curve data
#define MXPTS 20000000				// maximum number of waveform points
#elif defined TDS3054B
#define RECLEN 500				// default waveform record length; actual curve data
#define MXPTS 10000				// maximum number of waveform points
#endif
#define CLEN MXPTS+1000				// curve length; includes header plus extra...
#define HLEN 4096				// data header length
#define SLEN 500				// generic string length
#define TOUT 100000				// tek scope timeout
#define NTRIES 2				// number of tries to read data or waveforms from scope
#define TEKSTR 50				// maximum length of string commands to tek scope
#define REPORT_FREQ 100				// report frequency for number of events measured

#if defined DPO4104B
#define SCOPE_ADDR "TCPIP::atl-dpo4104b::INSTR"		// IP address for DPO4104B
#elif defined MDO3054
//#define SCOPE_ADDR "TCPIP::atl-mdo3054::INSTR"		// IP address for MDO3054
#define SCOPE_ADDR "TCPIP::169.254.5.212::INSTR"		// IP address for MDO3054
#elif defined TDS3054B
//#define SCOPE_ADDR "TCPIP::scope021::INSTR"		// IP address for TDS3054B
#define SCOPE_ADDR "TCPIP::142.104.60.94::INSTR"	// IP address for TDS3054B
#endif

#define OUTFILE	"tekdaq.dat"			// default output filename

/********************************* Scope Defaults *********************************/

#define SCALE "200.E-3"				// default vertical scale (volts)
#define COUPL "DC"				// default coupling
#define IMPED "MEG"				// default impedance
#define POS "+0.0E0"				// default offset
#define TRSRC "TRIG:A:EDGE:SOU CH1"		// default trigger source
#define SLOPE "TRIG:A:EDGE:SLO RIS"		// default trigger edge slope
#define TRLEV "+500.E-3"			// default trigger level
#define HSAMP "HOR:SCA 20.E-9"			// default horizontal scale
#if defined DPO4104B || defined MDO3054
#define HTRPOS "HOR:POS 20"			// default pretrigger position (20%)
#elif defined TDS3054B
#define HTRPOS "HOR:TRIG:POS 20"		// default pretrigger position (20%)
#endif

/**********************************************************************************/




