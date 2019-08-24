#ifndef ScintillatorSD_h
#define ScintillatorSD_h 1

#include <G4VSensitiveDetector.hh>

class G4Step;
class G4HCofThisEvent;

#include "ScintHit.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class ScintillatorSD : public G4VSensitiveDetector
{
public:
	ScintillatorSD(G4String);
	~ScintillatorSD();

	void   Initialize(G4HCofThisEvent*);
	G4bool ProcessHits(G4Step*, G4TouchableHistory*);
	void   EndOfEvent(G4HCofThisEvent*);

private:

	ScintHitsCollection*  HitsCollection;
	ScintHitsCollection*  NumHitsCollection;
	int LastTrackID;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
