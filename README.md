# Tektronix Oscilloscope DAQ

## Description

 This is a data acquisition program for Tektronics oscilloscopes. The software is written for these models:
 * TDS3054B
 * MDO3054   (untested)
 * DPO4104B  (untested)

## Installation

  You will need to install the NI VISA software before installing the DAQ software. You can find it [here](https://www.ni.com/visa/).  
(SL6 version [here](http://www.ni.com/download/ni-visa-17.0/6700/en/))

### NI VISA installation instructions:
    cd /tmp 
    mkdir NiVisa
    cd NiVisa
    cp $NIVISALOC/NI-VISA-17.0.0.iso .
    mount -o loop NI-VISA-17.0.0.iso .
    ./Install
    umount /tmp/NiVisa

 You may need these packages
 
	     libstdc++-devel
	     libstdc++.i686
	     avahi-devel.x86_64


  You will also need to install ROOT from cern ([here](https://root.cern.ch/downloading-root
))
  Makefiles for the different models can be found in the makefiles directory. Use the appropriate one for the 'scope your using.

  Build the software using 'make'


## Usage

  Run tekdaq -h to get usage instructions.


## Output

  The software produces a ROOT file containing two branches:
  
	MetaData:
		Contains information on the 'scope setting during this run
	data:
		Contains branches with the data:
			 a vector<double> branch containing the waveform
			 a double branch containing the peak as measured by the 'scope
			 a double branch containing the horizontal bin width
			 a double branch containing the trigger timestamp in UNIX time


  There is a ROOT script included for displaying the waveforms (draw.C). To use it, run			 
  	.x draw.C("tek.root", ch, ev) 
  at the ROOT prompt. ch is the channel number, ev is the event number.

  While running, the software produces temporary files named filename_n.root, each containing some number of entries (set by the tekdaq -r command). If the software closes without crashing, these are merged into filename.root and deleted.  

  
  ## Acknowledgements 
  
  Much of the code used in this software was written by Paul Poffenberger. I adapted the code to an object oriented framework and added some more options
