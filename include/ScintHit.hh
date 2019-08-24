#ifndef ScintHit_h
#define ScintHit_h 1

#include <G4VHit.hh>
#include <G4THitsCollection.hh>
#include <G4Allocator.hh>
#include <G4ThreeVector.hh>
#include <string>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class ScintHit : public G4VHit
{
  public:

      ScintHit();
     ~ScintHit();
      ScintHit(const ScintHit&);
      const ScintHit& operator=(const ScintHit&);
      G4int operator==(const ScintHit&) const;

      inline void* operator new(size_t);
      inline void  operator delete(void*);

      void Draw();
      void Print(std::ostream &stream = G4cout, bool printtime = true, bool printposition = false, bool printenergy = false);

  public:

      void SetTime(G4double t)            { time = t; }
      void SetEnergy   (G4double en)      { energy = en; }
	  void SetPos      (G4ThreeVector xyz){ pos = xyz; }
	  void SetName(std::string n)		  { name = n; }

      G4double GetTime()  { return time; }
      G4double GetEnergy()    { return energy; }
	  G4ThreeVector GetPos(){ return pos; }
	  std::string& GetName() { return name; }
      
  private:
  
      G4double      time;
      G4double      energy;
      G4ThreeVector pos;
	  std::string   name;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

typedef G4THitsCollection<ScintHit> ScintHitsCollection;
extern G4Allocator<ScintHit> ScintHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* ScintHit::operator new(size_t)
{
  void *aHit;
  aHit = (void *) ScintHitAllocator.MallocSingle();
  return aHit;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void ScintHit::operator delete(void *aHit)
{
  ScintHitAllocator.FreeSingle((ScintHit*) aHit);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
