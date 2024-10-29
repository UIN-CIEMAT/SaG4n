
#include "SaG4nDetectorConstruction.hh"
#include "G4SystemOfUnits.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4UserLimits.hh"

#include "G4Sphere.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4UniformMagField.hh"
#include "G4LogicalVolumeStore.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"


#include "GB01BOptrMultiParticleChangeCrossSection.hh"

SaG4nDetectorConstruction::SaG4nDetectorConstruction(SaG4nInputManager* anInputManager){
  theInputManager=anInputManager;
  LogicName=0;
}

SaG4nDetectorConstruction::~SaG4nDetectorConstruction(){
  delete [] LogicName;
}



G4VPhysicalVolume* SaG4nDetectorConstruction::Construct(){

  theInputManager->ReadInput();
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
  G4bool pSurfChk=false;

  //--------------------------------------------------------------------------------------------------------
  //World:
  G4Material*   worldMaterial = G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic");
  G4double WorldSize=theInputManager->GetWorldSize();
  G4VSolid* solidWorld = new G4Box("World",WorldSize/2.,WorldSize/2.,WorldSize/2.);
  
  G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld,       // its solid
                                                    worldMaterial,    // its material
                                                    "World");         // its name
  
  G4PVPlacement* physiWorld = new G4PVPlacement(0,                    // no rotation
                                                G4ThreeVector(),      // at (0,0,0)
                                                logicWorld,           // its logical volume
                                                "World",              // its name
                                                0,                    // its mother volume
                                                false,                // no boolean operation
                                                0);                   // copy number

  logicWorld->SetVisAttributes(G4VisAttributes::GetInvisible());
  //--------------------------------------------------------------------------------------------------------


  NVolumes=theInputManager->GetNVolumes();
  LogicName=new G4String[NVolumes];
  //--------------------------------------------------------------------------------------------------------
  //Solid and logic volumes:
  G4VSolid** theSolid=new G4VSolid*[NVolumes];
  G4LogicalVolume** theLogic=new G4LogicalVolume*[NVolumes];
  G4String SolidName,VolName;
  for(G4int i=0;i<NVolumes;i++){
    VolName=theInputManager->GetVolName(i);
    SolidName=VolName;
    LogicName[i]=VolName;
    G4int VolType=theInputManager->GetVolType(i);
    G4int MatID=theInputManager->GetMatID(i);
    if(VolType==1){
      theSolid[i]=new G4Sphere(SolidName,0,theInputManager->GetVolParam(i,0),0,360*degree,0,180*degree);
    }
    else if(VolType==2){
      theSolid[i]=new G4Box(SolidName,theInputManager->GetVolParam(i,0)/2.,theInputManager->GetVolParam(i,1)/2.,theInputManager->GetVolParam(i,2)/2.);
    }
    else if(VolType==3){
      theSolid[i]=new G4Tubs(SolidName,0,theInputManager->GetVolParam(i,0),theInputManager->GetVolParam(i,1)/2.,0,360*degree);
    }
    else{
      G4cout<<" ******* Error: Volume type "<<VolType<<" not yet defined *******"<<G4endl; exit(1);
    }
    theLogic[i]=new G4LogicalVolume(theSolid[i],theInputManager->GetMaterial(MatID),LogicName[i]);
  }
  //--------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------
  //Physical volumes:
  G4String PhysiName;
  for(G4int i=0;i<NVolumes;i++){
    VolName=theInputManager->GetVolName(i);
    G4double vx=0,vy=0,vz=0;
    G4int VolType=theInputManager->GetVolType(i);
    if(VolType==1){vx=theInputManager->GetVolParam(i,1); vy=theInputManager->GetVolParam(i,2); vz=theInputManager->GetVolParam(i,3);}
    else if(VolType==2){vx=theInputManager->GetVolParam(i,3); vy=theInputManager->GetVolParam(i,4); vz=theInputManager->GetVolParam(i,5);}
    else if(VolType==3){vx=theInputManager->GetVolParam(i,2); vy=theInputManager->GetVolParam(i,3); vz=theInputManager->GetVolParam(i,4);}
    PhysiName=VolName;
    G4LogicalVolume* theMotherLogic=logicWorld;
    G4int MotherID=theInputManager->GetMotherID(i);
    for(G4int j=0;j<NVolumes;j++){
      if(MotherID==theInputManager->GetVolID(j)){
	theMotherLogic=theLogic[j];
	break;
      }
    }
    G4VPhysicalVolume* thePhysVol=new G4PVPlacement(0,G4ThreeVector(vx,vy,vz),theLogic[i],PhysiName,theMotherLogic,false,i+1,pSurfChk);
    G4bool Overlaps=thePhysVol->CheckOverlaps();
    if(Overlaps==true){
      G4cout<<G4endl<<G4endl;
      G4cout<<" ############# WARNING: An overlap in the geometry has been detected by Geant4, when placing volume "<<PhysiName<<" #############"<<G4endl;
      G4cout<<G4endl<<G4endl;
      if(!theInputManager->IsInteractive()){
	exit(1);
      }
    }
 }
  //--------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------
  G4double MaxStepSize=theInputManager->GetMaxStepSize();

  if(MaxStepSize>0){
    G4UserLimits* fStepLimit = new G4UserLimits(MaxStepSize);
    for(G4int i=0;i<NVolumes;i++){
      theLogic[i]->SetUserLimits(fStepLimit);
    }
  }
  //--------------------------------------------------------------------------------------------------------


  delete [] theSolid;
  delete [] theLogic;

  return physiWorld;
}




void SaG4nDetectorConstruction::ConstructSDandField()
{

  G4double BiasFactor=theInputManager->GetBiasFactor();

  if(BiasFactor<=0){
    return;
  }
  G4String biasedParticleName="alpha";

  // ----------------------------------------------
  // -- operator creation and attachment to volume:
  // ----------------------------------------------
  GB01BOptrMultiParticleChangeCrossSection* testMany = new GB01BOptrMultiParticleChangeCrossSection();
  testMany->AddParticle(biasedParticleName,BiasFactor);

  for(G4int i=0;i<NVolumes;i++){
    // -- Fetch volume for biasing:
    G4LogicalVolume* logicTest = G4LogicalVolumeStore::GetInstance()->GetVolume(LogicName[i]);
    testMany->AttachTo(logicTest); // La reducci<F3>n de varianza se hace en estos vol<FA>menes l<F3>gicos
    G4cout << " Attaching biasing operator to logical volume " << logicTest->GetName()<<" with biasFactor = "<<BiasFactor<<" in "<<biasedParticleName<<G4endl;
  }
  
}













