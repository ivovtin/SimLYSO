//#include </Users/macalex/geant4/geant4.10.02.p02-install/include/Geant4/CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/SystemOfUnits.h>

#include <G4Event.hh>
#include <G4RunManager.hh>
#include <G4EventManager.hh>
#include <G4HCofThisEvent.hh>
#include <G4VHitsCollection.hh>
#include <G4VVisManager.hh>
#include <G4TrajectoryContainer.hh>
#include <G4Trajectory.hh>
#include <G4UImanager.hh>
#include <G4SDManager.hh>
#include <G4ios.hh>
#include "Histo.hh"
#include <G4ThreeVector.hh>
#include "G4Run.hh"
#include <G4INCLParticle.hh>

#include <sstream>
#include <iomanip>

#include <G4GeometryManager.hh>
#include <G4SolidStore.hh>
#include <G4LogicalVolumeStore.hh>
#include <G4PhysicalVolumeStore.hh>

#include <TTree.h>
#include <TFile.h>
#include <TH1.h>

#include "ScOpticalHit.hh"
#include "ScintHit.hh"

#include "g4root.hh"
#include <iostream>

#include "Histo.hh"

#include "g4root.hh"
#include "TString.h"

#include "DetectorConstruction_SiPM.hh"
#include "G4GenericMessenger.hh"

//locale
#include "EventAction.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "PhysicsList.hh"
#include "ExtraPhysics.hh"
#include "OpticalPhysics.hh"
#include "DetectorConstruction_SiPM.hh"
#include "CathodeSD.hh"
#include "ScintillatorSD.hh"
#include "PrimaryGeneratorAction.hh"
#include "EventActionMessenger.hh"

using namespace std;

extern int nTimeHistNumber;

std::vector<int> vTime;
    struct data_t
    {
        int number;
        int amp[1024];
        int ti[1024];
        float sum;
    } dat;
TH1F* tsp;

using namespace CLHEP;

EventAction::EventAction(PrimaryGeneratorAction* generator, RunAction* action, int _idVerbose):DetectorRID(-1), DetectorLID(-1), EnHitsCollID(-1), NumHitsCollID(-1), primary(generator), runAction(action)
{
  verbose    = false;
        verboseTop = false;
        idVerbose = _idVerbose;
        if( idVerbose==1 ){
                verbose = true;
        } else if( idVerbose==2 ){
                 verbose    = true;
                 verboseTop = true;
        }
        if( verbose )
                G4cout << "\n\t => method EventAction::EventAction() starting" << G4endl;

  fOutRoot = NULL;

  actionMessenger = new EventActionMessenger(this);
  //primary   = (PrimaryGeneratorAction*) G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction(); // link to gun
  //runAction = (RunAction*) G4RunManager::GetRunManager()->GetUserRunAction(); // link to runAction
  detector  = (DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction(); // link to detector

  dat_tree = new TTree("tt","G4sim results");
  dat_tree->Branch("data",&dat.number,"number/i:amp[1024]/i:ti[1024]/i:sum/f");
  tsp = new TH1F("tsp","Time spectrum",100000,0,100000);
  tsp -> SetXTitle("Time, ps");
}

EventAction::~EventAction()
{
	delete actionMessenger;

	dat_tree->Write();
	tsp->Write();
	dat_tree->Reset();
	tsp->Reset();
	fOutRoot->Close();


	if( verbose )
                G4cout << "\t => method EventAction::~EventAction() done" << G4endl;
}

void EventAction::BeginOfEventAction(const G4Event* evt)
{
     if( verboseTop )
                G4cout << "\n\t => method EventAction::BeginOfEventAction() starting" << G4endl;

    int evtNb = evt->GetEventID();
    dat.number = evtNb;
    vTime.clear();

    G4SDManager * SDman = G4SDManager::GetSDMpointer();

    if(DetectorRID == -1) { DetectorRID = SDman -> GetCollectionID("sensitiveDetectorR");
		G4cout << "DetectorRID = " << DetectorRID << G4endl;
		 }
    if(DetectorLID == -1) { DetectorLID = SDman -> GetCollectionID("sensitiveDetectorL"); 
			G4cout << "DetectorLID = " << DetectorLID << G4endl;
}
    if(EnHitsCollID == -1) { EnHitsCollID = SDman -> GetCollectionID("EnHitsCollection"); }
    if(NumHitsCollID == -1) { NumHitsCollID = SDman -> GetCollectionID("NumHitsCollection"); }

    if (evtNb%1000 == 0) {G4cout << "\n---> Begin of Event: " << evtNb << G4endl;}
    if( verboseTop )
                G4cout << "\t => method EventAction::BeginOfEventAction() done" << G4endl;
}


void EventAction::EndOfEventAction(const G4Event* evt)
{
  if( verboseTop )
                G4cout << "\n\t => method EventAction::EndOfEventAction() starting" << G4endl;

  if( verbose )
  G4cout << "\n---> GetArmAngle(): " << int(detector->GetArmAngle()*180/3.14) << G4endl;
  if( verbose )
  G4cout << "\n---> GetNumberPrimaryEvents(): " << runAction->GetNumberPrimaryEvents() << G4endl;
  if( verbose )
  G4cout << "\n---> GetEnergy: " << primary->GetEnergy()/MeV << G4endl;
  if( verbose )
  G4cout << "\n---> GetPositionY(): " << primary->GetPositionY() << G4endl;

  if( verbose )
  G4cout << "\n---> c_light: " << CLHEP::c_light  << G4endl;     //c   = 299.792458 mm/ns , 2.99792458e+8 * m/s; static const double c_light   = 2.99792458e+8 * m/s;
  //G4cout << "\n---> c_light: " << c_light * m/s << G4endl;     //c   = 299.792458 mm/ns , 2.99792458e+8 * m/s;

  G4HCofThisEvent* HCE = evt->GetHCofThisEvent();
  ScOpticalHitsCollection* OpticalHitsColl = 0;
  ScintHitsCollection*     EnergyHitsColl = 0;
  ScintHitsCollection*     NumHitsColl = 0;

  G4double gamma;
  G4double beta;
  G4double E;
  G4double Lenght;

  E=primary->GetEnergy()/MeV;
  gamma=E/primary->GetMass();
  beta=sqrt(1-1/(gamma*gamma));
  Lenght=abs(primary->GetPositionY());

  G4double dt;
  dt = Lenght*100/(beta*c_light*cos(detector->GetArmAngle())); // mm*100/mm/ns -> ps
  if(detector->GetArmAngle()<0) dt=-dt;

  if( verbose )
  G4cout << "\n---> dt(ps)=: " << dt << " cos()=" << cos(detector->GetArmAngle()) << " c_light(mm/ns)=" << CLHEP::c_light << " Mass(MeV)=" << primary->GetMass() << " beta=" << beta << " Lenght(mm)=" << Lenght << G4endl;


  //==============================================================================
  G4AnalysisManager* man = G4AnalysisManager::Instance();

  //==============================================================================

  if(HCE)
  {
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	if(DetectorRID != -1) {OpticalHitsColl = static_cast<ScOpticalHitsCollection*>(HCE->GetHC(DetectorRID));
	G4cout << "DETECTORRIGHT: "<<DetectorRID << G4endl;}
	if (OpticalHitsColl)
	{
		G4int nOptHits = OpticalHitsColl -> entries();
		G4cout << "DETECTORRIGHT " << nOptHits << G4endl;
		for (G4int iHit = 0; iHit<nOptHits; iHit++)
		{
			int nTime = static_cast<int>((*OpticalHitsColl)[iHit]->GetTime() / CLHEP::picosecond);
			vTime.push_back(nTime);

			G4String creator = (*OpticalHitsColl)[iHit] -> GetProcessName();
			G4cout << "DETECTORRIGHT " << creator << G4endl;
			if (creator == "Scintillation") { man->FillH1(man->GetH1Id("sctRight"), nTime, 1);  tsp->Fill(nTime);
			}
			if (creator == "Cerenkov") { man->FillH1(man->GetH1Id("chtRight"), nTime, 1); tsp->Fill(nTime);
			}
			      G4ThreeVector xyzCoor = (*OpticalHitsColl)[iHit]->GetPosition();
				G4cout << xyzCoor.z()/mm << G4endl;
//ll			      G4ThreeVector xyzCoor = (*ScHitsColl)[iHit]->GetPos();
		}
		//get time distributions
		std::sort(vTime.begin(), vTime.end());
//		int nHLength = std::min((int)vTime.size(), nTimeHistNumber);
		int nHLength = (int)vTime.size();
		G4cout<<"nHLength:"<<nHLength<<G4endl;
		for (int k = 0; k < 1024; k++){
		int n=0;
		dat.ti[k]=k;
		 for (int i=0; i<nHLength; i++) // (int vt : vTime )
		 {
			int vt=vTime[i];
			if ( vt>(200*k) && vt<(200*(k+1)) ) {dat.amp[k]++; n++;}
		 }
		dat.sum=+n/1024;
		}
	}

	if(DetectorLID != -1) OpticalHitsColl = static_cast<ScOpticalHitsCollection*>(HCE->GetHC(DetectorLID));
	if (OpticalHitsColl)
	{
		G4int nOptHits = OpticalHitsColl->entries();

		for (G4int iHit = 0; iHit<nOptHits; iHit++)
		{
			int nTime = static_cast<int>((*OpticalHitsColl)[iHit]->GetTime() / CLHEP::picosecond);
			vTime.push_back(nTime);

			G4String creator = (*OpticalHitsColl)[iHit]->GetProcessName();
			G4cout << "DETECTORLEFT " << creator << G4endl;
			if (creator == "Scintillation") { man->FillH1(man->GetH1Id("sctLeft"), nTime, 1);  tsp->Fill(nTime);
			}
			if (creator == "Cerenkov") { man->FillH1(man->GetH1Id("chtLeft"), nTime, 1); tsp->Fill(nTime);
			}
			      G4ThreeVector xyzCoor = (*OpticalHitsColl)[iHit]->GetPosition();
				G4cout << xyzCoor.z()/mm << G4endl;
//ll			      G4ThreeVector xyzCoor = (*ScHitsColl)[iHit]->GetPos();
		}
		//get time distributions
		std::sort(vTime.begin(), vTime.end());
//		int nHLength = std::min((int)vTime.size(), nTimeHistNumber);
		int nHLength = (int)vTime.size();
		G4cout<<"nHLength:"<<nHLength<<G4endl;
		for (int k = 0; k < 1024; k++){
		int n=0;
		dat.ti[k]=k;
		 for (int i=0; i<nHLength; i++) // (int vt : vTime )
		 {
			int vt=vTime[i];
			if ( vt>(200*k) && vt<(200*(k+1)) ) {dat.amp[k]++; n++;}
		 }
		dat.sum=+n/1024;
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	if(EnHitsCollID != -1) EnergyHitsColl  = static_cast<ScintHitsCollection*>(HCE->GetHC(EnHitsCollID));
	G4double Edep = 0;
	if (EnergyHitsColl)
	{
		G4int nHits = EnergyHitsColl->entries();
		for (G4int iHit = 0; iHit<nHits; iHit++)
		{
			Edep += (*EnergyHitsColl)[iHit]->GetEnergy();
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	if (NumHitsCollID != -1) NumHitsColl = static_cast<ScintHitsCollection*>(HCE->GetHC(NumHitsCollID));
	if (NumHitsColl)
	{
                G4int nOptic = 0;   //number optic phot.
		G4int nHits = NumHitsColl->entries();
		for (G4int iHit = 0; iHit<nHits; iHit++)
		{
			std::string partName = (*NumHitsColl)[iHit]->GetName();
			double kineticEnergy = (*NumHitsColl)[iHit]->GetEnergy() / CLHEP::keV;
			double dTime = (*NumHitsColl)[iHit]->GetTime() / CLHEP::picosecond;

                        //G4cout << "\n---> Begin of Event: " << partName << G4endl;
                        //G4cout << "\n---> iHit: " << iHit << G4endl;

			if (kineticEnergy > 0)
			{
				if (partName == "gamma")
				{
                                        //man->FillH1(0, kineticEnergy);
                                        man->FillNtupleDColumn(0, kineticEnergy);
				}
				else if (partName == "e-")
				{
                                        man->FillNtupleDColumn(1, kineticEnergy);
                                        man->FillH1(man->GetH1Id("ene"), kineticEnergy);
				}
   			        else if (partName == "e+")
				{
                                        man->FillNtupleDColumn(2, kineticEnergy);
				}
		         }
		}
	}
  }
  man->AddNtupleRow();
  dat_tree->Fill();

  if( verbose )
  cout << "EventAction::EndOfEventAction. Event -> " << evt->GetEventID() << endl;
  if( verbose )
  cout << "EventAction::EndOfEventAction. Run -> " << runAction->GetRun() << endl;

/*
  if( fOutRoot!=NULL ){
	dat_tree->Write();
	tsp->Write();
	dat_tree->Reset();
	tsp->Reset();

	fOutRoot->Close();
*/
	if( verbose )
              G4cout << "\t\t -> data is recorded" << G4endl;
  //}

  if( verboseTop )
                G4cout << "\t => method EventAction::EndOfEventAction() done" << G4endl;
}

//--------------------------------------
// OpenRootFile
//--------------------------------------
void EventAction::OpenRootFile(){
        if( verbose )
                G4cout << "\n\t => method EventAction::OpenRootFile() starting" << G4endl;

        int compressLevel = 1;

        fOutRoot = new TFile(nameRootFile.c_str(), "RECREATE", NULL, compressLevel);
    	if( !fOutRoot->IsOpen() ){
                G4cerr << "Can`t open output root file: " << nameRootFile.c_str() << " !" << G4endl;
                fOutRoot = NULL;
        } else if( verbose ){
                G4cout << "\t\t -> fOutRoot file is opened as \"" << nameRootFile.c_str() << "\"" << G4endl;
        }

        if( verbose )
                G4cout << "\t => method EventAction::OpenRootFile() done" << G4endl;
        return;
}


