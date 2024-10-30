
#include "G4RunManager.hh"

#include "G4UImanager.hh"

#include "SaG4nInputManager.hh"
#include "SaG4nDetectorConstruction.hh"
#include "SaG4nPhysicsList.hh"
#include "SaG4nPrimaryGeneratorAction.hh"
#include "SaG4nActionInitialization.hh"

#include "G4SystemOfUnits.hh"
#include "G4GenericBiasingPhysics.hh"
#include "G4StepLimiterPhysics.hh"
#include "G4ParticleHPManager.hh"

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include "G4UIExecutive.hh"

#include <ctime>

int main(int argc,char** argv){

  if(argc!=2){
    G4cout<<" ########################################################################"<<G4endl;
    G4cout<<" Input has to be like --->  ./SaG4n InputFname   "<<G4endl;
    G4cout<<" ########################################################################"<<G4endl;
    return 1;
  }

  time_t starttime=time(NULL);

  G4RunManager * runManager = new G4RunManager();
  SaG4nInputManager* theInputManager=new SaG4nInputManager(argv[1]);

  SaG4nDetectorConstruction* theDetectorConstruction=new SaG4nDetectorConstruction(theInputManager);
  runManager->SetUserInitialization(theDetectorConstruction);

  SaG4nPhysicsList* thePhysicsList=new SaG4nPhysicsList();
  G4GenericBiasingPhysics* biasingPhysics = new G4GenericBiasingPhysics();
  biasingPhysics->Bias("alpha");
  thePhysicsList->RegisterPhysics(biasingPhysics);
  runManager->SetUserInitialization(thePhysicsList);


  SaG4nActionInitialization* theActionInitialization=new SaG4nActionInitialization(theInputManager);
  runManager->SetUserInitialization(theActionInitialization);

  // set HP physics variables (formerly environmental variables) as required by SaG4n; others are default values so commented out
  G4ParticleHPManager::GetInstance()->SetSkipMissingIsotopes( true );
  G4ParticleHPManager::GetInstance()->SetDoNotAdjustFinalState( true );
  //G4ParticleHPManager::GetInstance()->SetUseOnlyPhotoEvaporation( false );
  //G4ParticleHPManager::GetInstance()->SetNeglectDoppler( false );
  //G4ParticleHPManager::GetInstance()->SetProduceFissionFragments( false );
  //G4ParticleHPManager::GetInstance()->SetUseWendtFissionModel( false );
  //G4ParticleHPManager::GetInstance()->SetUseNRESP71Model( false );

  runManager->Initialize();

  G4VisManager* visManager = new G4VisExecutive("quiet");
  visManager->Initialize();


  G4bool Interactive=theInputManager->IsInteractive();

  if(Interactive){
    G4UIExecutive* ui = new G4UIExecutive(argc,argv);
    ui->SessionStart();
    delete ui;
  }
  else{
    runManager->BeamOn(theInputManager->GetNEvents());
  }

  delete visManager;
  delete runManager;

  G4cout<<" Job with input "<<argv[1]<<" finished successfully after "<<difftime(time(NULL),starttime)<<" seconds"<<G4endl;

  return 0;
}










