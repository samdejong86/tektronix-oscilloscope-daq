#include <stdint.h>
#include <sstream>
#include "markTime.h"

using namespace std;

#include "fileManager.h"

void fileManager::OpenFile(){

  struct stat buf;

  if ( ! stat (fname.c_str(), &buf) )
    {
      printf ("Overwrite existing file `%s'? (y/n) ", fname.c_str());
      fflush (stdout);
      if (getc (stdin) != 'y')
	{
	  exit (0);
	}
      printf ("\n");
    }
  
  f = new TFile(fname.c_str(), "RECREATE");
  t = new TTree("wave", "wave");
  
  data.resize(4);  
  data.size();

  stringstream ss;
  
  for(int i=0; i<4; i++){
    ss<<i+1;
    string branch="ch"+ss.str();
    ss.str("");
    if(Parameters->get_wave[i]==1)
      t->Branch(branch.c_str(), &data[i]);
  }

  t->Branch("xinc", &xin, "xinc/D");
  t->Branch("time", &time, "time/D");

  if(Parameters->measureData){
    if(Parameters->get_wave[0]==1)
      t->Branch("pkch1", &CH1, "pkch1/D");
    if(Parameters->get_wave[1]==1)
      t->Branch("pkch2", &CH2, "pkch2/D");
    if(Parameters->get_wave[2]==1)
      t->Branch("pkch3", &CH3, "pkch3/D");
    if(Parameters->get_wave[3]==1)
      t->Branch("pkch4", &CH4, "pkch4/D");
  }
}

void fileManager::CloseFile(){

  t->Write();
  f->Close();

}

void fileManager::addEvent(char Curve[4][CLEN], int ByteCount[4]){

  time=markTime();

  for(int i=0; i<4; i++){
    if(Parameters->get_wave[i]==1){
      for(int j=14; j<ByteCount[i]-1; j++){
	int temp = (uint8_t)Curve[i][j];
	data[i].push_back(1000*( ((float)temp-yof[i])*ymu[i]+yze[i])  );
      }
    } else
      data[i].push_back(0);
    
  }

  
  if(Parameters->measureData){
    if(Parameters->get_wave[0]==1)
      CH1 = Parameters->CH1;
    if(Parameters->get_wave[1]==1)
      CH2 = Parameters->CH2;
    if(Parameters->get_wave[2]==1)
      CH3 = Parameters->CH3;
    if(Parameters->get_wave[3]==1)
      CH4 = Parameters->CH4;
  }


  t->Fill();
  for(int i=0; i<4; i++)
    data[i].clear();
   

}

std::string get_str_between_two_str(const std::string &s,
        const std::string &start_delim,
        const std::string &stop_delim)
{
    unsigned first_delim_pos = s.find(start_delim);
    unsigned end_pos_of_first_delim = first_delim_pos + start_delim.length();
    unsigned last_delim_pos = s.find_first_of(stop_delim, end_pos_of_first_delim);

    return s.substr(end_pos_of_first_delim,
            last_delim_pos - end_pos_of_first_delim);
}

void fileManager::parseHeader(char Header[CLEN], int ByteCount){
  
  string str(Header);

  int chn = atoi(get_str_between_two_str(str, "Ch", ",").c_str());
  
  xin = atof(get_str_between_two_str(str, "XINCR", ";").c_str());
  xun = get_str_between_two_str(str, "XUNIT", ";");
  yun = get_str_between_two_str(str, "YUNIT", ";");
  ymu[chn-1] = atof(get_str_between_two_str(str, "YMULT", ";").c_str());
  yof[chn-1] = atof(get_str_between_two_str(str, "YOFF", ";").c_str());
  yze[chn-1] = atof(get_str_between_two_str(str, "YZERO", ";").c_str());
  

}
 

void fileManager::ParseMetaData(string Header){
  
  if(!f->IsOpen()){
    cout<<"File not open!"<<endl;
    return;
  }

  double Scale[4];
  string Bandwidth[4];
  string Coupling[4];
  string Imp[4];
  double pos[4];

  stringstream ss;

  for(int i=0; i<4; i++){
    ss<<i+1;
    string num = ss.str();

    Scale[i] = atof(get_str_between_two_str(Header, "CH"+num+":SCALE", ":").c_str());
    Bandwidth[i] = get_str_between_two_str(Header, "CH"+num+":BANDWIDTH", ":");
    Coupling[i] = get_str_between_two_str(Header, "CH"+num+":COUPLING", ":");
    Imp[i] = get_str_between_two_str(Header, "CH"+num+":IMPEDANCE", ":");
    pos[i] = atof(get_str_between_two_str(Header, "CH"+num+":POSITION", ":").c_str());
	    		    
    ss.str("");
  }
  
  string HResolution  = get_str_between_two_str(Header,":HORIZONTAL:RESOLUTION", ";"); 
  double HScale = atof(get_str_between_two_str(Header,":HORIZONTAL:MAIN:SCALE", ";").c_str());

  int TrigSource = atoi(get_str_between_two_str(Header,":TRIGGER:A:EDGE:SOURCE CH", ":").c_str());
  string TrigSlope = get_str_between_two_str(Header,":TRIGGER:A:EDGE:SLOPE", ":");  
  double TrigLevel = atof(get_str_between_two_str(Header,":TRIGGER:A:LEVEL", ":").c_str());

  TTree *meta = new TTree("MetaData", "Run information");

  for(int i=0; i<4; i++){
    ss<<i+1;
    string num = ss.str();

    meta->Branch(("Ch"+num+"_Scale").c_str(), &Scale[i],("Ch"+num+"_Scale/D").c_str() );
    meta->Branch(("Ch"+num+"_Bandwidth").c_str(), &Bandwidth[i]);
    meta->Branch(("Ch"+num+"_Coupling").c_str(), &Coupling[i]);
    meta->Branch(("Ch"+num+"_Impedance").c_str(), &Imp[i]);
    meta->Branch(("Ch"+num+"_Position").c_str(), &pos[i]);
    ss.str("");
  }
  
  meta->Branch("TriggerSource", &TrigSource, "TriggerSource/I");
  meta->Branch("TriggerLevel", &TrigLevel, "TriggerLevel/D");
  meta->Branch("TriggerSlope", &TrigSlope);

  meta->Branch("HorizontalResolution", &HResolution);
  meta->Branch("HorizontalScale", &HScale, "HorizontalScale/D");


  
  meta->Fill();
  meta->Write();
  
}
