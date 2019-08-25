#include "ScintillatorSD.hh"

#include <G4HCofThisEvent.hh>
#include <G4Step.hh>
#include <G4ThreeVector.hh>
#include <G4Track.hh>
#include <G4DynamicParticle.hh>
#include <G4SDManager.hh>
#include <G4ios.hh>
#include <G4SDManager.hh>
#include <G4ParticleTypes.hh>
#include <G4ParticleDefinition.hh>
#include "G4OpBoundaryProcess.hh"
#include "G4ProcessManager.hh"
#include "G4RunManager.hh"

#include "OptService.h"

#include <iostream>
#include <fstream>
#include <vector>

//#include </Users/macalex/geant4/geant4.10.02.p02-install/include/Geant4/CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/SystemOfUnits.h>

using namespace CLHEP;

ScintillatorSD::ScintillatorSD(G4String name) : G4VSensitiveDetector(name), HitsCollection(NULL), NumHitsCollection(NULL)
{
	// In the constructor, define the name of the hits
	// collection which is handled by this sensitive detector.
	// In case your sensitive detector generates more than	one kinds of hits(e.g.anode and cathode hits
	// separately), define all collection names.
	collectionName.insert("EnHitsCollection");
	collectionName.insert("NumHitsCollection");

	G4SDManager *sdman = G4SDManager::GetSDMpointer();
	sdman->AddNewDetector(this);
}

ScintillatorSD::~ScintillatorSD()
{
}

void ScintillatorSD::Initialize(G4HCofThisEvent* HCE)
{
	static G4int HCID = -1;

	// init collections
	// SensitiveDetectorName and collectionName are data members of G4VSensitiveDetector
	HitsCollection = new ScintHitsCollection(SensitiveDetectorName, collectionName[0]);
	NumHitsCollection = new ScintHitsCollection(SensitiveDetectorName, collectionName[1]);
	// The SDM can return a hit collection ID (useful for fishing
	// out your collection from the hits collections of the event

	//Initialize() method is invoked at the beginning of each event.
	// GetCollectionID() is a heavy operation.It should not be used for	every events.
	// GetCollectionID() is available after this sensitive detector object is
	// constructed and registered to G4SDManager.
	//Argument: order of the collection as stored in CollectionName
	HCID = GetCollectionID(0); 
	HCE->AddHitsCollection(HCID, HitsCollection);
	HCID = GetCollectionID(1);
	HCE->AddHitsCollection(HCID, NumHitsCollection);

	LastTrackID = -1;

}

G4bool ScintillatorSD::ProcessHits(G4Step* theStep, G4TouchableHistory*)
{
	try {

		// collect all information about track
		G4Track *aTrack = theStep->GetTrack();
		G4StepPoint* thePostPoint = theStep->GetPostStepPoint();
		G4StepPoint* thePrePoint = theStep->GetPreStepPoint();

		//Add hit info to collection
		ScintHit* Hit = new ScintHit;
		Hit->SetEnergy(theStep->GetTotalEnergyDeposit());

		if (thePostPoint == NULL)
		{
			G4cout << "BUGS!!!!!!!!" << G4endl;
		}

		Hit->SetPos(thePostPoint->GetPosition());
		Hit->SetTime(aTrack->GetGlobalTime());
		HitsCollection->insert(Hit);

		//Get particle name
		G4String partName = aTrack->GetDefinition()->GetParticleName();

		if (LastTrackID != aTrack->GetTrackID())
		{
			LastTrackID = aTrack->GetTrackID();
			//collect information about number of inital hits
			ScintHit* Hit2 = new ScintHit;
			Hit2->SetEnergy(aTrack->GetKineticEnergy());
			Hit2->SetName(partName);
			Hit2->SetPos(thePostPoint->GetPosition());
			Hit2->SetTime(aTrack->GetGlobalTime());
			NumHitsCollection->insert(Hit2);
		}
	}

	catch (std::exception& excp)
	{
		G4cout << excp.what() << G4endl;
	}

	catch(...)
	{
		G4cout << "Unhandled exception" << G4endl;
	}
	return true;
}


void ScintillatorSD::EndOfEvent(G4HCofThisEvent*)
{
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

