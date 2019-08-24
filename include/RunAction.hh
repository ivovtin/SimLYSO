#ifndef RunAction_h
#define RunAction_h 1

#include "globals.hh"
#include "G4UserRunAction.hh"

class G4Timer;
class G4Run;
class PrimaryGeneratorAction;
class DetectorConstruction;
class HistoManager;
class G4GenericMessenger;

class RunAction : public G4UserRunAction 
{
  public:
	  RunAction(PrimaryGeneratorAction*);
    ~RunAction();

  public:
    void BeginOfRunAction(const G4Run* aRun);
    void EndOfRunAction(const G4Run* aRun);
    
    void SetName(G4double val); 
    G4String fName;   
     
    G4double fRunID;  
    G4double GetRun() { return fRunID; }
    
    int GetNumberPrimaryEvents() const { return nPrimaryEvents; };

  private:
        G4GenericMessenger* fMessenger;
        DetectorConstruction *detector;
        
        int nPrimaryEvents;
     
        G4Timer* timer;
	PrimaryGeneratorAction* pGenerator;
	HistoManager*           fHistoManager;
        int ex_energy;
};

#endif

