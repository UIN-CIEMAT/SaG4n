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
 // Hadronic Process: Very Low Energy Neutron X-Sections
 // original by H.P. Wellisch, TRIUMF, 14-Feb-97
 // Builds and has the Cross-section data for one material.
//
// 080520 Delete unnecessary dependencies by T. Koi
 
// P. Arce, June-2014 Conversion neutron_hp to particle_hp
//
#ifndef G4SaG4nParticleHPElementData_h
#define G4SaG4nParticleHPElementData_h 1
#include "globals.hh"
#include "G4SaG4nParticleHPIsoData.hh"
#include "G4SaG4nParticleHPVector.hh"
#include "G4Material.hh"
#include "G4HadronCrossSections.hh"
#include "G4ReactionProduct.hh"
#include "G4Nucleus.hh"
//#include "G4NeutronInelasticProcess.hh"
//#include "G4HadronFissionProcess.hh"
//#include "G4HadronCaptureProcess.hh"
#include "G4SaG4nParticleHPElasticData.hh"
#include "G4SaG4nParticleHPFissionData.hh"
#include "G4SaG4nParticleHPCaptureData.hh"
#include "G4SaG4nParticleHPInelasticData.hh"
#include "G4StableIsotopes.hh"
#include "G4Neutron.hh"

//class G4SaG4nParticleHPElementData : public G4HadronCrossSections
class G4SaG4nParticleHPElementData
{
public:

  G4SaG4nParticleHPElementData();
  
  ~G4SaG4nParticleHPElementData();
  
  //  void Init(G4Element * theElement,char* dataDirVariable = "G4NEUTRONHPDATA", G4ParticleDefinition* projectile = G4Neutron::Neutron() );
  void Init(G4Element * theElement, G4ParticleDefinition* projectile, const char* dataDirVariable);
  
  //void UpdateData(G4int A, G4int Z, G4int index, G4double abundance);
  //  void UpdateData(G4int A, G4int Z, G4int index, G4double abundance,char* dataDirVariable, G4ParticleDefinition* projectile = G4Neutron::Neutron() ) { G4int M=0; UpdateData( A, Z, M, index, abundance, projectile, dataDirVariable); };
  void UpdateData(G4int A, G4int Z, G4int index, G4double abundance,G4ParticleDefinition* projectile, const char* dataDirVariable ) { G4int M=0; UpdateData( A, Z, M, index, abundance, projectile, dataDirVariable); };
  void UpdateData(G4int A, G4int Z, G4int M, G4int index, G4double abundance, G4ParticleDefinition* projectile, const char* dataDirVariable );
  
  void Harmonise(G4SaG4nParticleHPVector *& theStore, G4SaG4nParticleHPVector * theNew);
  
  inline G4SaG4nParticleHPVector * GetData(G4SaG4nParticleHPFissionData * )
    {return theFissionData;}
  inline G4SaG4nParticleHPVector * GetData(G4SaG4nParticleHPCaptureData * )
    {return theCaptureData;}
  inline G4SaG4nParticleHPVector * GetData(G4SaG4nParticleHPElasticData * )
    {return theElasticData;}
  inline G4SaG4nParticleHPVector * GetData(G4SaG4nParticleHPInelasticData * )
    {return theInelasticData;}

  G4SaG4nParticleHPVector * MakePhysicsVector(G4Element * theElement,
					G4ParticleDefinition * projectile,
					G4SaG4nParticleHPFissionData* theSet,
					char* dataDirVariable);

  G4SaG4nParticleHPVector * MakePhysicsVector(G4Element * theElement,
					G4ParticleDefinition * projectile,
					G4SaG4nParticleHPCaptureData * theSet,
					char* dataDirVariable);
  
  G4SaG4nParticleHPVector * MakePhysicsVector(G4Element * theElement,
					G4ParticleDefinition * projectile,
					G4SaG4nParticleHPElasticData * theSet,
					char* dataDirVariable);
  
  G4SaG4nParticleHPVector * MakePhysicsVector(G4Element * theElement,
					 G4ParticleDefinition * projectile,
					 G4SaG4nParticleHPInelasticData * theSet,
					char* dataDirVariable );

private:

  G4SaG4nParticleHPVector * theFissionData;
  G4SaG4nParticleHPVector * theCaptureData;
  G4SaG4nParticleHPVector * theElasticData;
  G4SaG4nParticleHPVector * theInelasticData;
  G4double precision;
  
  G4SaG4nParticleHPVector * theBuffer;
  
  G4SaG4nParticleHPIsoData * theIsotopeWiseData;

  G4StableIsotopes theStableOnes;
  
  G4String filename;
  
};

#endif
