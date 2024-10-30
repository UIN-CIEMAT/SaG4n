#ifndef SAG4NEVENTACTION_HH
#define SAG4NEVENTACTION_HH 1


#include "G4UserEventAction.hh"
#include "globals.hh"
#include <ctime>
#include "g4root.hh"
#include "SaG4nInputManager.hh"

class SaG4nEventAction : public G4UserEventAction
{
public:
  SaG4nEventAction(SaG4nInputManager* anInputManager);
  ~SaG4nEventAction();

  void Init();

  void  BeginOfEventAction(const G4Event*);
  void  EndOfEventAction(const G4Event*);

  void AddSecondaryParticle(G4String parname,G4double Energy,G4double weight,G4int VolumeID);
  void AddFlux(G4double EneUp,G4double EneLow,G4double StepLength,G4double weight,G4int VolumeID);
  void WriteResults();

  G4int GetEventNumber(){return NEvents;}
  void PrintProgress();

private:

  G4int OutputType1;
  G4int OutputFormat;

  G4bool AlreadyInit;
  SaG4nInputManager* theInputManager;
  G4int NEvents;
  G4double NormFactor;

  G4int NVolumes;

  //-------------------------------------------------------------
  G4double* NeutronSpectra_OneEvent[G4AN_MAXNVOLUMES]; //Neutron spectra in one event
  G4double* AlphaFlux_OneEvent[G4AN_MAXNVOLUMES]; //Alpha Flux in one event
  G4double* NeutronSpectra_Sum[G4AN_MAXNVOLUMES]; //Sum values of the neutron spectra
  G4double* AlphaFlux_Sum[G4AN_MAXNVOLUMES]; //Sum values of the alpha flux 
  G4int* NeutronSpectra_NEntries[G4AN_MAXNVOLUMES]; //Number of entries in Sum and Sum2 spectra
  G4double* NeutronSpectra_Sum2[G4AN_MAXNVOLUMES]; //Sum^2 values of the neutron spectra
  G4double* AlphaFlux_Sum2[G4AN_MAXNVOLUMES]; //Sum^2 values of the alpha flux 
  G4int* AlphaFlux_NEntries[G4AN_MAXNVOLUMES]; //Number of entries in Sum and Sum2 spectra
  G4int NeutronSpectra_NFilled[G4AN_MAXNVOLUMES]; //Number of entries in the event
  G4int AlphaFlux_NFilled[G4AN_MAXNVOLUMES]; //Number of entries in the event
  //-------------------------------------------------------------

  G4int H_NBins;
  G4double H_MaxEne;
  
  time_t starttime;
  G4double NextPrint_minutes;
};




#endif

