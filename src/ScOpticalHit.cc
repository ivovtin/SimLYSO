#include "ScOpticalHit.hh"
#include "G4SystemOfUnits.hh"

G4Allocator<ScOpticalHit> ScOpticalHitAllocator;


ScOpticalHit::ScOpticalHit()
{

  fPhotEne      = 0.0;
  fPhotPos      = G4ThreeVector();
  fPhotTime     = 0.0;
  fProcessName  = "";
}


ScOpticalHit::~ScOpticalHit()
{;}


ScOpticalHit::ScOpticalHit(const ScOpticalHit& right) : G4VHit()
{
  fPhotEne     =  right.fPhotEne;
  fPhotPos     =  right.fPhotPos;
  fPhotTime    =  right.fPhotTime;
  fProcessName =  right.fProcessName;

}


const ScOpticalHit& ScOpticalHit::operator=(const ScOpticalHit& right)
{
  fPhotEne =  right.fPhotEne;
  fPhotPos =  right.fPhotPos;
  fPhotTime    =  right.fPhotTime;
  fProcessName =  right.fProcessName;
  return *this;
}


int ScOpticalHit::operator==(const ScOpticalHit& right) const
{
  return (this==&right) ? 1 : 0;
}


void ScOpticalHit::Draw()
{;}


void ScOpticalHit::Print()
{
  G4cout << fPhotEne/keV << G4endl;
  G4cout << fPhotPos/mm  << G4endl;
}

