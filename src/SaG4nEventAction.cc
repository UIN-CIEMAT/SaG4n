
#include "SaG4nEventAction.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "G4RunManager.hh"


SaG4nEventAction:: SaG4nEventAction(SaG4nInputManager* anInputManager){

  theInputManager=anInputManager;
  OutputType1=0;
  OutputFormat=0;
  AlreadyInit=false;
  NEvents=0;
  NormFactor=1;
  H_NBins=1000;
  H_MaxEne=20*MeV;
  NextPrint_minutes=1;
  for(G4int i=0;i<G4AN_MAXNVOLUMES;i++){
    NeutronSpectra_OneEvent[i]=0;
    AlphaFlux_OneEvent[i]=0;
    NeutronSpectra_Sum[i]=0;
    AlphaFlux_Sum[i]=0;
    NeutronSpectra_Sum2[i]=0;
    AlphaFlux_Sum2[i]=0;
    NeutronSpectra_NEntries[i]=0;
    AlphaFlux_NEntries[i]=0;
  }
}


SaG4nEventAction::~SaG4nEventAction(){

  WriteResults();
  for(G4int i=0;i<G4AN_MAXNVOLUMES;i++){
    if(NeutronSpectra_OneEvent[i]!=0){delete [] NeutronSpectra_OneEvent[i];}
    if(AlphaFlux_OneEvent[i]!=0){delete [] AlphaFlux_OneEvent[i];}
    if(NeutronSpectra_Sum[i]!=0){delete [] NeutronSpectra_Sum[i];}
    if(AlphaFlux_Sum[i]!=0){delete [] AlphaFlux_Sum[i];}
    if(NeutronSpectra_Sum2[i]!=0){delete [] NeutronSpectra_Sum2[i];}
    if(AlphaFlux_Sum2[i]!=0){delete [] AlphaFlux_Sum2[i];}
    if(NeutronSpectra_NEntries[i]!=0){delete [] NeutronSpectra_NEntries[i];}
    if(AlphaFlux_NEntries[i]!=0){delete [] AlphaFlux_NEntries[i];}
  }

}




void  SaG4nEventAction::BeginOfEventAction(const G4Event*){

if(NEvents==0){starttime=time(NULL);}
  NEvents++;
  Init();
  if(OutputType1==0){return;} //No histograms

  //-------------------------------------------------------------
  //Initialize *_OneEvent:
  for(G4int i=0;i<NVolumes;i++){
    NeutronSpectra_NFilled[i]=0;
    AlphaFlux_NFilled[i]=0;
    for(G4int j=0;j<=H_NBins+1;j++){
      NeutronSpectra_OneEvent[i][j]=0;
      AlphaFlux_OneEvent[i][j]=0;
    }
  }
  //-------------------------------------------------------------

}


void  SaG4nEventAction::EndOfEventAction(const G4Event*){

  //-------------------------------------------------------------
  //Pint progress:
  double CompTime=difftime(time(NULL),starttime)/60.; //minutes
  if(CompTime>NextPrint_minutes){
    PrintProgress();
    NextPrint_minutes*=2;
  }
  //-------------------------------------------------------------

  if(OutputType1==0){return;} //No histograms

  //-------------------------------------------------------------
  //Fill results:
  for(G4int i=0;i<NVolumes;i++){
    G4double TotalNeutrons_OneEvent=0;
    G4int cont=0;
    for(G4int j=1;j<=H_NBins+1;j++){ //First bin is for the total number of neutrons !!!
      if(cont==NeutronSpectra_NFilled[i]){break;}
      if(NeutronSpectra_OneEvent[i][j]!=0){
	NeutronSpectra_Sum[i][j]+=NeutronSpectra_OneEvent[i][j];
	NeutronSpectra_Sum2[i][j]+=NeutronSpectra_OneEvent[i][j]*NeutronSpectra_OneEvent[i][j];
	NeutronSpectra_NEntries[i][j]++;
	TotalNeutrons_OneEvent+=NeutronSpectra_OneEvent[i][j];
	cont++;
      }
    }
    if(TotalNeutrons_OneEvent!=0){
      NeutronSpectra_Sum[i][0]+=TotalNeutrons_OneEvent;
      NeutronSpectra_Sum2[i][0]+=TotalNeutrons_OneEvent*TotalNeutrons_OneEvent;
    }
    cont=0;
    for(G4int j=0;j<=H_NBins+1;j++){ 
      if(cont==AlphaFlux_NFilled[i]){break;}
      if(AlphaFlux_OneEvent[i][j]!=0){
	AlphaFlux_Sum[i][j]+=AlphaFlux_OneEvent[i][j];
	AlphaFlux_Sum2[i][j]+=AlphaFlux_OneEvent[i][j]*AlphaFlux_OneEvent[i][j];
	AlphaFlux_NEntries[i][j]++;
	cont++;
      }
    }
  }
  //-------------------------------------------------------------

}



void SaG4nEventAction::AddSecondaryParticle(G4String parname,G4double Energy,G4double weight,G4int VolumeID){

  if(OutputType1==0){return;} //No histograms

  if(parname=="neutron"){
    //-------------------------------------------------------------
    G4int binNumber=(Energy/H_MaxEne)*H_NBins+1;
    if(binNumber>H_NBins+1){binNumber=H_NBins+1;}
    NeutronSpectra_OneEvent[VolumeID][binNumber]+=weight;
    NeutronSpectra_NFilled[VolumeID]++;
    //-------------------------------------------------------------
  }

}

void SaG4nEventAction::AddFlux(G4double EneUp,G4double EneLow,G4double StepLength,G4double weight,G4int VolumeID){

  if(OutputType1==0){return;} //No histograms

  //Equal distribution of StepLengt in the full EneLow-EneUp energy range
  G4double lowE,highE,frac;
  G4double BinWidth=H_MaxEne/H_NBins;
  G4int binMin=(EneLow/H_MaxEne)*H_NBins+1;
  G4int binMax=(EneUp/H_MaxEne)*H_NBins+1;
  binMin--; if(binMin<0){binMin=0;} if(binMin>H_NBins-1){binMin=H_NBins-1;}
  binMax++; if(binMax<0){binMax=0;} if(binMax>H_NBins-1){binMax=H_NBins-1;}
  for(G4int i=binMin;i<binMax;i++){
    lowE=BinWidth*i;
    highE=BinWidth*(i+1);
    frac=0;
    if(lowE<=EneLow && highE>=EneLow && highE<=EneUp){
      frac=(highE-EneLow)/(EneUp-EneLow);
    }
    else if(lowE>=EneLow && highE<=EneUp){
      frac=BinWidth/(EneUp-EneLow);
    }
    else if(lowE>=EneLow && lowE<=EneUp && highE>=EneUp){
      frac=(EneUp-lowE)/(EneUp-EneLow);
    }
    else if(lowE<=EneLow && highE>=EneUp){
      frac=1;
    }
    if(frac>0){
      //-------------------------------------------------------------
      G4int binNumber=((lowE+highE)/2./H_MaxEne)*H_NBins+1;
      if(binNumber>H_NBins+1){binNumber=H_NBins+1;}
      AlphaFlux_OneEvent[VolumeID][binNumber]+=frac*StepLength*weight;
      AlphaFlux_NFilled[VolumeID]++;
      //-------------------------------------------------------------
    }
  }

}


void SaG4nEventAction::WriteResults(){


  if(OutputType1==0){return;}

  //==================================================================
  //Normalization:
  NormFactor=theInputManager->GetNormalizationFactor();

  for(G4int i=0;i<NVolumes;i++){ //Neutron spectra:
    //-------------------------------------------------------------
    for(G4int j=0;j<=H_NBins+1;j++){
      NeutronSpectra_Sum[i][j]*=NormFactor/NEvents;
      NeutronSpectra_Sum2[i][j]*=NormFactor*NormFactor/NEvents;
    }
    //-------------------------------------------------------------
  }
  for(G4int i=0;i<NVolumes;i++){ //Alpha flux:
    G4double FinalNormFac=NormFactor/theInputManager->GetVolumeSize(i);
    //-------------------------------------------------------------
    for(G4int j=0;j<=H_NBins+1;j++){
      AlphaFlux_Sum[i][j]*=FinalNormFac/NEvents;
      AlphaFlux_Sum2[i][j]*=FinalNormFac*FinalNormFac/NEvents;
    }
    //-------------------------------------------------------------
  }
  //==================================================================

  G4double error2=0;
  G4String Hname;

  //==================================================================
  // Write results:
  if(OutputFormat==1){ // ASCII format 
    std::ofstream out(theInputManager->GetOutFname()+G4String(".txt"));
    if(!out.good()){
      G4cout<<" ############ Error opening "<<theInputManager->GetOutFname()+G4String(".txt")<<" (please set the appropriate path in the OUTPUTFILE field of the input file) ############"<<G4endl; exit(1);
    }
    G4int CWidth=15;
    //----------------------------------------------------------------
    for(G4int i=0;i<NVolumes;i++){ //Neutron spectra:
      Hname="NSpec_"+theInputManager->GetVolName(i);
      out<<Hname<<"  "<<H_NBins<<"  "<<H_MaxEne<<G4endl;
      for(G4int j=0;j<=H_NBins+1;j++){
	//error2=(NeutronSpectra_Sum2[i][j]-NeutronSpectra_Sum[i][j]*NeutronSpectra_Sum[i][j])/NEvents; //option 1
	error2=NeutronSpectra_Sum2[i][j]/NEvents; //option 2
	out<<std::setw(CWidth)<<(H_MaxEne/H_NBins)*j<<" "<<std::setw(CWidth)<<NeutronSpectra_Sum[i][j]<<" "<<std::setw(CWidth)<<sqrt(error2)<<G4endl;
      }
      out<<G4endl;
    }
    //----------------------------------------------------------------
    //----------------------------------------------------------------
    for(G4int i=0;i<NVolumes;i++){ //Alpha fluence:
      Hname="AFlux_"+theInputManager->GetVolName(i);
      out<<Hname<<"  "<<H_NBins<<"  "<<H_MaxEne<<G4endl;
      for(G4int j=0;j<=H_NBins+1;j++){
	error2=(AlphaFlux_Sum2[i][j]-AlphaFlux_Sum[i][j]*AlphaFlux_Sum[i][j])/NEvents; //option 1
	//error2=AlphaFlux_Sum2[i][j]/NEvents; //option 2
	out<<std::setw(CWidth)<<(H_MaxEne/H_NBins)*j<<" "<<std::setw(CWidth)<<AlphaFlux_Sum[i][j]*cm2<<" "<<std::setw(CWidth)<<sqrt(error2)*cm2<<G4endl;
      }
      out<<G4endl;
    }
    //----------------------------------------------------------------
    out.close();
  }
  else if(OutputFormat==2){ // ROOT format
    //-------------------------------------------------------------------
    //Init Analysis Manager
    G4AnalysisManager* theAnalysisManager=G4AnalysisManager::Instance();
    G4bool fileOpen=theAnalysisManager->OpenFile(theInputManager->GetOutFname()+G4String(".root"));
    if(!fileOpen){
      G4cout<<" ############ Error opening "<<theInputManager->GetOutFname()+G4String(".root")<<" (please set the appropriate path in the OUTPUTFILE field of the input file) ############"<<G4endl; exit(1);
    }
    //Neutron spectrum and alpha flux histograms:
    for(G4int i=0;i<NVolumes;i++){
      Hname="NSpec_"+theInputManager->GetVolName(i);
      theAnalysisManager->CreateH1(Hname,"Neutron energy spectrum",H_NBins, 0.,H_MaxEne);
      theAnalysisManager->SetH1XAxisTitle(i,"Neutron energy (MeV)");
    }

    for(G4int i=0;i<NVolumes;i++){
      Hname="AFlux_"+theInputManager->GetVolName(i);
      theAnalysisManager->CreateH1(Hname,"Alpha flux",H_NBins, 0.,H_MaxEne);
      theAnalysisManager->SetH1XAxisTitle(NVolumes+i,"Alpha energy (MeV)");
    }
    //-------------------------------------------------------------------
    //----------------------------------------------------------------
    //geant4.10.05/source/analysis/g4tools/include/tools/histo/h1
    for(G4int i=0;i<NVolumes;i++){ //Neutron spectra:
      tools::histo::h1d* h1=theAnalysisManager->GetH1(i);
      for(G4int j=0;j<=H_NBins+1;j++){
	//error2=(NeutronSpectra_Sum2[i][j]-NeutronSpectra_Sum[i][j]*NeutronSpectra_Sum[i][j])/NEvents; //option 1
	error2=NeutronSpectra_Sum2[i][j]/NEvents; //option 2
	h1->set_bin_content(j,NeutronSpectra_NEntries[i][j],NeutronSpectra_Sum[i][j],error2,0,0);
      }
    }
    for(G4int i=0;i<NVolumes;i++){ //Alpha fluence:
      tools::histo::h1d* h1=theAnalysisManager->GetH1(NVolumes+i);
      for(G4int j=0;j<=H_NBins+1;j++){
	error2=(AlphaFlux_Sum2[i][j]-AlphaFlux_Sum[i][j]*AlphaFlux_Sum[i][j])/NEvents; //option 1
	//error2=AlphaFlux_Sum2[i][j]/NEvents; //option 2
	h1->set_bin_content(j,AlphaFlux_NEntries[i][j],AlphaFlux_Sum[i][j]*cm2,error2*cm2*cm2,0,0);
      }
    }
    theAnalysisManager->Write();
    theAnalysisManager->CloseFile();
    delete theAnalysisManager;
    //----------------------------------------------------------------
  }
  else{
    G4cout<<" ############ Error writing results, OutputFormat =  "<<OutputFormat<<" not yet defined !! ############"<<G4endl; exit(1);
  }
  //==================================================================



}

void SaG4nEventAction::Init(){

  if(AlreadyInit){return;}
  AlreadyInit=true;
  theInputManager->ReadInput();

  OutputType1=theInputManager->GetOutputType1();
  OutputFormat=theInputManager->GetOutputFormat();

  if(OutputType1==0){return;} //then no histograms

  NVolumes=theInputManager->GetNVolumes();

  for(G4int i=0;i<NVolumes;i++){
    NeutronSpectra_Sum[i]=new G4double[H_NBins+2];
    NeutronSpectra_Sum2[i]=new G4double[H_NBins+2];
    NeutronSpectra_NEntries[i]=new G4int[H_NBins+2];
    NeutronSpectra_OneEvent[i]=new G4double[H_NBins+2];
    AlphaFlux_Sum[i]=new G4double[H_NBins+2];
    AlphaFlux_Sum2[i]=new G4double[H_NBins+2];
    AlphaFlux_NEntries[i]=new G4int[H_NBins+2];
    AlphaFlux_OneEvent[i]=new G4double[H_NBins+2];
    for(G4int j=0;j<=H_NBins+1;j++){
      NeutronSpectra_Sum[i][j]=0;
      NeutronSpectra_Sum2[i][j]=0;
      NeutronSpectra_NEntries[i][j]=0;
      NeutronSpectra_OneEvent[i][j]=0;
      AlphaFlux_Sum[i][j]=0;
      AlphaFlux_Sum2[i][j]=0;
      AlphaFlux_NEntries[i][j]=0;
      AlphaFlux_OneEvent[i][j]=0;
    }
  }

}



void SaG4nEventAction::PrintProgress(){

  double CompTime=difftime(time(NULL),starttime)/60.; //minutes
  G4int TotalNEvents=theInputManager->GetNEvents();

  G4cout<<" #### PROGRESS: "<<NEvents<<"/"<<TotalNEvents<<" run histories have been computed ("<<NEvents*100./TotalNEvents<<" %) in "<<CompTime<<" minutes ("<<CompTime/60.<<" h) ---> ("<<NEvents*60./CompTime<<" run histories/h) ####"<<G4endl;
  G4cout<<" #### Estimated time left to complete the job ("<<theInputManager->GetInputFname()<<"): "<<CompTime*(TotalNEvents-NEvents)/(G4double)NEvents<<" minutes ("<<CompTime*(TotalNEvents-NEvents)/(G4double)NEvents*1/60.<<" h) ####"<<G4endl;
  
}




