#ifndef SAG4NDETECTORCONSTRUCTION_HH
#define SAG4NDETECTORCONSTRUCTION_HH 1

#include "G4VUserDetectorConstruction.hh"
#include "SaG4nInputManager.hh"
#include "globals.hh"


class SaG4nDetectorConstruction : public G4VUserDetectorConstruction
{
public:
  
  SaG4nDetectorConstruction(SaG4nInputManager* anInputManager);
  ~SaG4nDetectorConstruction();
  
public:
  
  virtual G4VPhysicalVolume* Construct();
  virtual void     ConstructSDandField();


private:
  SaG4nInputManager* theInputManager;
  G4int NVolumes;
  G4String* LogicName;
};

#endif

