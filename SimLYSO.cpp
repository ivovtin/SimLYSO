#include <time.h>

#ifdef G4MULTITHREADED
#include <G4MTRunManager.hh>
#else
#include <G4RunManager.hh>
#endif

#include <G4UImanager.hh>
#include <G4UIterminal.hh>

#ifdef G4VIS_USE
#include <G4VisExecutive.hh>
#endif

#ifdef G4UI_USE
#include <G4UIExecutive.hh>
#endif

#include <Randomize.hh>
#include <G4PhysListFactory.hh>

//#include "DetectorConstruction.hh"
//#include "DetectorConstruction_BoxSc.hh"
#include "DetectorConstruction_SiPM.hh"
#include "PhysicsList.hh"
#include "RunAction.hh"
#include "PrimaryGeneratorAction.hh"
#include "EventAction.hh"
#include "TrackingAction.hh"
#include "StackingAction.hh"
#include "Histo.hh"

//#include <conio.h>
#include <iostream>
#include <sstream>

using namespace std;

void SaveVector(char* fname, std::vector<std::pair<double, double>>& vec)
{
	FILE* out = fopen(fname, "w");
	if (out)
	{
		for (int i = 0; i < int(vec.size()); ++i)
		{
			fprintf(out, "%E %E\n", vec[i].first, vec[i].second);
		}
		fclose(out);
	}
}


int nTimeHistNumber = 2;
std::vector<ST1dHistogramm*> vTimeHist;

int  nEvents       = 0; 
int  nUsefulEvents = 0;

int main(int argc,char** argv)
{
   //int verbose = 0;
   //int verbose = 1;
   int verbose = 2;  //Top and verbose
   
  // Choose the Random engine
  CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);
  // Seed the random number generator manually
  // -----------------------------------------
  G4long myseed = time(NULL);
  CLHEP::HepRandom::setTheSeed(myseed);
  cout << "Current time used as seed: " << myseed << G4endl;
  CLHEP::HepRandom::showEngineStatus();

  // Construct the default run manager
  G4RunManager* runManager = new G4RunManager;

  //set mandatory initialization classes
  //
  DetectorConstruction*   det  = new DetectorConstruction();
  // "QGSP_BIC_AllHP" - for neutrons
  PhysicsList*  phys = new PhysicsList("QGSP_BIC_AllHP");  

//  G4PhysListFactory factory;
//  G4VModularPhysicsList* phys = factory.GetReferencePhysList("QGSP_BIC_AllHP");
//  phys->SetVerboseLevel(3);

  runManager->SetUserInitialization(det);
  runManager->SetUserInitialization(phys);
  
  //set user action classes
  //
  PrimaryGeneratorAction* kin   = new PrimaryGeneratorAction(); 
  RunAction*              run   = new RunAction(kin);
  EventAction*            evt   = new EventAction(kin, run, verbose);
  TrackingAction*         treck = new TrackingAction(det);
  StackingAction*         steck = new StackingAction();
  
  runManager->SetUserAction(kin); 
  runManager->SetUserAction(run); 
  runManager->SetUserAction(evt);
  runManager->SetUserAction(treck);
  runManager->SetUserAction(steck);

  // visualization manager
  #ifdef G4VIS_USE
      G4VisManager* visManager = new G4VisExecutive;
      visManager -> Initialize ();
  #endif
 
  // get the pointer to the User Interface manager 
  G4UImanager *UImanager = G4UImanager::GetUIpointer();  

  std::vector<double> dEnergy;
////////////////////////////////////////
  if (dEnergy.size() > 0)
  {
	  for (int i = 0; i < int(dEnergy.size()); i++)
	  {
		  runManager->GeometryHasBeenModified();
		  runManager->PhysicsHasBeenModified();
		  runManager->Initialize();

	          std::stringstream strm;
		  strm << "/gun/energy " << dEnergy[i] << " keV";
		  UImanager->ApplyCommand(strm.str().c_str());

		  if (argc == 1)
		  {
			  //UI->ApplyCommand("/control/execute input.mac");
			  //UI->ApplyCommand("/control/execute init_vis_example.mac");
			// interactive mode : define UI session
			#ifdef G4UI_USE
      			G4UIExecutive* ui = new G4UIExecutive(argc, argv);
			#ifdef G4VIS_USE
      			UImanager->ApplyCommand("/control/execute init_vis_example.mac");
			#else
      			UImanager->ApplyCommand("/control/execute init_example.mac");
			#endif
      			ui->SessionStart();
      			delete ui;
			#endif
		  }
		  else
		  {
			  // Batch mode
			  G4String command = "/control/execute ";
			  G4String fileName = argv[1];
			  UImanager->ApplyCommand(command + fileName);
		  }
	  }
  }
  else
  {
	  //WHEN WE READ *.mac FILE WE SHOULD INITIALIZE IN IT
	  if (argc == 1)
	  {
		  //UI->ApplyCommand("/control/execute input.mac");
		  //UImanager->ApplyCommand("/control/execute init_vis_example.mac");
			#ifdef G4UI_USE
                        G4UIExecutive* ui = new G4UIExecutive(argc, argv);
                        #ifdef G4VIS_USE
                        UImanager->ApplyCommand("/control/execute init_vis_example.mac");
                        #else
                        UImanager->ApplyCommand("/control/execute init_example.mac");
                        #endif
                        ui->SessionStart();
                        delete ui;
                        #endif
	  }
	  else
	  {
		  // Batch mode
		  G4String command = "/control/execute ";
		  G4String fileName = argv[1];
		  UImanager->ApplyCommand(command + fileName);
	  }
  }

////////////////////////////////////////

  getchar();

  #ifdef G4VIS_USE
      delete visManager;
  #endif

  // job termination
  delete runManager;
 
  return 0;
}


