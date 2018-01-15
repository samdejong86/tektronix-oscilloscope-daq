

void draw(TString file, int ch, int event){

  TFile *f = new TFile(file);
  TTree *t = (TTree*)f->Get("wave");

  stringstream ss;
  ss<<ch;
  TString c = ss.str();

  vector<double> *data;

  t->SetBranchAddress("ch"+c, &data);

  t->GetEntry(event);

  TGraph *gr = new TGraph();
  
  for(int i=0; i<data->size(); i++){
    gr->SetPoint(i+1, i, data->at(i));

  }

  gr->Draw("AP");



}
