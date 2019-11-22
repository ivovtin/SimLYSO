#include <iostream>
using namespace std;

// Make this appear first!
#include "G4Timer.hh"

#include "RunAction.hh"
#include "G4Run.hh"
#include <G4RunManager.hh>
#include "G4UImanager.hh"
#include "G4ios.hh"
#include <sstream>
#include <iomanip>
#include "PrimaryGeneratorAction.hh"
#include "HistoManager.hh"

#include "Histo.hh"

#include "g4root.hh"
#include "TString.h"

#include "DetectorConstruction_SiPM.hh"
#include "G4GenericMessenger.hh"

RunAction::RunAction(PrimaryGeneratorAction* pga)
:fMessenger(0)
{
  nPrimaryEvents = -1;
  timer = new G4Timer;
  pGenerator = pga;
  fHistoManager = new HistoManager();
  detector  = (DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction(); // link to detector
}

RunAction::~RunAction()
{
  delete timer;
  delete fHistoManager;
}

void RunAction::BeginOfRunAction(const G4Run* aRun)  //вызывается в начале каждого запуска
{
  nPrimaryEvents = aRun->GetNumberOfEventToBeProcessed();
  //выводит на экран порядковый номер запуска (0, 1 и т. д.)
  cout << "RunAction::BeginOfRunAction. Run " << aRun->GetRunID() << " start." << endl;
  timer->Start();

  fRunID = aRun->GetRunID();

  //histograms
  //
  //G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  //if (analysisManager->IsActive()) { analysisManager->OpenFile(); }

  //=====================================================================================
  // Get analysis manager
  G4AnalysisManager* man = G4AnalysisManager::Instance();

  ex_energy=pGenerator->GetParticleEnergy_keV()/1000;
  G4String filename = "Result_" + std::to_string(ex_energy) + "MeV_" + std::to_string(int(detector->GetArmAngle()*180/3.14)) + "deg.root";
  // Open an output file
  man->OpenFile(filename);
  //man->OpenFile("exampleRoot");
  // Create histogram(s)
  //man->CreateH1("0","Edep in absorber", 100, 0., 800*MeV);
  //man->CreateH1("0","Edep in absorber", 1000, 0., 10000000);
  /*
  man->SetFirstHistoId(0);
  man->CreateH1("Time","Time", 100, 0., 10000000);
  //man->CreateH1("1","Edep in gap", 1000, 0., 10000000);
  man->CreateH2("0","Number of photons # Time", 100, 0., 10000000.,100,0.,10000000.);
  */
  // Create ntuple
  //man->SetFirstNtupleId(0);
  man->CreateNtuple("Energy", "Energy");
  man->CreateNtupleDColumn("gamma");
  man->CreateNtupleDColumn("electron");
  man->CreateNtupleDColumn("positron");
  man->FinishNtuple();
  //man->CreateNtuple("Number", "Number");
  //man->CreateNtupleDColumn("opticalphoton");
  //man->CreateNtupleDColumn("time");
  //man->FinishNtuple();

  man->CreateH1("ene","Energy", 100, 0., 10000000);
  man->CreateH1("chtRight","chTimeRight", 100000, 0., 100000);
  man->SetH1XAxisTitle(man->GetH1Id("chtRight"),"Time, ps");
  man->CreateH1("sctRight","scTimeRight", 100000, 0., 100000);
  man->SetH1XAxisTitle(man->GetH1Id("sctRight"),"Time, ps");
  man->CreateH1("chtLeft","chTimeLeft", 100000, 0., 100000);
  man->SetH1XAxisTitle(man->GetH1Id("chtLeft"),"Time, ps");
  man->CreateH1("sctLeft","scTimeLeft", 100000, 0., 100000);
  man->SetH1XAxisTitle(man->GetH1Id("sctLeft"),"Time, ps");
  //man->CreateH1("optphot","optTime", 100000, 0., 100000);
  //man->SetH1XAxisTitle(man->GetH1Id("optphot"),"Time, ps");
  man->CreateH1("chsp","chSpectrum", 900, 100., 1000.);
  man->SetH1XAxisTitle(man->GetH1Id("chsp"),"WL, nm");
  man->CreateH1("scsp","scSpectrum", 900, 100., 1000.);
  man->SetH1XAxisTitle(man->GetH1Id("scsp"),"WL, nm");
  //man->CreateH2("evst","Energy # Time", 100, 0., 10000000.,100,0.,10000000.);
  //man->SetH2XAxisTitle(man->GetH1Id("evst"),"Energy, keV");
  //man->SetH2YAxisTitle(man->GetH1Id("evst"),"Time, ps");
  //=====================================================================================
}

void RunAction::EndOfRunAction(const G4Run* aRun)
{
  timer->Stop();
  cout << "RunAction::EndOfRunAction. number of event = " << aRun->GetNumberOfEvent() << endl;
  cout << " " << *timer << endl;

  //save histograms
  //G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  //if (analysisManager->IsActive())
  //{
	 // analysisManager->Write();
	 // analysisManager->CloseFile();
  //}

  //======================================================================================
  G4AnalysisManager* man = G4AnalysisManager::Instance();
  man->Write();
  man->CloseFile();
  delete G4AnalysisManager::Instance();
  //=======================================================================================
}


