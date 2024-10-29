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
 // Hadronic Process: Very Low Energy Neutron X-Sections
 // original by H.P. Wellisch, TRIUMF, 14-Feb-97
 // Has the Cross-section data for all materials.
// P. Arce, June-2014 Conversion neutron_hp to particle_hp
//
#ifndef G4SaG4nParticleHPData_h
#define G4SaG4nParticleHPData_h 1
#include "globals.hh"
#include "G4Element.hh"
#include "G4SaG4nParticleHPElasticData.hh"
#include "G4SaG4nParticleHPInelasticData.hh"
#include "G4SaG4nParticleHPFissionData.hh"
#include "G4SaG4nParticleHPCaptureData.hh"
#include "G4SaG4nParticleHPElementData.hh"

#include <vector>

class G4SaG4nParticleHPData
{
public:

  G4SaG4nParticleHPData(G4ParticleDefinition* projectile);

  ~G4SaG4nParticleHPData();

  inline G4PhysicsVector * MakePhysicsVector(G4Element * thE, G4SaG4nParticleHPFissionData * theP)
  {
     if ( numEle <= (G4int)thE->GetIndex() ) addPhysicsVector(); 
     return DoPhysicsVector((*theData[thE->GetIndex()]).GetData(theP));
  }
  inline G4PhysicsVector * MakePhysicsVector(G4Element * thE, G4SaG4nParticleHPCaptureData * theP)
  {
     if ( numEle <= (G4int)thE->GetIndex() ) addPhysicsVector(); 
     return DoPhysicsVector((*theData[thE->GetIndex()]).GetData(theP));
  }
  inline G4PhysicsVector * MakePhysicsVector(G4Element * thE, G4SaG4nParticleHPElasticData * theP)
  {
     if ( numEle <= (G4int)thE->GetIndex() ) addPhysicsVector(); 
     return DoPhysicsVector((*theData[thE->GetIndex()]).GetData(theP));
  }
  inline G4PhysicsVector * MakePhysicsVector(G4Element * thE, G4SaG4nParticleHPInelasticData * theP)
  {
//     G4cout << "entered G4SaG4nParticleHPData::MakePhysicsVector!!!"<<G4endl;
//     G4cout << "thE->GetIndex()="<<thE->GetIndex()<<G4endl;
     if ( numEle <= (G4int)thE->GetIndex() ) addPhysicsVector(); 
     return DoPhysicsVector((*theData[thE->GetIndex()]).GetData(theP));
  }

  G4PhysicsVector * DoPhysicsVector(G4SaG4nParticleHPVector * theVector);
  
  static G4SaG4nParticleHPData * Instance(G4ParticleDefinition* projectile);
  
private:

  std::vector< G4SaG4nParticleHPElementData* > theData;
  G4int numEle;
  void addPhysicsVector();

  G4ParticleDefinition* theProjectile;
  G4String theDataDirVariable;
};

#endif
