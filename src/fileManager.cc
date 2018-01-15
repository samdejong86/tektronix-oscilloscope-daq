#include <stdint.h>
#include <sstream>

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

    t->Branch(branch.c_str(), &data[i]);
  }

  t->Branch("xinc", &xin, "xinc/D");



}

void fileManager::CloseFile(){

  t->Write();
  f->Close();

}

void fileManager::addEvent(char Curve[4][CLEN], int ByteCount[4]){

  for(int i=0; i<4; i++){
    for(int j=14; j<ByteCount[i]-1; j++){
    int temp = (uint8_t)Curve[i][j];
    data[i].push_back(1000*( ((float)temp-yof[i])*ymu[0]+yze[i])  );
          
    }
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

void fileManager::parseHeader(char Header[CLEN]){
  
  string str(Header);

  int chn = atoi(get_str_between_two_str(str, "Ch", ",").c_str());
  
  xin = atof(get_str_between_two_str(str, "XINCR", ";").c_str());
  xun = get_str_between_two_str(str, "XUNIT", ";");
  yun = get_str_between_two_str(str, "YUNIT", ";");
  ymu[chn-1] = atof(get_str_between_two_str(str, "YMULT", ";").c_str());
  yof[chn-1] = atof(get_str_between_two_str(str, "YOFF", ";").c_str());
  yze[chn-1] = atof(get_str_between_two_str(str, "YZERO", ";").c_str());
  

}
 
