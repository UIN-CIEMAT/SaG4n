#ifndef SAG4NACTIONINITIALIZATION_HH
#define SAG4NACTIONINITIALIZATION_HH 1


#include "G4VUserActionInitialization.hh"
#include "globals.hh"
#include "SaG4nInputManager.hh"

class SaG4nActionInitialization : public G4VUserActionInitialization
{
public:
  SaG4nActionInitialization(SaG4nInputManager* anInputManager);
  virtual ~SaG4nActionInitialization();

  virtual void BuildForMaster() const;
  virtual void Build() const;

private:
  SaG4nInputManager* theInputManager;
};


#endif

