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
// Hadronic Inelastic Process Class
// J.L. Chuma, TRIUMF, 24-Mar-1997
// Last modified: 27-Mar-1997
// J.P. Wellisch: Bug hunting, 23-Apr-97
// Modified by J.L.Chuma 8-Jul-97 to eliminate possible division by zero for sigma
//
// 14-APR-98 F.W.Jones: variant G4HadronInelastic process for
// G4CrossSectionDataSet/DataStore class design.
//
// 17-JUN-98 F.W.Jones: removed extraneous code causing core dump.
// 01-SEP-2008 V.Ivanchenko: use methods from the base class
// 14-Sep-12 M.Kelsey -- Pass subType code to base ctor
//
 
#include "G4HadronInelasticProcess_SaG4n.hh"
#include "G4GenericIon.hh"
#include "G4ParticleDefinition.hh"
  
G4HadronInelasticProcess_SaG4n::G4HadronInelasticProcess_SaG4n(const G4String& processName,
                                                   const G4ParticleDefinition* particleDef):
  G4HadronicProcess(processName,fHadronInelastic), fParticleDef(particleDef)
{}

G4HadronInelasticProcess_SaG4n::~G4HadronInelasticProcess_SaG4n() 
{}

G4bool G4HadronInelasticProcess_SaG4n::IsApplicable(const G4ParticleDefinition&)
{
  return  true;
}


G4double G4HadronInelasticProcess_SaG4n::GetMaxXS(const G4Track& aTrack){

  
  G4double kinEne=aTrack.GetKineticEnergy();
  
  // We have to provide a XS (in cm^-1) which is max or greater than the max between 0 and kinEne
  G4int i_bin=(G4int)(kinEne/theMaxXS_Emax*theMaxXS_nBins)+1;
  if(i_bin>theMaxXS_nBins){ //out of range
    return theCrossSectionDataStore->ComputeCrossSection(aTrack.GetDynamicParticle(),aTrack.GetMaterial());
  }

  G4int  materialIndex = (G4int)aTrack.GetMaterial()->GetIndex();
  return theMaxXS[materialIndex][i_bin];
}

void G4HadronInelasticProcess_SaG4n::InitMaxXS(){

  if(theMaxXS_filled){return ;}
  theMaxXS_filled=true;

  const G4MaterialTable* theMatTable = G4Material::GetMaterialTable();
  size_t nMat = G4Material::GetNumberOfMaterials();
  theMaxXS.resize(nMat);
  for (int i = 0; i < nMat; ++i) {
    theMaxXS[i].resize(theMaxXS_nBins, 0.0);
    auto mat = (*theMatTable)[i];
    for(int j=0;j<theMaxXS_nBins;j++){
      double e1=j*(theMaxXS_Emax/theMaxXS_nBins);
      double e2=(j+1)*(theMaxXS_Emax/theMaxXS_nBins);
      theMaxXS[i][j]=0;
      if(j>0){
	theMaxXS[i][j]=theMaxXS[i][j-1];
      }
      //We have to estimate a maximum (or larger) between e1 and e2
      for(G4int k=0;k<theMaxXS_NEvalPerBin;k++){
	G4double ekin=e1+(e2-e1)*k/(theMaxXS_NEvalPerBin-1.); //lin or log?
	auto dp = new G4DynamicParticle(fParticleDef,G4ThreeVector(0.,0.,1.), ekin);
	G4double newXS=theCrossSectionDataStore->ComputeCrossSection(dp, mat);
	delete dp;
	if(newXS>theMaxXS[i][j]){theMaxXS[i][j]=newXS;}
      }
    }
  }

}

G4double G4HadronInelasticProcess_SaG4n::PostStepGetPhysicalInteractionLength(
                             const G4Track& track, 
			     G4double previousStepSize,
                             G4ForceCondition* condition)
{

  *condition = NotForced;
 
  //Here we should compute a XS which is the max. XS of the XS along the step or greater.
  InitMaxXS();
  G4double maxXS=GetMaxXS(track);
  
  maxXS*=theMaxXS_safetyFactor; //we increase the value, just in case
  theLastCrossSection = aScaleFactor*maxXS;
  G4double thisMFP = (theLastCrossSection > 0.0) ? 1.0/theLastCrossSection : DBL_MAX;

  // zero cross section
  if(theLastCrossSection <= 0.0) { 
    theNumberOfInteractionLengthLeft = -1.0;
    currentInteractionLength = DBL_MAX;
    return DBL_MAX;
  }
  
  // non-zero cross section
  if (theNumberOfInteractionLengthLeft < 0.0) {
    theNumberOfInteractionLengthLeft = -G4Log( G4UniformRand() );
    theInitialNumberOfInteractionLength = theNumberOfInteractionLengthLeft; 
  } else {
    theNumberOfInteractionLengthLeft -= 
      previousStepSize/currentInteractionLength;
    theNumberOfInteractionLengthLeft = 
      std::max(theNumberOfInteractionLengthLeft, 0.0);
  }
  currentInteractionLength = thisMFP;

  
  return theNumberOfInteractionLengthLeft*thisMFP;
}



G4VParticleChange*
G4HadronInelasticProcess_SaG4n::PostStepDoIt(const G4Track& aTrack, const G4Step&)
{
  theNumberOfInteractionLengthLeft = -1.0;

  //G4cout << "PostStepDoIt " << aTrack.GetDefinition()->GetParticleName()
  //	 << " Ekin= " << aTrack.GetKineticEnergy() << G4endl;
  // if primary is not Alive then do nothing
  theTotalResult->Clear();
  theTotalResult->Initialize(aTrack);
  fWeight = aTrack.GetWeight();
  theTotalResult->ProposeWeight(fWeight);
  if(aTrack.GetTrackStatus() != fAlive) { return theTotalResult; }

  // Find cross section at end of step and check if <= 0
  //
  const G4DynamicParticle* aParticle = aTrack.GetDynamicParticle();
  const G4Material* aMaterial = aTrack.GetMaterial();

  G4double xs = aScaleFactor*
    theCrossSectionDataStore->ComputeCrossSection(aParticle,aMaterial);
  //G4cout << "xs=" << xs << " xs0=" << theLastCrossSection
  //	   << "  " << aMaterial->GetName() << G4endl;
  if(xs < theLastCrossSection*G4UniformRand()) {
    // No interaction
    return theTotalResult;
  }    

  G4Nucleus theTargetNucleus;
  const G4Element* anElement = 
    theCrossSectionDataStore->SampleZandA(aParticle,aMaterial,theTargetNucleus);

  // Next check for illegal track status
  //
  if (aTrack.GetTrackStatus() != fAlive && 
      aTrack.GetTrackStatus() != fSuspend) {
    if (aTrack.GetTrackStatus() == fStopAndKill ||
        aTrack.GetTrackStatus() == fKillTrackAndSecondaries ||
        aTrack.GetTrackStatus() == fPostponeToNextEvent) {
      G4ExceptionDescription ed;
      ed << "G4HadronicProcess_SaG4n: track in unusable state - "
	 << aTrack.GetTrackStatus() << G4endl;
      ed << "G4HadronicProcess_SaG4n: returning unchanged track " << G4endl;
      DumpState(aTrack,"PostStepDoIt",ed);
      G4Exception("G4HadronicProcess_SaG4n::PostStepDoIt", "had004", JustWarning, ed);
    }
    // No warning for fStopButAlive which is a legal status here
    return theTotalResult;
  }

  // Initialize the hadronic projectile from the track
  thePro.Initialise(aTrack);

  G4HadronicInteraction* anInteraction = ChooseHadronicInteraction(thePro, theTargetNucleus, 
                                             aMaterial, anElement);
  if(nullptr == anInteraction) {
    G4ExceptionDescription ed;
    ed << "Target element "<<anElement->GetName()<<"  Z= "
       << theTargetNucleus.GetZ_asInt() << "  A= "
       << theTargetNucleus.GetA_asInt() << G4endl;
    DumpState(aTrack,"ChooseHadronicInteraction",ed);
    ed << " No HadronicInteraction found out" << G4endl;
    G4Exception("G4HadronicProcess_SaG4n::PostStepDoIt", "had005",
                FatalException, ed);
    return theTotalResult;
  }

  G4HadFinalState* result = nullptr;
  G4int reentryCount = 0;
  /*
  G4cout << "### " << aParticle->GetDefinition()->GetParticleName() 
	 << "  Ekin(MeV)= " << aParticle->GetKineticEnergy()
	 << "  Z= " << theTargetNucleus.GetZ_asInt() 
	 << "  A= " << theTargetNucleus.GetA_asInt()
	 << "  by " << anInteraction->GetModelName() 
	 << G4endl;
  */
  do
  {
    try
    {
      // Call the interaction
      result = anInteraction->ApplyYourself( thePro, theTargetNucleus);
      ++reentryCount;
    }
    catch(G4HadronicException & aR)
    {
      G4ExceptionDescription ed;
      aR.Report(ed);
      ed << "Call for " << anInteraction->GetModelName() << G4endl;
      ed << "Target element "<<anElement->GetName()<<"  Z= "
	 << theTargetNucleus.GetZ_asInt()
	 << "  A= " << theTargetNucleus.GetA_asInt() << G4endl;
      DumpState(aTrack,"ApplyYourself",ed);
      ed << " ApplyYourself failed" << G4endl;
      G4Exception("G4HadronicProcess_SaG4n::PostStepDoIt", "had006", FatalException,
		  ed);
    }

    // Check the result for catastrophic energy non-conservation
    result = CheckResult(thePro, theTargetNucleus, result);

    if(reentryCount>100) {
      G4ExceptionDescription ed;
      ed << "Call for " << anInteraction->GetModelName() << G4endl;
      ed << "Target element "<<anElement->GetName()<<"  Z= "
	 << theTargetNucleus.GetZ_asInt()
	 << "  A= " << theTargetNucleus.GetA_asInt() << G4endl;
      DumpState(aTrack,"ApplyYourself",ed);
      ed << " ApplyYourself does not completed after 100 attempts" << G4endl;
      G4Exception("G4HadronicProcess_SaG4n::PostStepDoIt", "had006", FatalException,
		  ed);
    }
  }
  while(!result);  /* Loop checking, 30-Oct-2015, G.Folger */

  // Check whether kaon0 or anti_kaon0 are present between the secondaries: 
  // if this is the case, transform them into either kaon0S or kaon0L,
  // with equal, 50% probability, keeping their dynamical masses (and
  // the other kinematical properties). 
  // When this happens - very rarely - a "JustWarning" exception is thrown.
  // Because Fluka-Cern produces kaon0 and anti_kaon0, we reduce the number
  // of warnings to max 1 per thread.
  G4int nSec = (G4int)result->GetNumberOfSecondaries();
  if ( nSec > 0 ) {
    for ( G4int i = 0; i < nSec; ++i ) {
      auto dynamicParticle = result->GetSecondary(i)->GetParticle();
      auto part = dynamicParticle->GetParticleDefinition();
      if ( part == G4KaonZero::Definition() || 
           part == G4AntiKaonZero::Definition() ) {
        G4ParticleDefinition* newPart;
        if ( G4UniformRand() > 0.5 ) { newPart = G4KaonZeroShort::Definition(); }
        else { newPart = G4KaonZeroLong::Definition(); }
        dynamicParticle->SetDefinition( newPart );
      }
    }
  }

  result->SetTrafoToLab(thePro.GetTrafoToLab());
  FillResult(result, aTrack);

  if (epReportLevel != 0) {
    CheckEnergyMomentumConservation(aTrack, theTargetNucleus);
  }
  //G4cout << "PostStepDoIt done nICelectrons= " << nICelectrons << G4endl;
  return theTotalResult;
}
