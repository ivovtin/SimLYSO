#include "StackingAction.hh"
#include "G4ios.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4Track.hh"
#include "G4VProcess.hh"
#include "Histo.hh"

#include "OptService.h"
#include "g4root.hh"

StackingAction::StackingAction(): gammaCounter(0)
{
}

StackingAction::~StackingAction()
{
}

G4ClassificationOfNewTrack
StackingAction::ClassifyNewTrack(const G4Track * aTrack)
{
  G4AnalysisManager* man = G4AnalysisManager::Instance();  

  double wL;
  if(aTrack->GetDefinition()==G4OpticalPhoton::OpticalPhotonDefinition())
  { // particle is optical photon
    if(aTrack->GetParentID()>0)
    { // particle is secondary
      gammaCounter++;

	  wL = OptService::eV2nm(aTrack->GetDynamicParticle()->GetKineticEnergy() / CLHEP::eV);
	  if (wL <= 300 || wL>=1000.)
	  {
		//  G4cout << "Strange particle produced in Cherenkov process: " << wL << " nm." << G4endl;
		  const_cast<G4Track*>(aTrack)->SetTrackStatus(fStopAndKill);
	  }

	  ////Sort accordingly generated process
	  if(aTrack->GetCreatorProcess()->GetProcessName() == "Scintillation")
	  {
		 // sciWlSpecter->AddEvent(wL);
	          man->FillH1(man->GetH1Id("scsp"), wL, 1); 
          }
	  else if(aTrack->GetCreatorProcess()->GetProcessName() == "Cerenkov")
	  {
		 // cherWlSpecter->AddEvent(wL);
                 man->FillH1(man->GetH1Id("chsp"), wL, 1);
	  }
    }
  }

  return fUrgent;
}

void StackingAction::NewStage()
{
	//optSpectrum->AddEvent(gammaCounter);
	if (gammaCounter > 0)
	{
		G4cout << "Number of gamma produces in this event : " << gammaCounter << G4endl;
	}
}

void StackingAction::PrepareNewEvent()
{ 
	gammaCounter = 0; 
}
