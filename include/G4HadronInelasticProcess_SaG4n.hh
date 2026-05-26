//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// Hadronic Inelastic Process class
// The specific particle inelastic processes derive from this class
// This is an abstract base class, since the pure virtual function
// PostStepDoIt has not been defined yet.
//
// J.L. Chuma, TRIUMF, 10-Mar-1997
// Last modified: 27-Mar-1997
//
// 14-APR-98 F.W.Jones: variant G4HadronInelastic process for
// G4CrossSectionDataSet/DataStore class design.
// 29-JUN-98 F.W.Jones: default data set G4HadronCrossSections
//

#ifndef G4HadronInelasticProcess_SaG4n_h
#define G4HadronInelasticProcess_SaG4n_h 1

#include "G4HadronicProcess.hh"

class G4ParticleDefinition;

class G4HadronInelasticProcess_SaG4n : public G4HadronicProcess
{
public:
    
  explicit G4HadronInelasticProcess_SaG4n(const G4String &processName,
                                    const G4ParticleDefinition* particleDef = nullptr );
    
  ~G4HadronInelasticProcess_SaG4n() override;
        
  G4bool IsApplicable(const G4ParticleDefinition&) override;
  inline const G4ParticleDefinition* GetParticleDefinition() const;

  //new methods:
  // compute step limit
  G4double PostStepGetPhysicalInteractionLength(const G4Track& track,
           G4double, G4ForceCondition*) override;

  // generic PostStepDoIt recommended for all derived classes
  G4VParticleChange* PostStepDoIt(const G4Track& aTrack, 
				  const G4Step& aStep) override;

private:

  // hide assignment operator as private 
  G4HadronInelasticProcess_SaG4n& operator=
  (const G4HadronInelasticProcess_SaG4n& right) = delete;
  G4HadronInelasticProcess_SaG4n(const G4HadronInelasticProcess_SaG4n&) = delete;
  
  const G4ParticleDefinition* fParticleDef;

  //new methods & attributes:
  G4double GetMaxXS(const G4Track& aTrack);
  void InitMaxXS();
  std::vector<std::vector <G4double>> theMaxXS;
  G4bool theMaxXS_filled=false;
  G4double theMaxXS_Emax=100*CLHEP::MeV;
  G4int theMaxXS_nBins=100;
  G4int theMaxXS_NEvalPerBin=100;
  G4double theMaxXS_safetyFactor=1.2;
};


inline const G4ParticleDefinition* G4HadronInelasticProcess_SaG4n::GetParticleDefinition() const {
  return fParticleDef;
}

#endif
