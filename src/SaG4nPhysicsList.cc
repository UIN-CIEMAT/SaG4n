
#include "SaG4nPhysicsList.hh"

#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

#include "G4EmStandardPhysics_option4.hh"
#include "G4EmExtraPhysics.hh"
#include "G4DecayPhysics.hh"
#include "G4HadronPhysicsQGSP_BIC_HP.hh"
#include "G4HadronElasticPhysicsPHP.hh"
#include "G4StoppingPhysics.hh"
#include "G4IonPhysicsPHP_new.hh"
#include "G4RadioactiveDecayPhysics.hh"
#include "G4StepLimiterPhysics.hh"
#include "G4HadronicProcessStore.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SaG4nPhysicsList::SaG4nPhysicsList()
:G4VModularPhysicsList()
{

  G4int ver = 0;
  defaultCutValue = 0.7*CLHEP::mm;  
  SetVerboseLevel(ver);


  // EM physics
  RegisterPhysics(new G4EmStandardPhysics_option4(ver));

  // Synchroton Radiation & GN Physics
  //RegisterPhysics( new G4EmExtraPhysics(ver) );

  // Decays
  //RegisterPhysics( new G4DecayPhysics(ver) );

  // Hadron Elastic scattering
  RegisterPhysics( new G4HadronElasticPhysicsPHP(ver) ); // NeutronElastic process !

  // Hadron Physics
  RegisterPhysics( new G4HadronPhysicsQGSP_BIC_HP(ver));

  // Stopping Physics
  //RegisterPhysics( new G4StoppingPhysics(ver) );

  // Ion Physics
  RegisterPhysics( new G4IonPhysicsPHP_new(ver));

  // Radioactive decay
  RegisterPhysics(new G4RadioactiveDecayPhysics(ver));

  //step limit
  RegisterPhysics( new G4StepLimiterPhysics(ver) );

  if(G4HadronicProcessStore::Instance()){
    G4HadronicProcessStore::Instance()->SetVerbose(0);
  }
}

SaG4nPhysicsList::~SaG4nPhysicsList()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SaG4nPhysicsList::SetCuts()
{
  SetCutValue(defaultCutValue, "proton");
  SetCutValue(defaultCutValue, "e-");
  SetCutValue(defaultCutValue, "e+");
  SetCutValue(defaultCutValue, "gamma");      
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

