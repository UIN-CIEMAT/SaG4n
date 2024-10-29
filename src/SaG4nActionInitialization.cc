
#include "SaG4nActionInitialization.hh"
#include "SaG4nPrimaryGeneratorAction.hh"
#include "SaG4nEventAction.hh"
#include "SaG4nSteppingAction.hh"
#include "SaG4nStackingAction.hh"


SaG4nActionInitialization::SaG4nActionInitialization(SaG4nInputManager* anInputManager){

  theInputManager=anInputManager;

}


SaG4nActionInitialization::~SaG4nActionInitialization(){


}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SaG4nActionInitialization::BuildForMaster() const
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SaG4nActionInitialization::Build() const
{

  SetUserAction(new SaG4nPrimaryGeneratorAction(theInputManager));
  SetUserAction(new SaG4nEventAction(theInputManager));
  SetUserAction(new SaG4nSteppingAction());
  SetUserAction(new SaG4nStackingAction(theInputManager));

}












