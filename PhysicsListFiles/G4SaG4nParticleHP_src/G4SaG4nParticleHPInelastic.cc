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
// this code implementation is the intellectual property of
// neutron_hp -- source file
// J.P. Wellisch, Nov-1996
// A prototype of the low energy neutron transport model.
//
// By copying, distributing or modifying the Program (or any work
// based on the Program) you indicate your acceptance of this statement,
// and all its terms.
//
//
// 070523 bug fix for G4FPE_DEBUG on by A. Howard (and T. Koi)
// 081203 limit maximum trial for creating final states add protection for 1H isotope case by T. Koi
//
// P. Arce, June-2014 Conversion neutron_hp to particle_hp
//
#include "G4SaG4nParticleHPInelastic.hh"
#include "G4SystemOfUnits.hh"
#include "G4SaG4nParticleHPManager.hh"
#include "G4Threading.hh"

G4SaG4nParticleHPInelastic::G4SaG4nParticleHPInelastic(G4ParticleDefinition* projectile, const char* name )
  :G4HadronicInteraction(name)
  ,theInelastic(NULL)
  ,numEle(0)
  ,theProjectile(projectile)
{
  G4String baseName; 
  if ( getenv("G4PARTICLEHPDATA") ) {
     baseName = getenv( "G4PARTICLEHPDATA" );
  }
  //const char* dataDirVariable;
  G4String particleName;
  if( theProjectile == G4Neutron::Neutron() ) {
    dataDirVariable = "G4NEUTRONHPDATA";
  }else if( theProjectile == G4Proton::Proton() ) {
    dataDirVariable = "G4PROTONHPDATA";
     particleName = "Proton";
  }else if( theProjectile == G4Deuteron::Deuteron() ) {
    dataDirVariable = "G4DEUTERONHPDATA";
     particleName = "Deuteron";
  }else if( theProjectile == G4Triton::Triton() ) {
    dataDirVariable = "G4TRITONHPDATA";
     particleName = "Triton";
  }else if( theProjectile == G4He3::He3() ) {
    dataDirVariable = "G4HE3HPDATA";
     particleName = "He3";
  }else if( theProjectile == G4Alpha::Alpha() ) {
    dataDirVariable = "G4ALPHAHPDATA";
     particleName = "Alpha";
  } else {
    G4String message("G4SaG4nParticleHPInelastic may only be called for neutron, proton, deuteron, triton, He3 or alpha, while it is called for " + theProjectile->GetParticleName());
    throw G4HadronicException(__FILE__, __LINE__,message.c_str());
  }

    SetMinEnergy( 0.0 );
    SetMaxEnergy( 20.*MeV );

//    G4cout << " entering G4SaG4nParticleHPInelastic constructor"<<G4endl;
  if ( !getenv("G4PARTICLEHPDATA") && !getenv(dataDirVariable) ) {
     G4String message( "Please set the environement variable " + G4String(dataDirVariable) + " to point to the " + theProjectile->GetParticleName() + " cross-section files." );
     throw G4HadronicException(__FILE__, __LINE__,message.c_str());
  }
  if ( getenv(dataDirVariable) ) {
     dirName = getenv(dataDirVariable);
  } else {
     dirName = baseName + "/" + particleName;
  }
G4cout << dirName << G4endl;

    G4String tString = "/Inelastic";
    dirName = dirName + tString;
    //numEle = G4Element::GetNumberOfElements();

    G4cout << "@@@ G4SaG4nParticleHPInelastic instantiated for particle " << theProjectile->GetParticleName() << " data directory variable is " << dataDirVariable << " pointing to " << dirName << G4endl;

/*
    theInelastic = new G4SaG4nParticleHPChannelList[numEle];
    for (G4int i=0; i<numEle; i++)
    { 
      theInelastic[i].Init((*(G4Element::GetElementTable()))[i], dirName);
      G4int itry = 0;
      do
      {
	theInelastic[i].Register(&theNFS, "F01"); // has
	theInelastic[i].Register(&theNXFS, "F02");
	theInelastic[i].Register(&the2NDFS, "F03");
 	theInelastic[i].Register(&the2NFS, "F04"); // has, E Done
 	theInelastic[i].Register(&the3NFS, "F05"); // has, E Done
  	theInelastic[i].Register(&theNAFS, "F06");
	theInelastic[i].Register(&theN3AFS, "F07");
	theInelastic[i].Register(&the2NAFS, "F08");
	theInelastic[i].Register(&the3NAFS, "F09");
	theInelastic[i].Register(&theNPFS, "F10");
	theInelastic[i].Register(&theN2AFS, "F11");
	theInelastic[i].Register(&the2N2AFS, "F12");
	theInelastic[i].Register(&theNDFS, "F13");
	theInelastic[i].Register(&theNTFS, "F14");
	theInelastic[i].Register(&theNHe3FS, "F15");
	theInelastic[i].Register(&theND2AFS, "F16");
	theInelastic[i].Register(&theNT2AFS, "F17");
	theInelastic[i].Register(&the4NFS, "F18"); // has, E Done
	theInelastic[i].Register(&the2NPFS, "F19");
	theInelastic[i].Register(&the3NPFS, "F20");
	theInelastic[i].Register(&theN2PFS, "F21");
	theInelastic[i].Register(&theNPAFS, "F22");
  	theInelastic[i].Register(&thePFS, "F23");
	theInelastic[i].Register(&theDFS, "F24");
	theInelastic[i].Register(&theTFS, "F25");
	theInelastic[i].Register(&theHe3FS, "F26");
	theInelastic[i].Register(&theAFS, "F27");
	theInelastic[i].Register(&the2AFS, "F28");
	theInelastic[i].Register(&the3AFS, "F29");
	theInelastic[i].Register(&the2PFS, "F30");
	theInelastic[i].Register(&thePAFS, "F31");
	theInelastic[i].Register(&theD2AFS, "F32");
	theInelastic[i].Register(&theT2AFS, "F33");
	theInelastic[i].Register(&thePDFS, "F34");
	theInelastic[i].Register(&thePTFS, "F35");
	theInelastic[i].Register(&theDAFS, "F36");
	theInelastic[i].RestartRegistration();
        itry++;
      }
      //while(!theInelastic[i].HasDataInAnyFinalState());
      while( !theInelastic[i].HasDataInAnyFinalState() && itry < 6 );
                                                              // 6 is corresponding to the value(5) of G4SaG4nParticleHPChannel. TK  

      if ( itry == 6 ) 
      {
         // No Final State at all.
         G4bool exceptional = false;
         if ( (*(G4Element::GetElementTable()))[i]->GetNumberOfIsotopes() == 1 )
         {
            if ( (*(G4Element::GetElementTable()))[i]->GetIsotope( 0 )->GetZ() == 1 && (*(G4Element::GetElementTable()))[i]->GetIsotope( 0 )->GetN() == 1 ) exceptional = true;  //1H
         } 
         if ( !exceptional ) throw G4HadronicException(__FILE__, __LINE__, "Channel: Do not know what to do with this element");
      }
    }
*/
/*
    for (G4int i=0; i<numEle; i++)
    { 
      theInelastic.push_back( new G4SaG4nParticleHPChannelList );
      (*theInelastic[i]).Init((*(G4Element::GetElementTable()))[i], dirName, theProjectile);
      G4int itry = 0;
      do
      {
	(*theInelastic[i]).Register(&theNFS, "F01"); // has
	(*theInelastic[i]).Register(&theNXFS, "F02");
	(*theInelastic[i]).Register(&the2NDFS, "F03");
 	(*theInelastic[i]).Register(&the2NFS, "F04"); // has, E Done
 	(*theInelastic[i]).Register(&the3NFS, "F05"); // has, E Done
  	(*theInelastic[i]).Register(&theNAFS, "F06");
	(*theInelastic[i]).Register(&theN3AFS, "F07");
	(*theInelastic[i]).Register(&the2NAFS, "F08");
	(*theInelastic[i]).Register(&the3NAFS, "F09");
	(*theInelastic[i]).Register(&theNPFS, "F10");
	(*theInelastic[i]).Register(&theN2AFS, "F11");
	(*theInelastic[i]).Register(&the2N2AFS, "F12");
	(*theInelastic[i]).Register(&theNDFS, "F13");
	(*theInelastic[i]).Register(&theNTFS, "F14");
	(*theInelastic[i]).Register(&theNHe3FS, "F15");
	(*theInelastic[i]).Register(&theND2AFS, "F16");
	(*theInelastic[i]).Register(&theNT2AFS, "F17");
	(*theInelastic[i]).Register(&the4NFS, "F18"); // has, E Done
	(*theInelastic[i]).Register(&the2NPFS, "F19");
	(*theInelastic[i]).Register(&the3NPFS, "F20");
	(*theInelastic[i]).Register(&theN2PFS, "F21");
	(*theInelastic[i]).Register(&theNPAFS, "F22");
  	(*theInelastic[i]).Register(&thePFS, "F23");
	(*theInelastic[i]).Register(&theDFS, "F24");
	(*theInelastic[i]).Register(&theTFS, "F25");
	(*theInelastic[i]).Register(&theHe3FS, "F26");
	(*theInelastic[i]).Register(&theAFS, "F27");
	(*theInelastic[i]).Register(&the2AFS, "F28");
	(*theInelastic[i]).Register(&the3AFS, "F29");
	(*theInelastic[i]).Register(&the2PFS, "F30");
	(*theInelastic[i]).Register(&thePAFS, "F31");
	(*theInelastic[i]).Register(&theD2AFS, "F32");
	(*theInelastic[i]).Register(&theT2AFS, "F33");
	(*theInelastic[i]).Register(&thePDFS, "F34");
	(*theInelastic[i]).Register(&thePTFS, "F35");
	(*theInelastic[i]).Register(&theDAFS, "F36");
	(*theInelastic[i]).RestartRegistration();
        itry++;
      }
      while( !(*theInelastic[i]).HasDataInAnyFinalState() && itry < 6 );
                                                              // 6 is corresponding to the value(5) of G4SaG4nParticleHPChannel. TK  

      if ( itry == 6 ) 
      {
         // No Final State at all.
         G4bool exceptional = false;
         if ( (*(G4Element::GetElementTable()))[i]->GetNumberOfIsotopes() == 1 )
         {
            if ( (*(G4Element::GetElementTable()))[i]->GetIsotope( 0 )->GetZ() == 1 && (*(G4Element::GetElementTable()))[i]->GetIsotope( 0 )->GetN() == 1 ) exceptional = true;  //1H
         } 
	 if ( !exceptional ) {
 	 G4cerr << " ELEMENT Z " << (*(G4Element::GetElementTable()))[i]->GetIsotope( 0 )->GetZ() << " N " << (*(G4Element::GetElementTable()))[i]->GetIsotope( 0 )->GetN() << G4endl;  //1H
throw G4HadronicException(__FILE__, __LINE__, "Channel: Do not know what to do with this element");
	 }
      }

    }
*/

  }

  G4SaG4nParticleHPInelastic::~G4SaG4nParticleHPInelastic()
  {
//    delete [] theInelastic;
    //Vector is shared, only master deletes
    if ( !G4Threading::IsWorkerThread() ) {
        if ( theInelastic != NULL ) {
            for ( std::vector<G4SaG4nParticleHPChannelList*>::iterator
                it = theInelastic->begin() ; it != theInelastic->end() ; it++ ) {
                delete *it;
            }
            theInelastic->clear();
        }
     }
  }
  
  #include "G4SaG4nParticleHPThermalBoost.hh"
  
  G4HadFinalState * G4SaG4nParticleHPInelastic::ApplyYourself(const G4HadProjectile& aTrack, G4Nucleus& aNucleus )
  {
    G4SaG4nParticleHPManager::GetInstance()->OpenReactionWhiteBoard();
    const G4Material * theMaterial = aTrack.GetMaterial();
    G4int n = theMaterial->GetNumberOfElements();
    G4int index = theMaterial->GetElement(0)->GetIndex();
    G4int it=0;
    if(n!=1)
    {
      G4double* xSec = new G4double[n];
      G4double sum=0;
      G4int i;
      const G4double * NumAtomsPerVolume = theMaterial->GetVecNbOfAtomsPerVolume();
      G4double rWeight;    
      G4SaG4nParticleHPThermalBoost aThermalE;
      for (i=0; i<n; i++)
      {
        index = theMaterial->GetElement(i)->GetIndex();
        rWeight = NumAtomsPerVolume[i];
	if ( aTrack.GetDefinition() == G4Neutron::Neutron() ) {
	  xSec[i] = ((*theInelastic)[index])->GetXsec(aThermalE.GetThermalEnergy(aTrack,
									      theMaterial->GetElement(i),
									      theMaterial->GetTemperature()));
	} else {
	  xSec[i] = ((*theInelastic)[index])->GetXsec(aTrack.GetKineticEnergy());
	}
        xSec[i] *= rWeight;
        sum+=xSec[i];
#ifdef G4PHPDEBUG
	if( getenv("G4SaG4nParticleHPDebug") ) G4cout << " G4SaG4nParticleHPInelastic XSEC ELEM " << i << " = " << xSec[i] << G4endl;
#endif

      }
      G4double random = G4UniformRand();
      G4double running = 0;
      for (i=0; i<n; i++)
      {
        running += xSec[i];
        index = theMaterial->GetElement(i)->GetIndex();
        it = i;
        //if(random<=running/sum) break;
        if( sum == 0 || random<=running/sum) break;
      }
      delete [] xSec;
    }

#ifdef G4PHPDEBUG
    if( getenv("G4SaG4nParticleHPDebug") ) G4cout << " G4SaG4nParticleHPInelastic SELECTED ELEM " << it << " = " << theMaterial->GetElement(it)->GetName() << " FROM MATERIAL " << theMaterial->GetName() << G4endl;
#endif
    //return theInelastic[index].ApplyYourself(theMaterial->GetElement(it), aTrack);
    G4HadFinalState* result = ((*theInelastic)[index])->ApplyYourself(theMaterial->GetElement(it), aTrack);
    //
    aNucleus.SetParameters(G4SaG4nParticleHPManager::GetInstance()->GetReactionWhiteBoard()->GetTargA(),G4SaG4nParticleHPManager::GetInstance()->GetReactionWhiteBoard()->GetTargZ());
    const G4Element* target_element = (*G4Element::GetElementTable())[index];
    const G4Isotope* target_isotope=NULL;
    G4int iele = target_element->GetNumberOfIsotopes();
    for ( G4int j = 0 ; j != iele ; j++ ) { 
       target_isotope=target_element->GetIsotope( j );
       if ( target_isotope->GetN() == G4SaG4nParticleHPManager::GetInstance()->GetReactionWhiteBoard()->GetTargA() ) break; 
    }
    //G4cout << "Target Material of this reaction is " << theMaterial->GetName() << G4endl;
    //G4cout << "Target Element of this reaction is " << target_element->GetName() << G4endl;
    //G4cout << "Target Isotope of this reaction is " << target_isotope->GetName() << G4endl;
    aNucleus.SetIsotope( target_isotope );

    G4SaG4nParticleHPManager::GetInstance()->CloseReactionWhiteBoard();

    //GDEB
    if( getenv("G4PHPTEST") ) {
      G4HadSecondary* seco = result->GetSecondary(0);
      if(seco) {
	G4ThreeVector secoMom =  seco->GetParticle()->GetMomentum();
	G4cout << " G4SaG4nParticleHPinelastic COS THETA " << std::cos(secoMom.theta()) <<" " << secoMom << G4endl;
      }
    }

    return result;
  }

const std::pair<G4double, G4double> G4SaG4nParticleHPInelastic::GetFatalEnergyCheckLevels() const
{
      // max energy non-conservation is mass of heavy nucleus
//      if ( getenv("G4PHP_DO_NOT_ADJUST_FINAL_STATE") ) return std::pair<G4double, G4double>(5*perCent,250*GeV);
      // This should be same to the hadron default value
//      return std::pair<G4double, G4double>(10*perCent,10*GeV);
      return std::pair<G4double, G4double>(10*perCent,DBL_MAX);
}

/*
void G4SaG4nParticleHPInelastic::addChannelForNewElement()
{
   for ( G4int i = numEle ; i < (G4int)G4Element::GetNumberOfElements() ; i++ ) 
   {
      G4cout << "G4SaG4nParticleHPInelastic Prepairing Data for the new element of " << (*(G4Element::GetElementTable()))[i]->GetName() << G4endl;

      theInelastic.push_back( new G4SaG4nParticleHPChannelList );
      (*theInelastic[i]).Init((*(G4Element::GetElementTable()))[i], dirName, theProjectile);
      G4int itry = 0;
      do
      {
         (*theInelastic[i]).Register(&theNFS, "F01"); // has
         (*theInelastic[i]).Register(&theNXFS, "F02");
         (*theInelastic[i]).Register(&the2NDFS, "F03");
         (*theInelastic[i]).Register(&the2NFS, "F04"); // has, E Done
         (*theInelastic[i]).Register(&the3NFS, "F05"); // has, E Done
         (*theInelastic[i]).Register(&theNAFS, "F06");
         (*theInelastic[i]).Register(&theN3AFS, "F07");
         (*theInelastic[i]).Register(&the2NAFS, "F08");
         (*theInelastic[i]).Register(&the3NAFS, "F09");
         (*theInelastic[i]).Register(&theNPFS, "F10");
         (*theInelastic[i]).Register(&theN2AFS, "F11");
         (*theInelastic[i]).Register(&the2N2AFS, "F12");
         (*theInelastic[i]).Register(&theNDFS, "F13");
         (*theInelastic[i]).Register(&theNTFS, "F14");
         (*theInelastic[i]).Register(&theNHe3FS, "F15");
         (*theInelastic[i]).Register(&theND2AFS, "F16");
         (*theInelastic[i]).Register(&theNT2AFS, "F17");
         (*theInelastic[i]).Register(&the4NFS, "F18"); // has, E Done
         (*theInelastic[i]).Register(&the2NPFS, "F19");
         (*theInelastic[i]).Register(&the3NPFS, "F20");
         (*theInelastic[i]).Register(&theN2PFS, "F21");
         (*theInelastic[i]).Register(&theNPAFS, "F22");
         (*theInelastic[i]).Register(&thePFS, "F23");
         (*theInelastic[i]).Register(&theDFS, "F24");
         (*theInelastic[i]).Register(&theTFS, "F25");
         (*theInelastic[i]).Register(&theHe3FS, "F26");
         (*theInelastic[i]).Register(&theAFS, "F27");
         (*theInelastic[i]).Register(&the2AFS, "F28");
         (*theInelastic[i]).Register(&the3AFS, "F29");
         (*theInelastic[i]).Register(&the2PFS, "F30");
         (*theInelastic[i]).Register(&thePAFS, "F31");
         (*theInelastic[i]).Register(&theD2AFS, "F32");
         (*theInelastic[i]).Register(&theT2AFS, "F33");
         (*theInelastic[i]).Register(&thePDFS, "F34");
         (*theInelastic[i]).Register(&thePTFS, "F35");
         (*theInelastic[i]).Register(&theDAFS, "F36");
         (*theInelastic[i]).RestartRegistration();
         itry++;
      }
      while( !(*theInelastic[i]).HasDataInAnyFinalState() && itry < 6 );
                                                                  // 6 is corresponding to the value(5) of G4SaG4nParticleHPChannel. TK  

      if ( itry == 6 ) 
      {
         // No Final State at all.
         G4bool exceptional = false;
         if ( (*(G4Element::GetElementTable()))[i]->GetNumberOfIsotopes() == 1 )
         {
            if ( (*(G4Element::GetElementTable()))[i]->GetIsotope( 0 )->GetZ() == 1 && (*(G4Element::GetElementTable()))[i]->GetIsotope( 0 )->GetN() == 1 ) exceptional = true;  //1H
         } 
         if ( !exceptional ) throw G4HadronicException(__FILE__, __LINE__, "Channel: Do not know what to do with this element");
      }
   }

   numEle = (G4int)G4Element::GetNumberOfElements();
}
*/
G4int G4SaG4nParticleHPInelastic::GetVerboseLevel() const
{
   return G4SaG4nParticleHPManager::GetInstance()->GetVerboseLevel();
}
void G4SaG4nParticleHPInelastic::SetVerboseLevel( G4int newValue ) 
{
   G4SaG4nParticleHPManager::GetInstance()->SetVerboseLevel(newValue);
}

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

void G4SaG4nParticleHPInelastic::BuildPhysicsTable(const G4ParticleDefinition& projectile) {

   G4SaG4nParticleHPManager* hpmanager = G4SaG4nParticleHPManager::GetInstance();

   theInelastic = hpmanager->GetInelasticFinalStates( &projectile );

   if ( G4Threading::IsMasterThread() ) {

      if ( theInelastic == NULL ) theInelastic = new std::vector<G4SaG4nParticleHPChannelList*>;

      if ( numEle == (G4int)G4Element::GetNumberOfElements() ) return;

      if ( theInelastic->size() == G4Element::GetNumberOfElements() ) {
         numEle = G4Element::GetNumberOfElements();
         return;
      }
/*
      const char* dataDirVariable;
      if( &projectile == G4Neutron::Neutron() ) {
        dataDirVariable = "G4NEUTRONHPDATA";
      }else if( &projectile == G4Proton::Proton() ) {
        dataDirVariable = "G4PROTONHPDATA";
      }else if( &projectile == G4Deuteron::Deuteron() ) {
        dataDirVariable = "G4DEUTERONHPDATA";
      }else if( &projectile == G4Triton::Triton() ) {
        dataDirVariable = "G4TRITONHPDATA";
      }else if( &projectile == G4He3::He3() ) {
        dataDirVariable = "G4HE3HPDATA";
      }else if( &projectile == G4Alpha::Alpha() ) {
        dataDirVariable = "G4ALPHAHPDATA";
      } else {
         G4String message("G4SaG4nParticleHPInelastic may only be called for neutron, proton, deuteron, triton, He3 or alpha, while it is called for " + projectile.GetParticleName());
         throw G4HadronicException(__FILE__, __LINE__,message.c_str());
      }
      if(!getenv(dataDirVariable)){
         G4String message("Please set the environement variable " + G4String(dataDirVariable) + " to point to the " + projectile.GetParticleName() + " cross-section files.");
         throw G4HadronicException(__FILE__, __LINE__,message.c_str());
      }
      dirName = getenv(dataDirVariable);
      G4cout << dirName << G4endl;

      G4String tString = "/Inelastic";
      dirName = dirName + tString;

*/
      G4cout << "@@@ G4SaG4nParticleHPInelastic instantiated for particle " << projectile.GetParticleName() << " data directory variable is " << dataDirVariable << " pointing to " << dirName << G4endl;
      for (G4int i = numEle ; i < (G4int)G4Element::GetNumberOfElements(); i++)
      {
        theInelastic->push_back( new G4SaG4nParticleHPChannelList );
        ((*theInelastic)[i])->Init((*(G4Element::GetElementTable()))[i], dirName, const_cast<G4ParticleDefinition*>(&projectile));
        G4int itry = 0;
        do
        {
          ((*theInelastic)[i])->Register( new G4SaG4nParticleHPNInelasticFS , "F01"); // has
          ((*theInelastic)[i])->Register( new G4SaG4nParticleHPNXInelasticFS , "F02");
          ((*theInelastic)[i])->Register( new G4SaG4nParticleHP2NDInelasticFS , "F03");
          ((*theInelastic)[i])->Register( new G4SaG4nParticleHP2NInelasticFS , "F04"); // has, E Done
          ((*theInelastic)[i])->Register( new G4SaG4nParticleHP3NInelasticFS , "F05"); // has, E Done
          ((*theInelastic)[i])->Register( new G4SaG4nParticleHPNAInelasticFS , "F06");
          ((*theInelastic)[i])->Register( new G4SaG4nParticleHPN3AInelasticFS , "F07");
          ((*theInelastic)[i])->Register( new G4SaG4nParticleHP2NAInelasticFS , "F08");
          ((*theInelastic)[i])->Register( new G4SaG4nParticleHP3NAInelasticFS , "F09");
          ((*theInelastic)[i])->Register( new G4SaG4nParticleHPNPInelasticFS , "F10");
          ((*theInelastic)[i])->Register( new G4SaG4nParticleHPN2AInelasticFS , "F11");
          ((*theInelastic)[i])->Register( new G4SaG4nParticleHP2N2AInelasticFS , "F12");
          ((*theInelastic)[i])->Register( new G4SaG4nParticleHPNDInelasticFS , "F13");
          ((*theInelastic)[i])->Register( new G4SaG4nParticleHPNTInelasticFS , "F14");
          ((*theInelastic)[i])->Register( new G4SaG4nParticleHPNHe3InelasticFS , "F15");
          ((*theInelastic)[i])->Register( new G4SaG4nParticleHPND2AInelasticFS , "F16");
          ((*theInelastic)[i])->Register( new G4SaG4nParticleHPNT2AInelasticFS , "F17");
          ((*theInelastic)[i])->Register( new G4SaG4nParticleHP4NInelasticFS , "F18"); // has, E Done
          ((*theInelastic)[i])->Register( new G4SaG4nParticleHP2NPInelasticFS , "F19");
          ((*theInelastic)[i])->Register( new G4SaG4nParticleHP3NPInelasticFS , "F20");
          ((*theInelastic)[i])->Register( new G4SaG4nParticleHPN2PInelasticFS , "F21");
          ((*theInelastic)[i])->Register( new G4SaG4nParticleHPNPAInelasticFS , "F22");
          ((*theInelastic)[i])->Register( new G4SaG4nParticleHPPInelasticFS , "F23");
          ((*theInelastic)[i])->Register( new G4SaG4nParticleHPDInelasticFS , "F24");
          ((*theInelastic)[i])->Register( new G4SaG4nParticleHPTInelasticFS , "F25");
          ((*theInelastic)[i])->Register( new G4SaG4nParticleHPHe3InelasticFS , "F26");
          ((*theInelastic)[i])->Register( new G4SaG4nParticleHPAInelasticFS , "F27");
          ((*theInelastic)[i])->Register( new G4SaG4nParticleHP2AInelasticFS , "F28");
          ((*theInelastic)[i])->Register( new G4SaG4nParticleHP3AInelasticFS , "F29");
          ((*theInelastic)[i])->Register( new G4SaG4nParticleHP2PInelasticFS , "F30");
          ((*theInelastic)[i])->Register( new G4SaG4nParticleHPPAInelasticFS , "F31");
          ((*theInelastic)[i])->Register( new G4SaG4nParticleHPD2AInelasticFS , "F32");
          ((*theInelastic)[i])->Register( new G4SaG4nParticleHPT2AInelasticFS , "F33");
          ((*theInelastic)[i])->Register( new G4SaG4nParticleHPPDInelasticFS , "F34");
          ((*theInelastic)[i])->Register( new G4SaG4nParticleHPPTInelasticFS , "F35");
          ((*theInelastic)[i])->Register( new G4SaG4nParticleHPDAInelasticFS , "F36");
          ((*theInelastic)[i])->RestartRegistration();
          itry++;
        }
        while( !((*theInelastic)[i])->HasDataInAnyFinalState() && itry < 6 ); // Loop checking, 11.05.2015, T. Koi
                                                              // 6 is corresponding to the value(5) of G4SaG4nParticleHPChannel. TK  

        if ( itry == 6 ) {
           // No Final State at all.
           /*
           G4bool exceptional = false;
           if ( (*(G4Element::GetElementTable()))[i]->GetNumberOfIsotopes() == 1 )
           {
              if ( (*(G4Element::GetElementTable()))[i]->GetIsotope( 0 )->GetZ() == 1 && (*(G4Element::GetElementTable()))[i]->GetIsotope( 0 )->GetN() == 1 ) exceptional = true;  //1H
           } 
	   if ( !exceptional ) {
 	      G4cerr << " ELEMENT Z " << (*(G4Element::GetElementTable()))[i]->GetIsotope( 0 )->GetZ() << " N " << (*(G4Element::GetElementTable()))[i]->GetIsotope( 0 )->GetN() << G4endl;  //1H
              throw G4HadronicException(__FILE__, __LINE__, "Channel: Do not know what to do with this element");
	   }
           */
           if ( G4SaG4nParticleHPManager::GetInstance()->GetVerboseLevel() > 1 ) {
              G4cout << "ParticleHP::Inelastic for " << projectile.GetParticleName() << ". Do not know what to do with element of \"" << (*(G4Element::GetElementTable()))[i]->GetName() << "\"." << G4endl;
              G4cout << "The components of the element are" << G4endl;
 	      //G4cout << "TKDB dataDirVariable = " << dataDirVariable << G4endl;
              for ( G4int ii = 0 ; ii < (G4int)( (*(G4Element::GetElementTable()))[i]->GetNumberOfIsotopes() ) ; ii++ ) { 
 	          G4cout << " Z: " << (*(G4Element::GetElementTable()))[i]->GetIsotope( ii )->GetZ() << ", A: " << (*(G4Element::GetElementTable()))[i]->GetIsotope( ii )->GetN() << G4endl;
              }
              G4cout << "No possible final state data of the element is found in " << dataDirVariable << "." << G4endl;
           }
        }
      }
      hpmanager->RegisterInelasticFinalStates( &projectile , theInelastic );
   }
   numEle = G4Element::GetNumberOfElements();
}

void G4SaG4nParticleHPInelastic::ModelDescription(std::ostream& outFile) const
{
   outFile << "Extension of High Precision model for inelastic reaction of proton, deuteron, triton, He3 and alpha below 20MeV\n";
}

