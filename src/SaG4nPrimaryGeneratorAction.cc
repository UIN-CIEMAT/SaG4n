
#include "SaG4nPrimaryGeneratorAction.hh"
#include "G4SystemOfUnits.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "Randomize.hh"


SaG4nPrimaryGeneratorAction::SaG4nPrimaryGeneratorAction(SaG4nInputManager* anInputManager){

  theInputManager=anInputManager;
  G4int n_particle = 1;
  fParticleGun  = new G4ParticleGun(n_particle);
  G4ParticleDefinition* particle = G4ParticleTable::GetParticleTable()->FindParticle("alpha");
  fParticleGun->SetParticleDefinition(particle);
  AlreadyInit=false;
  TrX=0; TrY=0; TrZ=0;
  dir_ux=0; dir_uy=0; dir_uz=1;
}


SaG4nPrimaryGeneratorAction::~SaG4nPrimaryGeneratorAction()
{
  delete fParticleGun;
}


void SaG4nPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent){

  Init();

  //-----------------------------------------------------------------
  //Energy:
  G4double parEnergy=0;
  G4double rand=G4UniformRand();
  for(G4int i=0;i<NSourceEnergies;i++){
    if(rand<=SourceCumulProb[i]){
      if(SourceSigma[i]>0){
	parEnergy=G4RandGauss::shoot(SourceEnergy[i],SourceSigma[i]);
      }
      else{
	parEnergy=SourceEnergy[i];
      }
      break;
    }
  }
  fParticleGun->SetParticleEnergy(parEnergy);
  //-----------------------------------------------------------------


  //-----------------------------------------------------------------
  //Position:
  G4double Xpos=0,Ypos=0,Zpos=0,Xpos0=0,Ypos0=0,Zpos0=0;
  if(SourcePostType==-1){ //beam mode
    G4double PosRad=SourcePosParameters[6];
    G4double PosSigma=SourcePosParameters[7];
    if(PosSigma<=0){//uniform in a disk with radius r
      G4double radius=PosRad*sqrt(G4UniformRand()); //uniform
      G4double phi = 2.*3.1415926*G4UniformRand();
      Xpos0=radius*cos(phi);
      Ypos0=radius*sin(phi);
      Zpos0=0;
    }
    else{//gaussian
      do{
        Xpos0=G4RandGauss::shoot(0.,PosSigma);
        Ypos0=G4RandGauss::shoot(0.,PosSigma);
      }while(Xpos0*Xpos0+Ypos0*Ypos0>PosRad*PosRad);
      Zpos0=0;
    }
  }
  else if(SourcePostType==1){ // Sphere - Radius, VX, VY, VZ 
    G4double radius=SourcePosParameters[0]*pow(G4UniformRand(),1./3.);
    G4double phi = 2.*3.1415926*G4UniformRand();
    G4double costheta = 1-2.*G4UniformRand();
    G4double sintheta=sqrt(1.-costheta*costheta);
    Xpos0=radius*sintheta*cos(phi);
    Ypos0=radius*sintheta*sin(phi);
    Zpos0=radius*costheta;
  }
  else if(SourcePostType==2){ // Box - DX, DY, DZ, VX, VY, VZ
    Xpos0=-SourcePosParameters[0]*(-0.5+G4UniformRand());
    Ypos0=-SourcePosParameters[1]*(-0.5+G4UniformRand());
    Zpos0=-SourcePosParameters[2]*(-0.5+G4UniformRand());
  }
  else if(SourcePostType==3){ // Cylinder - Radius, Length, VX, VY, VZ
    G4double radius=SourcePosParameters[0]*sqrt(G4UniformRand());
    G4double phi = 2.*3.1415926*G4UniformRand();
    Xpos0=radius*cos(phi);
    Ypos0=radius*sin(phi);
    Zpos0=SourcePosParameters[1]*(-0.5+G4UniformRand());
  }
  else{
    G4cout<<" ******* Error:  SourcePostType = "<<SourcePostType<<" not yet defined *******"<<G4endl; exit(1);
  }
  Xpos=Xpos0+TrX;
  Ypos=Ypos0+TrY;
  Zpos=Zpos0+TrZ;

  fParticleGun->SetParticlePosition(G4ThreeVector(Xpos,Ypos,Zpos));
  //-----------------------------------------------------------------

  //-----------------------------------------------------------------
  //Momentum direction:
  if(SourcePostType==-1){ //beam mode
    G4ThreeVector sourcedirection(dir_ux,dir_uy,dir_uz);
   fParticleGun->SetParticleMomentumDirection(sourcedirection);
  }
  else{ //isotropic source
    G4double phi = 2.*3.1415926*G4UniformRand();
    G4double costheta = 1-2.*G4UniformRand();
    G4double sintheta=sqrt(1.-costheta*costheta);
    G4ThreeVector sourcedirection(cos(phi)*sintheta,sin(phi)*sintheta,costheta);
    fParticleGun->SetParticleMomentumDirection(sourcedirection);
  }
  //-----------------------------------------------------------------

  fParticleGun->GeneratePrimaryVertex(anEvent);

}


void SaG4nPrimaryGeneratorAction::Init(){

  if(AlreadyInit){return;}
  AlreadyInit=true;
  theInputManager->ReadInput();

  //==========================================================================================================
  // Position: SourcePostType=-1 --> "beam mode"
  //           SourcePostType=0 means uniformly distributed in a volume
  //           SourcePostType=1,2,3 --> means uniformly distributed in a spehere, parallelepiped, cylinder
  SourcePostType=theInputManager->GetSourcePostType();

  //---------------------------------------------------------------------------------------
  if(SourcePostType==-1){ //beam mode
    NSourcePosParameters=theInputManager->GetNSourcePosParameters();
    for(G4int i=0;i<NSourcePosParameters;i++){
      SourcePosParameters[i]=theInputManager->GetSourcePosParameter(i);
    }
    TrX=SourcePosParameters[0]; TrY=SourcePosParameters[1]; TrZ=SourcePosParameters[2];
    dir_ux=SourcePosParameters[3]/cm; dir_uy=SourcePosParameters[4]/cm; dir_uz=SourcePosParameters[5]/cm;
  }
  else if(SourcePostType==0){ //then the source is uniformly distributed in a volume 
    G4int SourceVolumeID=theInputManager->GetNSourcePosParameters();
    int sMotherID;
    double vx=0,vy=0,vz=0;
    for(G4int j=0;j<theInputManager->GetNVolumes();j++){
      if(SourceVolumeID==theInputManager->GetVolID(j)){
	SourcePostType=theInputManager->GetVolType(j);
	sMotherID=theInputManager->GetMotherID(j);
	theInputManager->GetVolumeTrVector(j,vx,vy,vz);
	for(G4int i=0;i<theInputManager->GetNVolParam(j);i++){
	  SourcePosParameters[i]=theInputManager->GetVolParam(j,i);
	}
	break;
      }
    }
    if(SourcePostType<=0){
      G4cout<<" ######### Error in "<<__FILE__<<", line "<<__LINE__<<" #########"<<G4endl;  exit(1);
    }
    //------------------------
    while(sMotherID!=0){
      bool MotherFound=false;
      for(G4int j=0;j<theInputManager->GetNVolumes();j++){
	if(sMotherID==theInputManager->GetVolID(j)){
	  sMotherID=theInputManager->GetMotherID(j);
	  double tmp_vx,tmp_vy,tmp_vz;
	  theInputManager->GetVolumeTrVector(j,tmp_vx,tmp_vy,tmp_vz);
	  vx+=tmp_vx;
	  vy+=tmp_vy;
	  vz+=tmp_vz;
	  MotherFound=true;
	  break;
	}
      }
      if(!MotherFound){
	G4cout<<" ######### Error in "<<__FILE__<<", line "<<__LINE__<<" #########"<<G4endl;  exit(1);
      }
    }
    //------------------------
    TrX=vx;
    TrY=vy;
    TrZ=vz;
  }
  //---------------------------------------------------------------------------------------
  else{ // then the source position is uniformly distributed in a spehere, parallelepiped, cylinder
    NSourcePosParameters=theInputManager->GetNSourcePosParameters();
    for(G4int i=0;i<NSourcePosParameters;i++){
      SourcePosParameters[i]=theInputManager->GetSourcePosParameter(i);
    }
    if(SourcePostType==1){
      TrX=SourcePosParameters[1]; TrY=SourcePosParameters[2]; TrZ=SourcePosParameters[3];
    }
    else if(SourcePostType==2){
      TrX=SourcePosParameters[3]; TrY=SourcePosParameters[4]; TrZ=SourcePosParameters[5]; 
   }
    else if(SourcePostType==3){
      TrX=SourcePosParameters[2]; TrY=SourcePosParameters[3]; TrZ=SourcePosParameters[4]; 
    }
    else{
      G4cout<<" ######### Error in "<<__FILE__<<", line "<<__LINE__<<" #########"<<G4endl;  exit(1);
    }
  }
  //---------------------------------------------------------------------------------------
  //==========================================================================================================


  //==========================================================================================================
  //Normalization 
  SourceNormFactor=theInputManager->GetSourceNormFactor();
  NSourceEnergies=theInputManager->GetNSourceEnergies();
  SourceEnergy=new G4double[NSourceEnergies];
  SourceSigma=new G4double[NSourceEnergies];
  SourceCumulProb=new G4double[NSourceEnergies];
  G4double CalculatedNormFactor=0;
  for(G4int i=0;i<NSourceEnergies;i++){
    SourceEnergy[i]=theInputManager->GetSourceEnergy(i);
    SourceSigma[i]=theInputManager->GetSourceSigma(i);
    SourceCumulProb[i]=theInputManager->GetSourceIntensity(i);
    CalculatedNormFactor+=SourceCumulProb[i];
  }
  for(G4int i=1;i<NSourceEnergies;i++){
    SourceCumulProb[i]+=SourceCumulProb[i-1];
  }
  for(G4int i=0;i<NSourceEnergies;i++){
    SourceCumulProb[i]/=SourceCumulProb[NSourceEnergies-1];
  }

  if(SourceNormFactor<=0){
    SourceNormFactor=CalculatedNormFactor;
  }

  theInputManager->SetNormalizationFactor(SourceNormFactor);
  //==========================================================================================================

  /*
  G4cout<<" ======================================================================= "<<G4endl;
  G4cout<<"                        SOURCE ENERGY DISTRIBUTION                       "<<G4endl;
  G4cout<<" ======================================================================= "<<G4endl;
  G4cout<<" Number of initial alpha energies: "<<NSourceEnergies<<G4endl;
  G4cout<<std::setw(15)<<"Energy (MeV)"<<std::setw(15)<<"Sigma (MeV)"<<std::setw(15)<<"Intensity"<<G4endl;
  G4cout<<" ----------------------------------------------------------------------- "<<G4endl;
  G4cout<<std::setw(15)<<SourceEnergy[0]<<std::setw(15)<<SourceSigma[0]<<std::setw(15)<<SourceCumulProb[0]*SourceNormFactor<<G4endl;
  for(G4int i=1;i<NSourceEnergies;i++){
    G4cout<<std::setw(15)<<SourceEnergy[i]<<std::setw(15)<<SourceSigma[i]<<std::setw(15)<<(SourceCumulProb[i]-SourceCumulProb[i-1])*SourceNormFactor<<G4endl;
  }
  G4cout<<" ======================================================================= "<<G4endl;
  */
}














