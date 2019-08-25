#include "CathodeSD.hh"

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

#include "Histo.hh"
#include "OptService.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

//#include </Users/macalex/geant4/geant4.10.02.p02-install/include/Geant4/CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/SystemOfUnits.h>

using namespace CLHEP;

CathodeSD::CathodeSD(G4String name) : G4VSensitiveDetector(name), HitsCollection(NULL)
{
	// In the constructor, define the name of the hits
	// collection which is handled by this sensitive detector.
	// In case your sensitive detector generates more than	one kinds of hits(e.g.anode and cathode hits
	// separately), define all collection names.
    collectionName.insert("ScHitsCollection");

	//Register that
	G4SDManager *sdman = G4SDManager::GetSDMpointer();
	sdman->AddNewDetector(this);
}

CathodeSD::~CathodeSD()
{
}

void CathodeSD::Initialize(G4HCofThisEvent* HCE)
{
	static G4int HCID = -1;

	_nHits          = 0;
	_nDetectedLight = 0;
	_nScintillation = 0;
	_nCherenkov     = 0;

    // init collections
    // SensitiveDetectorName and collectionName are data members of G4VSensitiveDetector
    HitsCollection = new ScOpticalHitsCollection(SensitiveDetectorName, collectionName[0]);
	// The SDM can return a hit collection ID (useful for fishing
	// out your collection from the hits collections of the event

	//Initialize() method is invoked at the beginning of each event.
	// Get the unique ID number for this collection.
	// GetCollectionID() is a heavy operation.It should not be used for	every events.
	// GetCollectionID() is available after this sensitive detector object is
	// constructed and registered to G4SDManager.Thus, this method
	// cannot be invoked in the constructor of this detector class.
	//  Instantiate hits collection(s) and attach it / them to G4HCofThisEvent
    //	object given in the argument.
    if(HCID<0){ HCID = GetCollectionID(0); }
    HCE->AddHitsCollection(HCID, HitsCollection);
}

G4bool CathodeSD::ProcessHits(G4Step* theStep, G4TouchableHistory*)
{
	//////////////////////////////////////////////////////////
	// find the boundary process only once
	//
	static G4OpBoundaryProcess* boundary = NULL;
	if (!boundary)
	{
		G4ProcessManager* pm = theStep->GetTrack()->GetDefinition()->GetProcessManager();
		G4int nprocesses = pm->GetProcessListLength();
		G4ProcessVector* pv = pm->GetProcessList();

		for (G4int i = 0; i<nprocesses; i++)
		{
			if ((*pv)[i]->GetProcessName() == "OpBoundary")
			{
				boundary = (G4OpBoundaryProcess*)(*pv)[i];
				break;
			}
		}
	}

	//////////////////////////////////////////////////////////
	//
	G4Track* track = theStep->GetTrack();
	if((track->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition()) && (track->GetParentID()>0))
	{
		// collect all information about track
		G4StepPoint * thePostPoint = theStep->GetPostStepPoint();
		//define deposition point
		G4ThreeVector xyzCoor = thePostPoint->GetPosition();

		//add information about hit to collection
		_nHits++;

		//Check to see if the partcile was actually at a boundary
		//Otherwise the boundary status may not be valid
		//Prior to Geant4.6.0-p1 this would not have been enough to check

		if (thePostPoint->GetStepStatus() == fGeomBoundary)
		{
			if (boundary)
			{
				G4OpBoundaryProcessStatus boundaryStatus = boundary->GetStatus();
				switch (boundaryStatus)
				{
				case Detection://Note, this assumes that the volume causing detection
							   //is the photocathode because it is the only one with
							   //non-zero efficiency. Triger sensitive detector manually since photon is
							   //absorbed but status was Detection
				{
					//count detected
					_nDetectedLight++;

					//add information about hit to collection
					G4StepPoint * thePostPoint  = theStep->GetPostStepPoint();
					//define deposition point
					G4ThreeVector xyzCoor       = thePostPoint->GetPosition();

					ScOpticalHit* OpticalHit = new ScOpticalHit;
					OpticalHit->SetEnergy(theStep->GetTrack()->GetKineticEnergy());
					OpticalHit->SetPosition(xyzCoor);
					OpticalHit->SetTime(theStep->GetTrack()->GetGlobalTime());

					//get information about creator
					const G4VProcess* creatorProcess = track->GetCreatorProcess();
					if (creatorProcess)
					{
						G4String creator = creatorProcess->GetProcessName();
                        OpticalHit->SetProcessName(creator);
					}

					HitsCollection->insert(OpticalHit);
				}
				break;

				case Absorption:
				case FresnelReflection:
				case TotalInternalReflection:
				case LambertianReflection:
				case LobeReflection:
				case SpikeReflection:
				case BackScattering:
					break;

				default:
					break;
				}
			}
		}//if Boundary

	}
	//////////////////////////////////////////////////////////

  return true;
}

G4bool CathodeSD::ProcessHits_OpticalGenerated(int nScintillationPhotons, int nCherenkovPhotons)
{
	_nScintillation += nScintillationPhotons;
	_nCherenkov += nCherenkovPhotons;
	return true;
}


void CathodeSD::EndOfEvent(G4HCofThisEvent* HCE)
{
	  static G4int HCID = -1;
	  if(HCID<0)
	  { 
	      HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
	  }
	  HCE->AddHitsCollection(HCID, HitsCollection);
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

