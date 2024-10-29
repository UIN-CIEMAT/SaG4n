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
 // Hadronic Process: High Precision low E neutron tracking
 // original by H.P. Wellisch, TRIUMF, 14-Feb-97
 // Builds and has the Cross-section data for one material.
// P. Arce, June-2014 Conversion neutron_hp to particle_hp
//
#ifndef G4SaG4nParticleHPInelastic_h
#define G4SaG4nParticleHPInelastic_h 1

// Class Description
// Final state production model for a high precision (based on evaluated data
// libraries) description of neutron inelastic scattering below 20 MeV; 
// 36 exclusive final states are consideded.
// To be used in your physics list in case you need this physics.
// In this case you want to register an object of this class with 
// the corresponding process.
// Class Description - End

#include "globals.hh"
#include "G4SaG4nParticleHPChannel.hh"
#include "G4HadronicInteraction.hh"
#include "G4SaG4nParticleHPChannelList.hh"

/*
#include "G4SaG4nParticleHP2AInelasticFS.hh"
#include "G4SaG4nParticleHP2N2AInelasticFS.hh"
#include "G4SaG4nParticleHP2NAInelasticFS.hh"
#include "G4SaG4nParticleHP2NDInelasticFS.hh"
#include "G4SaG4nParticleHP2NInelasticFS.hh"
#include "G4SaG4nParticleHP2NPInelasticFS.hh"
#include "G4SaG4nParticleHP2PInelasticFS.hh"
#include "G4SaG4nParticleHP3AInelasticFS.hh"
#include "G4SaG4nParticleHP3NAInelasticFS.hh"
#include "G4SaG4nParticleHP3NInelasticFS.hh"
#include "G4SaG4nParticleHP3NPInelasticFS.hh"
#include "G4SaG4nParticleHP4NInelasticFS.hh"
#include "G4SaG4nParticleHPAInelasticFS.hh"
#include "G4SaG4nParticleHPD2AInelasticFS.hh"
#include "G4SaG4nParticleHPDAInelasticFS.hh"
#include "G4SaG4nParticleHPDInelasticFS.hh"
#include "G4SaG4nParticleHPHe3InelasticFS.hh"
#include "G4SaG4nParticleHPN2AInelasticFS.hh"
#include "G4SaG4nParticleHPN2PInelasticFS.hh"
#include "G4SaG4nParticleHPN3AInelasticFS.hh"
#include "G4SaG4nParticleHPNAInelasticFS.hh"
#include "G4SaG4nParticleHPND2AInelasticFS.hh"
#include "G4SaG4nParticleHPNDInelasticFS.hh"
#include "G4SaG4nParticleHPNHe3InelasticFS.hh"
#include "G4SaG4nParticleHPNInelasticFS.hh"
#include "G4SaG4nParticleHPNPAInelasticFS.hh"
#include "G4SaG4nParticleHPNPInelasticFS.hh"
#include "G4SaG4nParticleHPNT2AInelasticFS.hh"
#include "G4SaG4nParticleHPNTInelasticFS.hh"
#include "G4SaG4nParticleHPNXInelasticFS.hh"
#include "G4SaG4nParticleHPPAInelasticFS.hh"
#include "G4SaG4nParticleHPPDInelasticFS.hh"
#include "G4SaG4nParticleHPPInelasticFS.hh"
#include "G4SaG4nParticleHPPTInelasticFS.hh"
#include "G4SaG4nParticleHPT2AInelasticFS.hh"
#include "G4SaG4nParticleHPTInelasticFS.hh"
*/
#include "G4ParticleDefinition.hh"

class G4SaG4nParticleHPInelastic : public G4HadronicInteraction
{
  public: 

  G4SaG4nParticleHPInelastic(G4ParticleDefinition* projectile = G4Neutron::Neutron(), const char* name = "NeutronHPInelastic" );

  ~G4SaG4nParticleHPInelastic();
  
  G4HadFinalState * ApplyYourself(const G4HadProjectile & aTrack, G4Nucleus & aTargetNucleus);
  virtual const std::pair<G4double, G4double> GetFatalEnergyCheckLevels() const;

  public:
      G4int GetVerboseLevel() const;
      void SetVerboseLevel( G4int );
      void BuildPhysicsTable(const G4ParticleDefinition&);
      virtual void ModelDescription(std::ostream& outFile) const;

protected:
  
  //G4SaG4nParticleHPChannelList * theInelastic; // one List per element
  std::vector<G4SaG4nParticleHPChannelList*>* theInelastic; // one List per element
  G4String dataDirVariable;
  G4String dirName;
  G4int numEle;

  private:
 /* 
   G4SaG4nParticleHP2AInelasticFS the2AFS;
   G4SaG4nParticleHP2N2AInelasticFS the2N2AFS;
   G4SaG4nParticleHP2NAInelasticFS the2NAFS;
   G4SaG4nParticleHP2NDInelasticFS the2NDFS;
   G4SaG4nParticleHP2NInelasticFS the2NFS;
   G4SaG4nParticleHP2NPInelasticFS the2NPFS;
   G4SaG4nParticleHP2PInelasticFS the2PFS;
   G4SaG4nParticleHP3AInelasticFS the3AFS;
   G4SaG4nParticleHP3NAInelasticFS the3NAFS;
   G4SaG4nParticleHP3NInelasticFS the3NFS;
   G4SaG4nParticleHP3NPInelasticFS the3NPFS;
   G4SaG4nParticleHP4NInelasticFS the4NFS;
   G4SaG4nParticleHPAInelasticFS theAFS;
   G4SaG4nParticleHPD2AInelasticFS theD2AFS;
   G4SaG4nParticleHPDAInelasticFS theDAFS;
   G4SaG4nParticleHPDInelasticFS theDFS;
   G4SaG4nParticleHPHe3InelasticFS theHe3FS;
   G4SaG4nParticleHPN2AInelasticFS theN2AFS;
   G4SaG4nParticleHPN2PInelasticFS theN2PFS;
   G4SaG4nParticleHPN3AInelasticFS theN3AFS;
   G4SaG4nParticleHPNAInelasticFS theNAFS;
   G4SaG4nParticleHPND2AInelasticFS theND2AFS;
   G4SaG4nParticleHPNDInelasticFS theNDFS;
   G4SaG4nParticleHPNHe3InelasticFS theNHe3FS;
   G4SaG4nParticleHPNInelasticFS theNFS;
   G4SaG4nParticleHPNPAInelasticFS theNPAFS;
   G4SaG4nParticleHPNPInelasticFS theNPFS;
   G4SaG4nParticleHPNT2AInelasticFS theNT2AFS;
   G4SaG4nParticleHPNTInelasticFS theNTFS;
   G4SaG4nParticleHPNXInelasticFS theNXFS;
   G4SaG4nParticleHPPAInelasticFS thePAFS;
   G4SaG4nParticleHPPDInelasticFS thePDFS;
   G4SaG4nParticleHPPInelasticFS thePFS;
   G4SaG4nParticleHPPTInelasticFS thePTFS;
   G4SaG4nParticleHPT2AInelasticFS theT2AFS;
   G4SaG4nParticleHPTInelasticFS theTFS;
*/

   G4ParticleDefinition* theProjectile;

};

#endif
