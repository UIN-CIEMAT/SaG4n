#ifndef SAG4NINPUTMANAGER_HH
#define SAG4NINPUTMANAGER_HH 1

#include "G4Material.hh"

#define G4AN_MAXNMATERIALS 100
#define G4AN_MAXNVOLUMES 100

class SaG4nInputManager{

public:

  SaG4nInputManager(const char* infpname);
  ~SaG4nInputManager();

  void ReadInput();
  
  G4String GetInputFname(){return InputFname;}

  G4double GetWorldSize(){return WorldSize;}
  G4int GetNVolumes(){return NVolumes;}
  G4String GetVolName(G4int id){return VolName[id];}
  G4int GetVolID(G4int id){return VolID[id];}
  G4int GetMatID(G4int id){return MatID[id];}
  G4int GetVolType(G4int id){return VolType[id];}
  G4int GetMotherID(G4int id){return MotherID[id];}
  G4int GetNVolParam(G4int id){return NVolParam[id];}
  G4double GetVolParam(G4int id,G4int j){return VolParam[id][j];}
  G4Material* GetMaterial(G4int mat_id){return theDefinedMaterials[mat_id];}
  G4double GetVolumeSize(G4int id){return VolumeSize[id];}
  void GetVolumeTrVector(G4int id,G4double& vx,G4double& vy,G4double& vz);

  G4double GetMaxStepSize(){return MaxStepSize;}
  G4double GetBiasFactor(){return BiasFactor;}
  G4int GetNEvents(){return NEvents;}
  G4int GetOutputType1(){return OutputType1;}
  G4int GetOutputType2(){return OutputType2;}
  G4int GetOutputFormat(){return OutputFormat;}
  G4String GetOutFname(){return OutFname;}
  void SetNormalizationFactor(G4double val){NormalizationFactor=val;}
  G4double GetNormalizationFactor(){return NormalizationFactor;}
  G4bool IsInteractive(){return Interactive;}
  G4bool DoKillSecondaries(){return KillSecondaries;}
  G4int GetSeed(){return Seed;}

  G4int GetSourcePostType(){return SourcePostType;}
  G4int GetNSourcePosParameters(){return NSourcePosParameters;}
  G4double GetSourcePosParameter(G4int id){return SourcePosParameters[id];}
  G4double GetSourceNormFactor(){return SourceNormFactor;}
  G4int GetNSourceEnergies(){return NSourceEnergies;}
  G4double GetSourceEnergy(G4int id){return SourceEnergy[id];}
  G4double GetSourceSigma(G4int id){return SourceSigma[id];}
  G4double GetSourceIntensity(G4int id){return SourceIntensity[id];}

private:
  G4Material* CreateMaterialFromIsotopes(G4String MatName,G4int nIsotopes,G4double density,G4int* ZA,G4double* atomicfraction);
  G4Material* CreateMaterialFromElements(G4String MatName,G4int nElements,G4double density,G4int* Element_Z,G4double* elementfraction); // if elementfraction[i]>0 then it is atom fraction; if elementfraction[i]<0 then it is mass fraction
  void ComputeSourceIntensities();

private:
  G4String InputFname;
  G4String SaG4nDatabaseDir;
  G4bool InputAlreadyRead;

  G4double WorldSize;
  G4int NVolumes;
  G4String VolName[G4AN_MAXNVOLUMES];
  G4int VolID[G4AN_MAXNVOLUMES],MatID[G4AN_MAXNVOLUMES],VolType[G4AN_MAXNVOLUMES],MotherID[G4AN_MAXNVOLUMES];
  G4double VolumeSize[G4AN_MAXNVOLUMES];
  G4int NVolParam[G4AN_MAXNVOLUMES];
  G4double VolParam[G4AN_MAXNVOLUMES][10];
  G4Material* theDefinedMaterials[G4AN_MAXNMATERIALS];

  G4double MaxStepSize;
  G4double BiasFactor;
  G4int NEvents;
  G4int OutputType1,OutputType2,OutputFormat;
  G4String OutFname;
  G4double NormalizationFactor;
  G4bool Interactive;
  G4bool KillSecondaries;
  G4int Seed;

  G4int SourcePostType,NSourcePosParameters;
  G4double SourcePosParameters[100];
  G4double SourceNormFactor;
  G4int NSourceEnergies;
  G4String* SourceName;
  G4double* SourceEnergy;
  G4double* SourceSigma;
  G4double* SourceIntensity;
};

#endif




