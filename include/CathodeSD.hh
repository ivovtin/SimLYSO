#ifndef CathodeSD_h
#define CathodeSD_h 1

#include <G4VSensitiveDetector.hh>

class G4Step;
class G4HCofThisEvent;

#include "ScOpticalHit.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class CathodeSD : public G4VSensitiveDetector
{
  public:
      CathodeSD(G4String);
     ~CathodeSD();

      void   Initialize(G4HCofThisEvent*);
      G4bool ProcessHits(G4Step*, G4TouchableHistory*);
      G4bool ProcessHits_OpticalGenerated(int,int);
      void   EndOfEvent(G4HCofThisEvent*);

  private:
	//  int    Num;
	  G4String Name;
	  int   _nHits;
	  int   _nDetectedLight;
	  int   _nScintillation;
	  int   _nCherenkov;

	  //arriviat time of the first photons in event
	  int   nMaxOffset;

//	  std::vector<double> vWeight;
//	  std::vector<double> vShape;
      ScOpticalHitsCollection*  RHitsCollection;
      ScOpticalHitsCollection*  LHitsCollection;
      ScOpticalHitsCollection*  HitsCollection;
      ScOpticalHitsCollection*  HitsCollection2;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

