#ifndef DetectorConstruction_h
#define DetectorConstruction_h

#include <globals.hh>
#include <G4ThreeVector.hh>
#include <G4VUserDetectorConstruction.hh>
#include "CathodeSD.hh"
#include "ScintillatorSD.hh"
//#include </Users/macalex/geant4/geant4.10.02.p02-install/include/Geant4/CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/SystemOfUnits.h>
#include "G4RotationMatrix.hh"

class G4Box;
class G4Tubs;
class G4Cons;
class G4Trd;
class G4Ellipsoid;
class G4SubtractionSolid;
class G4UnionSolid;
class G4BooleanSolid;
class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;
class G4VPVParameterisation;
class G4UserLimits;
class Messenger;
class G4OpticalSurface;
class G4GenericMessenger;

//#include <CLHEP\Units\SystemOfUnits.h>
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:

     DetectorConstruction();
    ~DetectorConstruction();

  public:

     G4VPhysicalVolume* Construct();
     void setDefaultDimensions();
	 G4VSensitiveDetector* GetSensetiveDetector() { return theCathodeSDRight; }
	 G4VSensitiveDetector* GetSensetiveDetector2() { return theCathodeSDLeft; }
	 G4VPhysicalVolume*    GetScintillator() { return physiScint; }

         void SetArmAngle(G4double val);
         G4double GetArmAngle() { return fArmAngle; }
         G4double fArmAngle;
         int testAngle=fArmAngle*180/3.14;

  private:
     void DefineCommands();

     G4GenericMessenger* fMessenger;
     G4RotationMatrix* fArmRotation;
     G4VPhysicalVolume* physiWrapper;


     G4Box*             solidWorld;    // pointer to the solid envelope
     G4LogicalVolume*   logicWorld;    // pointer to the logical envelope
     G4VPhysicalVolume* physiWorld;    // pointer to the physical envelope

     G4Box*             solidScint;   // pointer to the conic solid Scintillator
     G4LogicalVolume*   logicScint;   // pointer to the logical Scintillator
     G4VPhysicalVolume* physiScint;   // pointer to the physical Scintillator

     G4Box*             solidEpoxyRight;
     G4LogicalVolume*   logicEpoxyRight;
     G4VPhysicalVolume* physiEpoxyRight;

     G4Box*             solidEpoxyLeft;
     G4LogicalVolume*   logicEpoxyLeft;
     G4VPhysicalVolume* physiEpoxyLeft;

     G4Box*             solidEpoxyCentral;
     G4LogicalVolume*   logicEpoxyCentral;
     G4VPhysicalVolume* physiEpoxyCentral;

     G4Tubs*            solidGreaseRight;
     G4LogicalVolume*   logicGreaseRight;
     G4VPhysicalVolume* physiGreaseRight;

     G4Tubs*            solidGreaseLeft;
     G4LogicalVolume*   logicGreaseLeft;
     G4VPhysicalVolume* physiGreaseLeft;

     G4Tubs*            solidGreaseCentral;
     G4LogicalVolume*   logicGreaseCentral;
     G4VPhysicalVolume* physiGreaseCentral;

     G4Box*             solidCathodeRight;  // pointer to the solid Cathode
     G4LogicalVolume*   logicCathodeRight;  // pointer to the logical Cathode
     G4VPhysicalVolume* physiCathodeRight;  // pointer to the physical Cathode

     G4Box*             solidCathodeLeft;  // pointer to the solid Cathode
     G4LogicalVolume*   logicCathodeLeft;  // pointer to the logical Cathode
     G4VPhysicalVolume* physiCathodeLeft;  // pointer to the physical Cathode

     G4Box*             solidCathodeCentral;  // pointer to the solid Cathode
     G4LogicalVolume*   logicCathodeCentral;  // pointer to the logical Cathode
     G4VPhysicalVolume* physiCathodeCentral;  // pointer to the physical Cathode

     G4Box*             solidSiBulkRight;  //
     G4LogicalVolume*   logicSiBulkRight;  //
     G4VPhysicalVolume* physiSiBulkRight;  //

     G4Box*             solidSiBulkLeft;  //
     G4LogicalVolume*   logicSiBulkLeft;  //
     G4VPhysicalVolume* physiSiBulkLeft;  //

     G4Box*             solidSiBulkCentral;  //
     G4LogicalVolume*   logicSiBulkCentral;  //
     G4VPhysicalVolume* physiSiBulkCentral;  //

     // surfaces
     G4OpticalSurface *polishedAir;              // polished crystal surface, not wrapped
     G4OpticalSurface *polishedWhitePainted;     // polished crystal surface painted white
     G4OpticalSurface *groundWhitePainted;       // ground crystal surface painted white
     G4OpticalSurface *polishedBlackPainted;     // polished crystal surface painted black
     G4OpticalSurface *groundBlackPainted;       // ground crystal surface painted black

     G4OpticalSurface *CathodeMaterial;          // surface between window and cathode
     G4OpticalSurface *polishedReflectorMaterial;// top of the scintillator
     G4OpticalSurface *groundReflectorMaterial;

     // arbitraryly valued variables (not changeable by user)
     G4double fWorldLength;            // Full length of the world volume

     // user-changeable variables
     G4double fSourceDistance;

	 G4double fSteelCaseR1;
	 G4double fSteelCaseR2;
	 G4double fSteelCaseLength;

	 G4double fAlCupLength;

	 G4double fScintDimX;
	 G4double fScintDimY;
	 G4double fScintDimZ;

	 G4double fGreaseRadius;
	 G4double fGreaseThickness;

	 G4double fEpoxyDimX;
	 G4double fEpoxyDimY;
	 G4double fEpoxyThickness;

	 G4double fCathodeDimX;
	 G4double fCathodeDimY;
	 G4double fCathodeThickness;

	 G4double fTopThickness;

	 //Sensetive detector data
     CathodeSD*      theCathodeSDRight;
     CathodeSD*      theCathodeSDLeft;
     CathodeSD*      theCathodeSDCentral;
     ScintillatorSD* theScintillatorSD;

     void defineMaterials();
     void defineSurfaces();
     G4VPhysicalVolume* constructDetector();

     void updateWorldLength();
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
