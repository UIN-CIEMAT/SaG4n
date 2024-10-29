#ifndef SAG4NPHYSICSLIST_HH
#define SAG4NPHYSICSLIST_HH 1

#include "G4VModularPhysicsList.hh"
#include "globals.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class SaG4nPhysicsList: public G4VModularPhysicsList
{
public:
  SaG4nPhysicsList();
 ~SaG4nPhysicsList();

public:
  virtual void SetCuts();
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif


