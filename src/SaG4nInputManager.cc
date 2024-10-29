
#include <fstream>
#include "G4ios.hh"
#include "G4Isotope.hh"
#include "G4Element.hh"
#include "G4Material.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

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
  SourceEnergy=0; SourceIntensity=0;

}

SaG4nInputManager::~SaG4nInputManager(){

  if(SourceEnergy){delete [] SourceEnergy;}
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
      G4double density,atomicfraction[1000];
      in>>matID>>MatName>>density>>nIsotopes;
      density*=g/cm3;
      if(matID>=G4AN_MAXNMATERIALS){
	G4cout<<" ******* Error reading input. matID max is "<<G4AN_MAXNMATERIALS-1<<" ---> matID = "<<matID<<" *******"<<G4endl; exit(1);
      }
      for(G4int i=0;i<nIsotopes;i++){
	in>>ZA[i]>>atomicfraction[i];
      }
      in>>word;
      if(word!="ENDMATERIAL"){
	G4cout<<" ****** Error reading material "<<matID<<" ******"<<G4endl;
	G4cout<<" ****** ERROR reading input: "<<InputFname<<" ******"<<G4endl; exit(1);
      }
      theDefinedMaterials[matID]=CreateMaterial(MatName,nIsotopes,density,ZA,atomicfraction);
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
      if(SourcePostType==0){ // source uniformly distributed inside a volume
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
      for(G4int i=0;i<NSourceEnergies;i++){
	in>>word>>SourceEnergy[i]>>SourceIntensity[i];
	SourceEnergy[i]*=MeV;
      }
      in>>word;
      if(word!="ENDSOURCE"){
	G4cout<<" ****** ERROR reading input: "<<InputFname<<" --> "<<word<<" (Source) ******"<<G4endl; exit(1);
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
     
}




G4Material* SaG4nInputManager::CreateMaterial(G4String MatName,G4int nIsotopes,G4double density,G4int* ZA,G4double* atomicfraction){

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
  G4Material* theMaterial=new G4Material(MatName, density,nElements,kStateUndefined,temperature);
  G4double fractionmass=0;
  for(G4int i=0;i<nElements;i++){
     theMaterial->AddElement(theElements[i],fractionmass=themassfractions[i]/totalmassfraction);
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



