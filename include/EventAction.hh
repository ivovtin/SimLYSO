#ifndef EventAction_h
#define EventAction_h 1

#include <G4UserEventAction.hh>
#include <G4ThreeVector.hh>
#include <globals.hh>

#include <TTree.h>
#include <TFile.h>
#include <TH1.h>

class G4Event;
class G4HCofThisEvent;
class PrimaryGeneratorAction;
class RunAction;
class DetectorConstruction;
class EventActionMessenger;

class EventAction : public G4UserEventAction
{
private:
        std::ostringstream o_str;
        G4String nameRootFile;
        EventActionMessenger* actionMessenger;
        bool zoomMode;

        int idVerbose;
        bool verbose, verboseTop;

        PrimaryGeneratorAction* primary;
        RunAction* runAction;
        DetectorConstruction *detector;

private:        
        G4int ScHitsCollID; 
	G4int EnHitsCollID;
	G4int NumHitsCollID;
	
	TFile* fOutRoot;
        TTree* dat_tree;     
        // Energy of primary particle
        double e_primary;
  public:
    	inline void SetNameRootFile(G4String _name){
                nameRootFile = _name;
                G4cout << "\tSetting name of the root file: " << nameRootFile << G4endl;
        }
        inline G4String GetNameRootFile() const { return nameRootFile; }
        inline void SetZoomMode(bool _zoomMode){
                zoomMode = _zoomMode;
                G4String _zoomModeStatus = "FALSE"; if( zoomMode ) _zoomModeStatus = "TRUE";
                G4cout << "\tSetting zoomMode: \"" << _zoomModeStatus << "\"" << G4endl;
        }

    EventAction(PrimaryGeneratorAction* generator, RunAction* action, int _idVerbose);
    virtual ~EventAction();

    virtual void   BeginOfEventAction(const G4Event*);
    virtual void   EndOfEventAction(const G4Event*);
    
    void OpenRootFile();
    //void PrintDataHC(bool _all);
    //int GetParticleId(std::string _name);  

};

#endif


