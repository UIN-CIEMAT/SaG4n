
#include "SaG4nSteppingAction.hh"
#include "G4RunManager.hh"
#include "Randomize.hh"
#include "G4SystemOfUnits.hh"


SaG4nSteppingAction::SaG4nSteppingAction(){

  theEventAction=(SaG4nEventAction*)G4RunManager::GetRunManager()->GetUserEventAction();

}


SaG4nSteppingAction::~SaG4nSteppingAction(){

}


void SaG4nSteppingAction::UserSteppingAction(const G4Step* aStep){

  //Calculate the alpha flux:
  if(aStep->GetTrack()->GetParentID()==0){
    G4double ParticleEnergy_Up=aStep->GetPreStepPoint()->GetKineticEnergy();
    G4double ParticleEnergy_Low=0;
    if(aStep->GetPostStepPoint()){
      ParticleEnergy_Low=aStep->GetPostStepPoint()->GetKineticEnergy();
    }
    G4double StepLength=aStep->GetStepLength();
    G4double Weight=aStep->GetPreStepPoint()->GetWeight();
    G4int VolID=aStep->GetTrack()->GetVolume()->GetCopyNo()-1;
    if(VolID>=0){
      theEventAction->AddFlux(ParticleEnergy_Up,ParticleEnergy_Low,StepLength,Weight,VolID);
    }
  }

}
