#include "ScintHit.hh"

//#include </Users/macalex/geant4/geant4.10.02.p02-install/include/Geant4/CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/SystemOfUnits.h>

#include <G4UnitsTable.hh>
#include <G4VVisManager.hh>
#include <G4VisAttributes.hh>

using namespace CLHEP;

G4Allocator<ScintHit> ScintHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ScintHit::ScintHit()
{
	name = "";
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ScintHit::~ScintHit() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ScintHit::ScintHit(const ScintHit& right) : G4VHit()
{
  time       = right.time;
  energy     = right.energy;
  pos        = right.pos;
  name       = right.name;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const ScintHit& ScintHit::operator=(const ScintHit& right)
{
  time       = right.time;
  energy     = right.energy;
  pos        = right.pos;
  name       = right.name;
  return *this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4int ScintHit::operator==(const ScintHit& right) const
{
  return (this==&right) ? 1 : 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ScintHit::Draw()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ScintHit::Print(std::ostream &stream, bool printtime, bool printposition, bool printenergy)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

