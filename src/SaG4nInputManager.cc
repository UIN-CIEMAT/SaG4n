
#include <fstream>
#include <sstream>
#include "G4ios.hh"
#include "G4Isotope.hh"
#include "G4Element.hh"
#include "G4Material.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "G4NistManager.hh"

#include "SaG4nInputManager.hh"


SaG4nInputManager::SaG4nInputManager(const char* infpname){

  InputFname=infpname;
  InputAlreadyRead=false;
  for(G4int i=0;i<G4AN_MAXNMATERIALS;i++){
    theDefinedMaterials[i]=0;
  }
  NVolumes=0;
  WorldSize=1000*cm;

  MaxStepSize=0.001*cm;
  BiasFactor=-1.0;
  NEvents=100000;
  OutputType1=1;
  OutputType2=0;
  OutputFormat=2;
  OutFname="./output";
  NormalizationFactor=1;
  Interactive=false;
  KillSecondaries=true;
  Seed=1234567;
  G4Random::setTheSeed(Seed);

  SourcePostType=0,NSourcePosParameters=0,SourceNormFactor=0;NSourceEnergies=0;
  SourceEnergy=0; SourceSigma=0; SourceIntensity=0;

}

SaG4nInputManager::~SaG4nInputManager(){

  if(SourceEnergy){delete [] SourceEnergy;}
  if(SourceSigma){delete [] SourceSigma;}
  if(SourceIntensity){delete [] SourceIntensity;}

}


void SaG4nInputManager::ReadInput(){

  if(InputAlreadyRead){return;} //then alreade read
  InputAlreadyRead=true;

  G4String word;
  std::ifstream in(InputFname.c_str());
  if(!in.good()){
    G4cout<<" ****** ERROR opening input: "<<InputFname<<" ******"<<G4endl; exit(1);
  }     

  while(in>>word){
    if(word=="END"){break;}
    else if(word.c_str()[0]=='#'){in.ignore(10000,'\n');}
    //----------------------------------------------------------------------------------------------------
    else if(word=="MATERIAL"){
      G4String MatName;
      G4int matID,nIsotopes,ZA[1000];
      G4double density,fraction[1000];
      in>>matID>>MatName>>density>>nIsotopes;
      density*=g/cm3;
      if(matID>=G4AN_MAXNMATERIALS){
	G4cout<<" ******* Error reading input. matID max is "<<G4AN_MAXNMATERIALS-1<<" ---> matID = "<<matID<<" *******"<<G4endl; exit(1);
      }
      if(nIsotopes<=0){ //then we take a material from the NIST database
	theDefinedMaterials[matID]=G4NistManager::Instance()->FindOrBuildMaterial(MatName);
	if(theDefinedMaterials[matID]==0){
	  G4cout<<" Error building NIST material with name = "<<MatName<<G4endl; 
	  G4cout<<" ########## Error in "<<__FILE__<<", line "<<__LINE__<<" ##########"<<G4endl; exit(1);
	}	
      }
      else{
	for(G4int i=0;i<nIsotopes;i++){
	  in>>ZA[i]>>fraction[i];
	}
	in>>word;
	if(word!="ENDMATERIAL"){
	  G4cout<<" ****** Error reading material "<<matID<<" ******"<<G4endl;
	  G4cout<<" ****** ERROR reading input: "<<InputFname<<" ******"<<G4endl; exit(1);
	}
	if(ZA[0]<150){
	  theDefinedMaterials[matID]=CreateMaterialFromElements(MatName,nIsotopes,density,ZA,fraction);
	}
	else{
	  theDefinedMaterials[matID]=CreateMaterialFromIsotopes(MatName,nIsotopes,density,ZA,fraction);
	}
      }
    }
    //----------------------------------------------------------------------------------------------------
    else if(word=="VOLUME"){
      in>>VolID[NVolumes]>>VolName[NVolumes]>>MatID[NVolumes]>>VolType[NVolumes]>>MotherID[NVolumes]>>word;
      if(VolType[NVolumes]==1){ // Sphere
	NVolParam[NVolumes]=4;
	for(G4int i=0;i<NVolParam[NVolumes];i++){in>>VolParam[NVolumes][i]; VolParam[NVolumes][i]*=cm;} // Radius, VX, VY, VZ 
	VolumeSize[NVolumes]=4./3.*3.141592*VolParam[NVolumes][0]*VolParam[NVolumes][0]*VolParam[NVolumes][0];
      }
      else if(VolType[NVolumes]==2){ // Box
	NVolParam[NVolumes]=6;
	for(G4int i=0;i<NVolParam[NVolumes];i++){in>>VolParam[NVolumes][i]; VolParam[NVolumes][i]*=cm;} // DX, DY, DZ, VX, VY, VZ
	VolumeSize[NVolumes]=VolParam[NVolumes][0]*VolParam[NVolumes][1]*VolParam[NVolumes][2];
      }
      else if(VolType[NVolumes]==3){ // Cylinder
	NVolParam[NVolumes]=5;
	for(G4int i=0;i<NVolParam[NVolumes];i++){in>>VolParam[NVolumes][i]; VolParam[NVolumes][i]*=cm;} // Radius, Length, VX, VY, VZ
	VolumeSize[NVolumes]=3.141592*VolParam[NVolumes][0]*VolParam[NVolumes][0]*VolParam[NVolumes][1];
      }
      else{
	G4cout<<" ******* Error reading input. Volume type "<<VolType[NVolumes]<<" not yet defined *******"<<G4endl; exit(1);
      }
      NVolumes++;
    }
    //----------------------------------------------------------------------------------------------------
    else if(word=="WORLDSIZE"){
      in>>WorldSize; WorldSize*=cm;
    }
    else if(word=="MAXSTEPSIZE"){
      in>>MaxStepSize; MaxStepSize*=cm;
    }
    else if(word=="BIASFACTOR"){
      in>>BiasFactor; 
    }
    else if(word=="NEVENTS"){
      in>>NEvents; 
    }
    else if(word=="OUTPUTTYPE"){
      in>>OutputType1>>OutputType2>>OutputFormat; 
      if(OutputType1!=0 && OutputType1!=1){
	G4cout<<" ******* Error reading input. OutType1 = "<<OutputType1<<" not yet defined *******"<<G4endl; exit(1);
      }
      if(OutputType2!=0 && OutputType2!=1 && OutputType2!=2){
	G4cout<<" ******* Error reading input. OutType2 = "<<OutputType2<<" not yet defined *******"<<G4endl; exit(1);
      }
      if(OutputFormat!=1 && OutputFormat!=2){
	G4cout<<" ******* Error reading input. OutputFormat = "<<OutputFormat<<" not yet defined *******"<<G4endl; exit(1);
      }
    }
    else if(word=="OUTPUTFILE"){ //without the extension
      in>>OutFname; 
    }
    else if(word=="INTERACTIVE"){
      Interactive=true;
    }
    else if(word=="DONOTKILLSECONDARIES"){
      KillSecondaries=false;
    }
    else if(word=="SEED"){
      in>>Seed;
      G4Random::setTheSeed(Seed);
    }
    //----------------------------------------------------------------------------------------------------
    else if(word=="SOURCE"){
      in>>SourcePostType;
      if(SourcePostType==-1){ // beam mode
	NSourcePosParameters=8; // x0, y0, z0, ux, uy, uz, rad, sigma
      }
      else if(SourcePostType==0){ // source uniformly distributed inside a volume
	in>>NSourcePosParameters; //VolumeID
      }
      else if(SourcePostType==1){ // Sphere 
	NSourcePosParameters=4; // Radius, VX, VY, VZ 
      }
      else if(SourcePostType==2){ // Box
	NSourcePosParameters=6; // DX, DY, DZ, VX, VY, VZ
      }
      else if(SourcePostType==3){ // Cylinder
	NSourcePosParameters=5; // Radius, Length, VX, VY, VZ
      }
      else{
	G4cout<<" ******* Error reading input. SourcePostType "<<SourcePostType<<" not yet defined *******"<<G4endl; exit(1);
      }
      if(SourcePostType!=0){
	for(G4int i=0;i<NSourcePosParameters;i++){in>>SourcePosParameters[i]; SourcePosParameters[i]*=cm;}
      }
      in>>SourceNormFactor>>NSourceEnergies;
      SourceEnergy=new G4double[NSourceEnergies];
      SourceIntensity=new G4double[NSourceEnergies];
      SourceSigma=new G4double[NSourceEnergies];
      G4bool HasChains=false;
      G4String tmp_str;
      G4double tmp_val;
      in.ignore(10000,'\n');
      for(G4int i=0;i<NSourceEnergies;i++){
	getline(in,tmp_str);
	std::istringstream iss(tmp_str);
	iss>>word>>SourceEnergy[i]>>SourceIntensity[i];
	if(iss>>tmp_val){SourceSigma[i]=tmp_val;}
	else{SourceSigma[i]=-1;}
	if(word=="Chain_Th232"){SourceEnergy[i]=-1; HasChains=true;}
	else if(word=="Chain_U235"){SourceEnergy[i]=-2; HasChains=true;}
	else if(word=="Chain_U238"){SourceEnergy[i]=-3; HasChains=true;}
	else{
	  SourceEnergy[i]*=MeV;
	  SourceSigma[i]*=MeV;
	  if(SourceEnergy[i]<0 || SourceEnergy[i]>200*MeV){
	    G4cout<<" ******* Error: Alpha energy from the source (i="<<i+1<<") is "<<SourceEnergy[i]/MeV<<" MeV, which is out of the [0,200] MeV range *******"<<G4endl; exit(1);
	  }
	}
      }
      in>>word;
      if(word!="ENDSOURCE"){
	G4cout<<" ****** ERROR reading input: "<<InputFname<<" --> "<<word<<" (Source) ******"<<G4endl; exit(1);
      }
      if(HasChains){
	AddChainsToSource();
      }
    }
    //----------------------------------------------------------------------------------------------------
    else{
      G4cout<<" ****** ERROR reading input: "<<InputFname<<" ******"<<G4endl; 
      G4cout<<" Last word read was ---> "<<word<<G4endl; exit(1);
    }
  }

  if(!in.good()){
    G4cout<<" ****** ERROR reading input: "<<InputFname<<" ******"<<G4endl;
    G4cout<<" Last word read was: "<<word<<G4endl; exit(1);
  }
  in.close();
  
  //-------------------------------------------------------------------------
  // re-calculate volume sizes, subtracting daughters:
  G4double ToBeSubtracted[G4AN_MAXNVOLUMES];
  for(G4int i=0;i<G4AN_MAXNVOLUMES;i++){ToBeSubtracted[i]=0;}
  for(G4int i=0;i<NVolumes;i++){
    if(MotherID[i]!=0){
      for(G4int j=0;j<NVolumes;j++){
	if(VolID[j]==MotherID[i]){
	  ToBeSubtracted[j]+=VolumeSize[i];
	}
      }
    }
  }
  for(G4int i=0;i<NVolumes;i++){
    VolumeSize[i]-=ToBeSubtracted[i];
  }
  //-------------------------------------------------------------------------

  //------------------------------------------
  //Check that we can open output files:
  if(OutputType1!=0 || OutputType2!=0){
    G4String testfname=OutFname+G4String(".test");
    std::ofstream out(testfname.c_str());
    if(!out.good()){
      G4cout<<" ############ Error trying to open files in "<<OutFname<<" (please set the appropriate path in the OUTPUTFILE field of the input file) ############"<<G4endl; exit(1);
    }
    remove(testfname.c_str());
  }
  //------------------------------------------

  
}




G4Material* SaG4nInputManager::CreateMaterialFromIsotopes(G4String MatName,G4int nIsotopes,G4double density,G4int* ZA,G4double* atomicfraction){

  //---------------------------------------------------------------
  //Calculate the isotope fractions:
  G4double isofraction[1000];
  G4double Sum[100];
  for(G4int i=0;i<100;i++){
    Sum[i]=0;
  }
  for(G4int i=0;i<100;i++){
    for(G4int j=0;j<nIsotopes;j++){
      if((ZA[j]/1000)==i){
        Sum[i]+=atomicfraction[j];
      }
    }
  }
  for(G4int i=0;i<100;i++){
    for(G4int j=0;j<nIsotopes;j++){
      if((ZA[j]/1000)==i){
        isofraction[j]=atomicfraction[j]/Sum[i];
      }
    }
  }
  //---------------------------------------------------------------
  G4double temperature=293*kelvin;
  char name[100];
  //---------------------------------------------------------------
  //Create the isotopes:
  G4int theA,theZ;
  G4double IsoMass[100];
  G4Isotope** theIsotopes=new G4Isotope*[nIsotopes];
  for(G4int i=0;i<nIsotopes;i++){
    theZ=ZA[i]/1000;
    theA=ZA[i]-theZ*1000;
    sprintf(name,"iso_%d",ZA[i]);
    theIsotopes[i]=new G4Isotope(G4String(name),theZ,theA);//mass taken from the NIST table by default
    IsoMass[i]=theIsotopes[i]->GetA();
  }
  //---------------------------------------------------------------

  //---------------------------------------------------------------
  //Create the elements:
  G4int IsotopesPerElement[100];
  for(G4int i=0;i<100;i++){
    IsotopesPerElement[i]=0;
  }
  for(G4int i=0;i<nIsotopes;i++){
    theZ=ZA[i]/1000;
    IsotopesPerElement[theZ]++;
  }
  G4int nElements=0;
  for(G4int i=0;i<100;i++){
    if(IsotopesPerElement[i]!=0){
      nElements++;
    }
  }
  G4int cont=0;
  G4Element** theElements=new G4Element*[nElements];
  G4double themassfractions[100];
  G4double totalmassfraction=0;
  for(G4int i=0;i<100;i++){
    if(IsotopesPerElement[i]!=0){
      sprintf(name,"Z_%d",i);
      theElements[cont]=new G4Element(G4String(name),G4String(name),IsotopesPerElement[i]);
      themassfractions[cont]=0;
      for(G4int j=0;j<nIsotopes;j++){
        theZ=ZA[j]/1000;
        if(theZ==i){
          theElements[cont]->AddIsotope(theIsotopes[j],isofraction[j]);
          themassfractions[cont]+=atomicfraction[j]*IsoMass[j];
        }
      }
      totalmassfraction+=themassfractions[cont];
      cont++;
    }
  }
  //---------------------------------------------------------------
  //Material:
  G4Material* theMaterial=new G4Material(MatName,density,nElements,kStateUndefined,temperature);
  G4double fractionmass=0;
  for(G4int i=0;i<nElements;i++){
     theMaterial->AddElement(theElements[i],fractionmass=themassfractions[i]/totalmassfraction);
  }
  //---------------------------------------------------------------
  return theMaterial;

}


G4Material* SaG4nInputManager::CreateMaterialFromElements(G4String MatName,G4int nElements,G4double density,G4int* Element_Z,G4double* elementfraction){

  //---------------------------------------------------------------
  //Elements:
  G4Element* theElements[100];
  for(G4int i=0;i<nElements;i++){
    theElements[i]=G4NistManager::Instance()->FindOrBuildElement(Element_Z[i]);
    if(theElements[i]==0){
      G4cout<<" Error building element with Z = "<<Element_Z[i]<<G4endl; 
      G4cout<<" ########## Error in "<<__FILE__<<", line "<<__LINE__<<" ##########"<<G4endl; exit(1);
    }
  }
  //---------------------------------------------------------------

  //---------------------------------------------------------------
  //if elementfraction[i]>0 then it is atom fraction
  //if elementfraction[i]<0 then it is mass fraction
  G4double elementmassfraction[1000];
  G4double totalmassfraction=0;
  if(elementfraction[0]>0){ // atom fraction
    for(G4int i=0;i<nElements;i++){
      elementmassfraction[i]=elementfraction[i]*theElements[i]->GetAtomicMassAmu();
      totalmassfraction+=elementmassfraction[i];
    }
  }
  else{ // mass fraction
    for(G4int i=0;i<nElements;i++){
      elementmassfraction[i]=-elementfraction[i];
      totalmassfraction+=elementmassfraction[i];
    }
  }
  //---------------------------------------------------------------


  //---------------------------------------------------------------
  //Material:
  G4double temperature=293*kelvin;
  G4Material* theMaterial=new G4Material(MatName,density,nElements,kStateUndefined,temperature);
  G4double fractionmass=0;
  for(G4int i=0;i<nElements;i++){
     theMaterial->AddElement(theElements[i],fractionmass=elementmassfraction[i]/totalmassfraction);
  }
  //---------------------------------------------------------------

  return theMaterial;

}



void SaG4nInputManager::GetVolumeTrVector(G4int id,G4double& vx,G4double& vy,G4double& vz){

  if(id>=0 && id<NVolumes){
    if(VolType[id]==1){
      vx=VolParam[id][1]; vy=VolParam[id][2]; vz=VolParam[id][3]; 
    }
    else if(VolType[id]==2){
      vx=VolParam[id][3]; vy=VolParam[id][4]; vz=VolParam[id][5]; 
    }
    else if(VolType[id]==3){
      vx=VolParam[id][2]; vy=VolParam[id][3]; vz=VolParam[id][4]; 
    }
    else{
      G4cout<<" ######### Error in "<<__FILE__<<", line "<<__LINE__<<" #########"<<G4endl;  exit(1);
    }
  }

}



void SaG4nInputManager::AddChainsToSource(){

  //==============================================================================================
  G4int const np_Th232=26;
  G4double Ea_Th232[np_Th232]={4.0123,3.9472,3.8111,5.42315,5.34036,5.211,5.173,5.138,5.68537,5.4486,5.161,5.051,5.034,6.28808,5.747,6.7783,5.985,6.08988,6.05078,5.768,5.626,5.607,5.481,5.345,5.302,8.78486};
  G4double Ia_Th232[np_Th232]={78.2,21.7,0.069,73.4,26,0.408,0.218,0.036,94.92,5.06,0.0071,0.0076,0.003,99.886,0.114,99.9981,0.0019,9.74693,25.1257,0.61098,0.0564258,0.406122,0.0046722,0.0003594,3.9534e-05,64.06};
  //-------------------------------------
  G4int const np_U235=160;
  G4double Ea_U235[np_U235]={4.5974,4.5561,4.5025,4.4385,4.4149,4.3954,4.3643,4.3229,4.295,4.2829,4.2662,4.248,4.219,4.2158,4.153,4.0806,3.976,5.0586,5.032,5.0284,5.0138,4.986,4.975,4.9513,4.934,4.9,4.853,4.795,4.736,4.713,4.681,4.643,4.632,4.599,4.566,4.508,4.41,4.95326,4.9407,4.8988,4.8727,4.855,4.822,4.796,4.785,4.768,4.738,4.715,4.594,4.589,4.581,4.512,4.459,4.445,4.423,4.363,6.03801,6.0088,5.9894,5.97772,5.9597,5.916,5.9099,5.8666,5.8075,5.7955,5.7623,5.75687,5.7281,5.7132,5.7088,5.7008,5.693,5.6743,5.668,5.6402,5.622,5.6133,5.6006,5.5859,5.5317,5.5099,5.4804,5.4586,5.4087,5.365,5.3359,5.3218,5.2643,5.2483,5.2292,5.2102,5.1937,5.1806,5.171,5.1465,5.1287,5.1105,5.0835,5.0561,5.0335,5.8713,5.8575,5.747,5.71623,5.60673,5.5398,5.5016,5.4812,5.4336,5.3656,5.3387,5.2873,5.2828,5.2588,5.2363,5.2116,5.1728,5.1518,5.1348,5.1124,5.0862,5.056,5.036,5.0255,5.0144,6.208,6.8191,6.5526,6.53,6.425,6.311,6.223,6.158,6.124,6.1,6,5.958,5.944,5.9,5.764,5.744,7.3861,6.954,6.819,6.802,6.754,6.671,6.636,6.589,6.519,8.026,7.61,6.6229,6.2782,5.2994,7.4503,6.8915,6.5683,5.8482};
  G4double Ia_U235[np_U235]={4.77,3.82,1.28,0.24,3.09,57.73,18.92,3.52,0.009,0.11,0.22,0.07,0.9,6.01,0.297,0.016,0.007,11,2.5,20,25.4,1.4,0.4,22.8,3,0.002,1.4,0.183,8.4,1.268,1.5,0.082,0.075,0.0153,0.0019,0.00365,0.0127,0.65826,0.54648,0.001518,0.08694,0.0828,0.000966,0.0138,0.001104,0.02484,0.001242,0.00552,0.000276,0.000138,4.14e-05,4.14e-05,6.9e-05,0.00069,8.28e-05,4.14e-05,23.866,2.85998,0.0019724,23.1757,2.9586,0.764305,0.171599,2.3866,1.25247,0.306708,0.224854,20.1185,0.033728,4.82252,8.18546,3.57991,1.4793,0.0564106,2.03157,0.017653,0.0069034,0.213019,0.167654,0.173571,0.0207102,0.0163709,0.00118344,0.00266274,0.000433928,0.000650892,0.00019724,0.000236688,0.00256412,0.00315584,0.00966476,0.0069034,0.00374756,0.00118344,0.00167654,0.00404342,0.000611444,0.000276136,0.00014793,0.000226826,0.000305722,0.999999,0.31,8.99999,51.6,25.2,8.99999,0.999999,0.00799999,2.22,0.13,0.13,0.15,0.0929999,0.042,0.041,0.0053,0.026,0.021,0.0017,0.0006,0.0003,0.0002,0.0004,0.000629999,0.00044,8.0316e-05,79.3999,12.9,0.11,7.49999,0.051,0.0043,0.018,0.000629999,0.0012,0.0031,0.00032,0.002,9.99999e-05,0.000919999,8.99999e-05,99.9998,0.0599999,0.000399999,0.0016,0.000799998,0.000799998,0.000299999,0.002,0.000299999,0.000229885,1.15e-07,83.5388,16.1852,0.00189476,0.273008,0.00150696,0.00148212,2.2356e-06};
  //-------------------------------------
  G4int const np_U238=47;
  G4double Ea_U238[np_U238]={4.198,4.151,4.038,4.7746,4.7224,4.6035,4.2773,4.1506,4.1086,4.687,4.6205,4.4798,4.4384,4.3718,4.2783,4.2485,3.8778,3.8294,4.78434,4.601,4.34,4.191,4.16,5.48948,4.986,4.826,6.00235,5.181,6.756,6.693,6.653,7.1292,6.5311,5.516,5.452,5.273,5.184,5.023,4.941,7.68682,6.9022,6.6098,3.72,4.694,4.656,5.30433,4.51658};
  G4double Ia_U238[np_U238]={79,20.9,0.078,71.38,28.42,0.2,4e-05,2.6e-05,7e-06,76.3,23.4,0.12,0.03,0.00097,8e-06,1.03e-05,3.4e-06,1.4e-06,93.84,6.16,0.0065,0.001,0.00027,99.92,0.078,0.0005,99.9789,0.00109978,0.00071928,0.017982,0.00127872,1.9974e-05,2.54e-08,0.009408,0.012936,0.001392,0.0001464,5.04e-05,6e-05,99.9655,0.0103975,5.99856e-05,1.9e-06,5.2e-05,7.8e-05,99.9999,0.00104};
  //==============================================================================================

  G4int newNSourceEnergies=0;
  G4double newSourceEnergy[10000];
  G4double newSourceIntensity[10000];
  for(G4int i=0;i<NSourceEnergies;i++){
    if(SourceEnergy[i]>0){
      newSourceEnergy[newNSourceEnergies]=SourceEnergy[i];
      newSourceIntensity[newNSourceEnergies]=SourceIntensity[i];
      newNSourceEnergies++;
    }
    else if(SourceEnergy[i]>-1.01 && SourceEnergy[i]<-0.99){//Th232
      G4double renorm=0;
      for(G4int j=0;j<np_Th232;j++){
	renorm+=Ia_Th232[j];
      }
      for(G4int j=0;j<np_Th232;j++){
	newSourceEnergy[newNSourceEnergies]=Ea_Th232[j]*MeV;
	newSourceIntensity[newNSourceEnergies]=Ia_Th232[j]/renorm*SourceIntensity[i];
	newNSourceEnergies++;
      }
    }
    else if(SourceEnergy[i]>-2.01 && SourceEnergy[i]<-1.99){//U235
      G4double renorm=0;
      for(G4int j=0;j<np_U235;j++){
	renorm+=Ia_U235[j];
      }
      for(G4int j=0;j<np_U235;j++){
	newSourceEnergy[newNSourceEnergies]=Ea_U235[j]*MeV;
	newSourceIntensity[newNSourceEnergies]=Ia_U235[j]/renorm*SourceIntensity[i];
	newNSourceEnergies++;
      }
    }
    else if(SourceEnergy[i]>-3.01 && SourceEnergy[i]<-2.99){//U238
      G4double renorm=0;
      for(G4int j=0;j<np_U238;j++){
	renorm+=Ia_U238[j];
      }
      for(G4int j=0;j<np_U238;j++){
	newSourceEnergy[newNSourceEnergies]=Ea_U238[j]*MeV;
	newSourceIntensity[newNSourceEnergies]=Ia_U238[j]/renorm*SourceIntensity[i];
	newNSourceEnergies++;
      }
    }
    else{
      G4cout<<" ######### Error in "<<__FILE__<<", line "<<__LINE__<<" #########"<<G4endl;  exit(1);
    }
  }
  
  delete [] SourceEnergy;
  delete [] SourceSigma;
  delete [] SourceIntensity;
  NSourceEnergies=newNSourceEnergies;
  SourceEnergy=new G4double[NSourceEnergies];
  SourceSigma=new G4double[NSourceEnergies];
  SourceIntensity=new G4double[NSourceEnergies];
  for(G4int i=0;i<NSourceEnergies;i++){
    SourceEnergy[i]=newSourceEnergy[i];
    SourceSigma[i]=-1;
    SourceIntensity[i]=newSourceIntensity[i];
  }
  
}
