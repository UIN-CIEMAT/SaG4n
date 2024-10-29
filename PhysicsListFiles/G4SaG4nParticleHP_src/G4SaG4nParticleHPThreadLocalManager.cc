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
// Class Description
// Manager of NetronHP
// 
// 121031 First implementation done by T. Koi (SLAC/PPA)

#include "G4SaG4nParticleHPThreadLocalManager.hh"
#include "G4SaG4nParticleHPReactionWhiteBoard.hh"
#include "G4HadronicException.hh"

G4SaG4nParticleHPThreadLocalManager::G4SaG4nParticleHPThreadLocalManager()
:RWB(0)
{
;
}

G4SaG4nParticleHPThreadLocalManager::~G4SaG4nParticleHPThreadLocalManager()
{
;
}

G4SaG4nParticleHPThreadLocalManager::
G4SaG4nParticleHPThreadLocalManager( const G4SaG4nParticleHPThreadLocalManager& )
{
;
}

G4SaG4nParticleHPThreadLocalManager* G4SaG4nParticleHPThreadLocalManager::GetInstance()
{
  static G4ThreadLocalSingleton<G4SaG4nParticleHPThreadLocalManager> instance;
  return instance.Instance();
}

void G4SaG4nParticleHPThreadLocalManager::OpenReactionWhiteBoard()
{
   if ( RWB )
   {
      G4cout << "Warning: G4SaG4nParticleHPReactionWhiteBoard is tried doubly opening" << G4endl;
      return;
   }
   
   RWB = new G4SaG4nParticleHPReactionWhiteBoard();
}

G4SaG4nParticleHPReactionWhiteBoard*
G4SaG4nParticleHPThreadLocalManager::GetReactionWhiteBoard()
{
   if ( !RWB )
   {
      G4cout << "Warning: try to access G4SaG4nParticleHPReactionWhiteBoard before opening" << G4endl;
      RWB = new G4SaG4nParticleHPReactionWhiteBoard();
   }
   return RWB; 
}

void G4SaG4nParticleHPThreadLocalManager::CloseReactionWhiteBoard()
{  
   delete RWB; 
   RWB=0;
}
