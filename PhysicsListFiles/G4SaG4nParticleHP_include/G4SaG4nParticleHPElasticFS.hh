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
//
//
// 25-08-06 New Final State type (refFlag==3 , Legendre (Low Energy) + Probability (High Energy) )
//          is added by T. KOI
//
// P. Arce, June-2014 Conversion neutron_hp to particle_hp
//
#ifndef G4SaG4nParticleHPElasticFS_h
#define G4SaG4nParticleHPElasticFS_h 1

#include "globals.hh"
#include "G4HadProjectile.hh"
#include "G4HadFinalState.hh"
#include "G4SaG4nParticleHPFinalState.hh"
#include "G4SaG4nParticleHPLegendreStore.hh"
#include "G4SaG4nParticleHPPartial.hh"
#include "G4SaG4nParticleHPFastLegendre.hh"
#include "G4SaG4nParticleHPInterpolator.hh"
#include "G4SaG4nParticleHPNames.hh"

class G4SaG4nParticleHPElasticFS : public G4SaG4nParticleHPFinalState
{
  public:
  
  G4SaG4nParticleHPElasticFS()
  {
    hasXsec = false; 
    theCoefficients = 0;
    theProbArray = 0;
    
    repFlag = 0;
    tE_of_repFlag3 = 0.0;
    targetMass = 0.0;
    frameFlag = 0;
  }
  ~G4SaG4nParticleHPElasticFS()
  {
    if(theCoefficients!=0) delete theCoefficients;
    if(theProbArray!=0) delete theProbArray;
  }
  void Init (G4double A, G4double Z, G4int M, G4String & dirName, G4String & aFSType, G4ParticleDefinition*);
  G4HadFinalState * ApplyYourself(const G4HadProjectile & theTrack);
  G4SaG4nParticleHPFinalState * New() 
  {
   G4SaG4nParticleHPElasticFS * theNew = new G4SaG4nParticleHPElasticFS;
   return theNew;
  }
  
  private:
  G4int repFlag;    // Legendre coeff(1), or probability array(2), or isotropic(0).
                    // add 3: for Legendre (Low Energy) + Probability (High Energy)
  G4double tE_of_repFlag3; // transition energy in case of  repFlag 3:
  G4double targetMass; // in neutronmass units.
  G4int frameFlag;  // CMS or Lab system.
  
  G4SaG4nParticleHPLegendreStore * theCoefficients; // the legendre coefficients
  G4SaG4nParticleHPPartial * theProbArray; // the probability array p,costh for energy
  G4SaG4nParticleHPInterpolator theInt; // interpolation
  
  G4SaG4nParticleHPFastLegendre theLegend; // fast look-up for leg-integrals
};
#endif
