
#include "G4RunManager.hh"
#include "SaG4nStackingAction.hh"
#include "G4SystemOfUnits.hh"
#include "G4SaG4nParticleHPManager.hh"
#include "G4ParticleGun.hh"

SaG4nStackingAction::SaG4nStackingAction(SaG4nInputManager* anInputManager){

  theInputManager=anInputManager;
  theEventAction=(SaG4nEventAction*)G4RunManager::GetRunManager()->GetUserEventAction();
  SaG4nPrimaryGeneratorAction* thePrimaryGeneratorAction=(SaG4nPrimaryGeneratorAction*)G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction();
  theParticleGun=thePrimaryGeneratorAction->GetParticleGun();
  AlreadyInit=false;
}


SaG4nStackingAction::~SaG4nStackingAction(){

  if(out.is_open()){
    out.close();
  }

}


G4ClassificationOfNewTrack SaG4nStackingAction::ClassifyNewTrack(const G4Track* aTrack){

  Init();

  if(aTrack->GetParentID()>0){
    G4String parName=aTrack->GetDynamicParticle()->GetDefinition()->GetParticleName();
    G4double parEnergy=aTrack->GetDynamicParticle()->GetKineticEnergy();
    G4double parWeight=aTrack->GetWeight();
    G4int VolID=aTrack->GetVolume()->GetCopyNo()-1;


    if(VolID>=0){
      theEventAction->AddSecondaryParticle(parName,parEnergy,parWeight,VolID);
    }
    //======================================================================================
    //Write results, if OutputType2==1 or OutputType2==2
    if((OutputType2==1 || OutputType2==2) && (parName=="neutron" || parName=="gamma")){
      G4int EventNumer=theEventAction->GetEventNumber();
      G4double pos_x=aTrack->GetPosition().x();
      G4double pos_y=aTrack->GetPosition().y();
      G4double pos_z=aTrack->GetPosition().z();
      G4double ux=aTrack->GetDynamicParticle()->GetMomentumDirection().x();
      G4double uy=aTrack->GetDynamicParticle()->GetMomentumDirection().y();
      G4double uz=aTrack->GetDynamicParticle()->GetMomentumDirection().z();
      G4int CWidth=15;
      out<<std::setw(CWidth)<<EventNumer<<" "<<std::setw(CWidth)<<parName<<"  "<<std::setw(CWidth)<<parEnergy/MeV<<"  "<<std::setw(CWidth)<<parWeight<<"  "<<std::setw(CWidth)<<pos_x/cm<<"  "<<std::setw(CWidth)<<pos_y/cm<<"  "<<std::setw(CWidth)<<pos_z/cm<<"  "<<std::setw(CWidth)<<ux<<"  "<<std::setw(CWidth)<<uy<<"  "<<std::setw(CWidth)<<uz;
      out<<G4endl;
      if(OutputType2==2){//More info
        G4int TargetZ=G4SaG4nParticleHPManager::GetInstance()->GetZofLastInteraction(); //Z of the target nucleus
        G4int TargetA=G4SaG4nParticleHPManager::GetInstance()->GetAofLastInteraction(); //A of the target nucleus
	G4double AlphaKinEne=G4SaG4nParticleHPManager::GetInstance()->GetKinEneOfLastInteraction(); //Energy of the alpha at the interaction
	G4double source_energy=theParticleGun->GetParticleEnergy();
	G4double source_posX=theParticleGun->GetParticlePosition().x();
	G4double source_posY=theParticleGun->GetParticlePosition().y();
	G4double source_posZ=theParticleGun->GetParticlePosition().z();
	G4double source_uX=theParticleGun->GetParticleMomentumDirection().x();
	G4double source_uY=theParticleGun->GetParticleMomentumDirection().y();
	G4double source_uZ=theParticleGun->GetParticleMomentumDirection().z();
	out<<"  "<<std::setw(CWidth)<<TargetZ<<"  "<<std::setw(CWidth)<<TargetA<<"  "<<std::setw(CWidth)<<AlphaKinEne;
	out<<"  "<<std::setw(CWidth)<<source_energy/MeV<<"  "<<std::setw(CWidth)<<source_posX/cm<<"  "<<std::setw(CWidth)<<source_posY/cm<<"  "<<std::setw(CWidth)<<source_posZ/cm;
	out<<"  "<<std::setw(CWidth)<<source_uX<<"  "<<std::setw(CWidth)<<source_uY<<"  "<<std::setw(CWidth)<<source_uZ;
	out<<G4endl;
      }
    }
    //======================================================================================
    if(KillSecondaries){
      return fKill;
    }
  }

  return fUrgent;

}


void SaG4nStackingAction::Init(){

  if(AlreadyInit){return;}
  AlreadyInit=true;
  theInputManager->ReadInput();

  KillSecondaries=theInputManager->DoKillSecondaries();

  OutputType2=theInputManager->GetOutputType2();
  if(OutputType2==1 || OutputType2==2){
    OutFname=theInputManager->GetOutFname()+G4String(".out");
    out.open(OutFname);
    if(!out.good()){
      G4cout<<" ############ Error opening file "<<OutFname<<" (please set the appropriate path in the OUTPUTFILE field of the input file) ############"<<G4endl; exit(1);
    }
    G4int CWidth=15;
    out<<std::setw(CWidth)<<"EventNumber"<<" "<<std::setw(CWidth)<<"parName"<<"  "<<std::setw(CWidth)<<"parEnergy (MeV)"<<"  "<<std::setw(CWidth)<<"parWeight"<<"  "<<std::setw(CWidth)<<"pos_x (cm)"<<"  "<<std::setw(CWidth)<<"pos_y (cm)"<<"  "<<std::setw(CWidth)<<"pos_z (cm)"<<"  "<<std::setw(CWidth)<<"ux"<<"  "<<std::setw(CWidth)<<"uy"<<"  "<<std::setw(CWidth)<<"uz";
    out<<G4endl;
    if(OutputType2==2){//More info
      out<<"  "<<std::setw(CWidth)<<"TargetZ"<<"  "<<std::setw(CWidth)<<"TargetA"<<"  "<<std::setw(CWidth)<<"KinEne (MeV)";
      out<<"  "<<std::setw(CWidth)<<"s_energy (MeV)"<<"  "<<std::setw(CWidth)<<"s_pos_x (cm)"<<"  "<<std::setw(CWidth)<<"s_pos_y (cm)"<<"  "<<std::setw(CWidth)<<"s_pos_z (cm)";
      out<<"  "<<std::setw(CWidth)<<"s_ux"<<"  "<<std::setw(CWidth)<<"s_uy"<<"  "<<std::setw(CWidth)<<"s_uz";
      out<<G4endl;
    }
  }


}
