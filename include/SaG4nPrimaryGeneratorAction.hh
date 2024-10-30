#ifndef SAG4NPRIMARYGENERATORACTION_HH
#define SAG4NPRIMARYGENERATORACTION_HH 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"
#include "SaG4nInputManager.hh"

class G4ParticleGun;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class SaG4nPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
  SaG4nPrimaryGeneratorAction(SaG4nInputManager* anInputManager);    

  virtual ~SaG4nPrimaryGeneratorAction();
  virtual void GeneratePrimaries(G4Event* anEvent);

  void Init();
  G4double GetSourceNormFactor(){return SourceNormFactor;}

  G4ParticleGun* GetParticleGun(){return fParticleGun;}

private:
  G4ParticleGun*           fParticleGun;         //pointer a to G4  class

  SaG4nInputManager* theInputManager;
  G4bool AlreadyInit;

  G4int SourcePostType,NSourcePosParameters;
  G4double SourcePosParameters[100];
  G4double TrX,TrY,TrZ;
  G4double dir_ux,dir_uy,dir_uz;
  G4double SourceNormFactor;
  G4int NSourceEnergies;
  G4double* SourceEnergy;
  G4double* SourceSigma;
  G4double* SourceCumulProb;


};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......



#endif







