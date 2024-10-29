#ifndef SAG4NSTACKINGACTION_HH
#define SAG4NSTACKINGACTION_HH 1

#include "G4UserStackingAction.hh"
#include "G4ios.hh"

#include "G4ios.hh"
#include "SaG4nEventAction.hh"
#include "SaG4nPrimaryGeneratorAction.hh"
#include "SaG4nInputManager.hh"


class SaG4nStackingAction : public G4UserStackingAction
{
public:

  SaG4nStackingAction(SaG4nInputManager* anInputManager);
  ~SaG4nStackingAction();
  G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track* aTrack);

  void Init();

private:
  G4bool AlreadyInit;
  SaG4nInputManager* theInputManager;
  G4int OutputType2;
  G4String OutFname;
  std::ofstream out;

  SaG4nEventAction* theEventAction;  
  G4ParticleGun* theParticleGun;  //to get info about the source
  G4bool KillSecondaries;
};




#endif





