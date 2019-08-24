#ifndef ScOpticalHit_h
#define ScOpticalHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

class ScOpticalHit : public G4VHit
{
public:
  
  ScOpticalHit();
  ~ScOpticalHit();
  ScOpticalHit(const ScOpticalHit&);
  const ScOpticalHit& operator=(const ScOpticalHit&);
  int operator==(const ScOpticalHit&) const;
  
  inline void* operator new(size_t);
  inline void  operator delete(void*);
  
  void Draw();
  void Print();

private:
  
  G4double      fPhotEne;     // Photon energy energy 
  G4double      fPhotTime;    // time of hit
  G4ThreeVector fPhotPos;     // Position of the hit
  G4String      fProcessName; //Creator process name  

public:

  // Set functions to store information on hits  
  inline void SetEnergy(G4double fEn)   {fPhotEne= fEn;}
  inline void SetTime(G4double fTm)   {fPhotTime= fTm;}
  inline void SetPosition(G4ThreeVector xyz) {fPhotPos = xyz;}
  inline void SetProcessName(G4String name) {fProcessName = name;}

  // Get functions to acess information on hits
  inline G4double      GetEnergy()     {return fPhotEne; }
  inline G4double      GetTime()       {return fPhotTime; }
  inline G4ThreeVector GetPosition()   {return fPhotPos; } 
  inline G4String      GetProcessName(){return fProcessName;}

};


typedef G4THitsCollection<ScOpticalHit> ScOpticalHitsCollection;
extern G4Allocator<ScOpticalHit> ScOpticalHitAllocator;


inline void* ScOpticalHit::operator new(size_t)
{
    return (void*)ScOpticalHitAllocator.MallocSingle();  
}


inline void ScOpticalHit::operator delete(void* aHit)
{
  ScOpticalHitAllocator.FreeSingle((ScOpticalHit*) aHit);
}

#endif
