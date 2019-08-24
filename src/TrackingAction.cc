#include "TrackingAction.hh"

#include "G4TrackingManager.hh"
#include "G4Track.hh"
#include "G4ParticleTypes.hh"

#include "CathodeSD.hh"
#include "DetectorConstruction_SiPM.hh"

//#include </Users/macalex/geant4/geant4.10.02.p02-install/include/Geant4/CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/SystemOfUnits.h>
#include "G4OpBoundaryProcess.hh"
#include "G4Scintillation.hh"

#include "OptService.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TrackingAction::TrackingAction(DetectorConstruction* myDC)
:myDetector(myDC)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TrackingAction::PreUserTrackingAction(const G4Track* aTrack)
{
	//if (aTrack->GetParentID() > 0)
	//{
	//	G4cout << "[Em10TrackingAction::DEBUG]" << G4endl;
	//	G4cout << " Track ID:          " << aTrack->GetTrackID() << G4endl;
	//	G4cout << " particle:          " << aTrack->GetDynamicParticle()->GetDefinition()->GetParticleName() << G4endl;
	//	G4cout << " Parent ID:         " << aTrack->GetParentID() << G4endl;
	//	G4cout << " created by:        " << aTrack->GetCreatorProcess()->GetProcessName() << G4endl;
	//	G4cout << " kin. energy (keV): " << aTrack->GetKineticEnergy() / CLHEP::keV << G4endl;
	//	G4cout << " volume:            " << aTrack->GetVolume()->GetName() << G4endl;
	//	G4cout << " global time:       " << aTrack->GetGlobalTime() << G4endl;

	//	G4cout << " Killing event..." << G4endl;
	//	if (aTrack->GetTrackID() != 1)
	//	{
	//		const_cast<G4Track*>(aTrack)->SetTrackStatus(fKillTrackAndSecondaries);
	//	}
	//}

	////find process 
	//static G4Scintillation* process = nullptr;
	//G4ProcessManager* pm = aTrack->GetDefinition()->GetProcessManager();
	//G4int nprocesses = pm->GetProcessListLength();
	//G4ProcessVector* pv = pm->GetProcessList();

	//for (G4int i = 0; i < nprocesses; i++)
	//{
	//	if ((*pv)[i]->GetProcessName() == "Scintillation")
	//	{
	//		if (process == nullptr)
	//		{
	//			process = (G4Scintillation*)(*pv)[i];
	//		}
	//		break;
	//	}
	//}

	//if (aTrack->GetCurrentStepNumber() == 0) return;

	//const G4Material* aMaterial = nullptr;
	//G4PhysicsVector* ScintillationIntegral = nullptr;
	//aMaterial = aTrack->GetMaterial();
	//if (process != nullptr && aMaterial!=nullptr)
	//{
	//	G4int materialIndex = aMaterial->GetIndex();
	//	G4MaterialPropertiesTable* aMaterialPropertiesTable = aMaterial->GetMaterialPropertiesTable();
	//	G4MaterialPropertyVector* Fast_Intensity = aMaterialPropertiesTable->GetProperty("FASTCOMPONENT");

	//	G4PhysicsTable* fTable = process->GetFastIntegralTable();
	//	ScintillationIntegral = (G4PhysicsVector*)((*fTable)(materialIndex));
	//}

	//std::ofstream myfile;
	//myfile.open("D://SPectrum_data.txt");
	//for (size_t ii = 0;	ii < ScintillationIntegral->GetVectorLength();	++ii)
	//{
	//	double currentE = ScintillationIntegral->Energy(ii);
	//	double currentV = (*ScintillationIntegral)[ii];
	//	myfile << currentE << "  " << currentV << std::endl;
	//}
	//myfile.close();

	//ScintillationIntegral->SetSpline(true);
	//myfile.open("D://SPectrum_spline1.txt");
	//for (double d = 0.0; d <= 10E-6; d += 1E-9)
	//{
	//	double dRes = ScintillationIntegral->GetEnergy(d);
	//	myfile << d << "  " << dRes << std::endl;
	//}
	//myfile.close();


	int fScintillationCounter = 0;
	int fCerenkovCounter = 0;

	if (aTrack->GetParentID() > 0)
	{
		if (aTrack->GetDynamicParticle()->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition())
		{
			if (aTrack->GetCreatorProcess()->GetProcessName() == "Scintillation") { fScintillationCounter++; }
			if (aTrack->GetCreatorProcess()->GetProcessName() == "Cerenkov"){ fCerenkovCounter++; }
		}
	}

	//count optical photons
	CathodeSD* pmtSD = dynamic_cast<CathodeSD*>(myDetector->GetSensetiveDetector());
	if (pmtSD)pmtSD->ProcessHits_OpticalGenerated(fScintillationCounter, fCerenkovCounter);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TrackingAction::PostUserTrackingAction(const G4Track* aTrack)
{
}
