#include "DetectorConstruction_SiPM.hh"
#include "CathodeSD.hh"
#include "ScintillatorSD.hh"
#include "PrimaryGeneratorAction.hh"

//#include </Users/macalex/geant4/geant4.10.02.p02-install/include/Geant4/CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/SystemOfUnits.h>

#include <G4Material.hh>
#include <G4Box.hh>
#include <G4Tubs.hh>
#include <G4Cons.hh>
#include <G4Trd.hh>
#include <G4Ellipsoid.hh>
#include <G4SubtractionSolid.hh>
#include <G4UnionSolid.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4PVParameterised.hh>
#include <G4SDManager.hh>
#include <G4GeometryTolerance.hh>
#include <G4GeometryManager.hh>
#include <G4NistManager.hh>
#include <G4RunManager.hh>
#include <G4PhysicalVolumeStore.hh>
#include <G4LogicalVolumeStore.hh>
#include <G4SolidStore.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4LogicalBorderSurface.hh>
#include <G4SurfaceProperty.hh>
#include <G4UImanager.hh>
#include <G4OpticalSurface.hh>
#include <G4UserLimits.hh>
#include <G4VisAttributes.hh>
#include <G4Colour.hh>
#include <G4ios.hh>
#include <G4GeneralParticleSource.hh>
#include <G4RegionStore.hh>
#include "G4ProductionCuts.hh"
#include "G4GenericMessenger.hh"

#include <string>
#include <sstream>
#include <limits>

#include "OptService.h"

using namespace CLHEP;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//extern double testAngle;

DetectorConstruction::DetectorConstruction()
:solidWorld(0),  logicWorld(0),  physiWorld(0),
 solidCathodeRight(0),logicCathodeRight(0),physiCathodeRight(0),
 solidCathodeLeft(0),logicCathodeLeft(0),physiCathodeLeft(0),
 solidCathodeCentral(0),logicCathodeCentral(0),physiCathodeCentral(0),
 fWorldLength(0.), fSourceDistance(0.), theCathodeSDRight(0),
 theCathodeSDCentral(0),
 fArmAngle(90.*deg), fArmRotation(0), physiWrapper(0),
 fMessenger(0)
{
       fArmRotation = new G4RotationMatrix();
       fArmRotation->rotateY(fArmAngle);
       // define commands for this class
       DefineCommands();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction()
{
       delete fArmRotation;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
	setDefaultDimensions();
	defineMaterials();
	defineSurfaces();

	return constructDetector();
}


void DetectorConstruction::setDefaultDimensions()
{
  //fSourceDistance              = 2.0*cm;
  fSourceDistance              = 200.0*cm;

  //fSteelCaseR1                 = 30.0 *mm;
  //fSteelCaseR2                 = 37.0 *mm;
  //fSteelCaseLength             = 65.0 *mm;

  //fAlCupLength                 =  1.0 *mm;
  //fTopThickness                =  1.0*um;

  /*
  fScintDimX                   = 11.5*mm;
  fScintDimY                   = 11.5*mm;
  fScintDimZ                   = 4.5*mm;
  */
  fScintDimX                   = 4.5*mm;              //LYSO bar 3x3x50 mm
  fScintDimY                   = 4.5*mm;
  fScintDimZ                   = 50.0*mm;


  //fEpoxyDimX                   = 5.0*mm;
  fEpoxyDimX                   = 3.1*mm;
  //fEpoxyDimY                   = 5.0*mm;
  fEpoxyDimY                   = 3.1*mm;
  fEpoxyThickness              = 0.3*mm;

  fCathodeDimX                 = 3. * mm;
  fCathodeDimY                 = 3. * mm;
  fCathodeThickness            = 1.0*um,

  //fGreaseRadius                = 0.5*sqrt(fScintDimX*fScintDimX + fScintDimY*fScintDimY);
  fGreaseRadius                = 0.5*sqrt(fEpoxyDimX*fEpoxyDimX + fEpoxyDimY*fEpoxyDimY);
  fGreaseThickness             = 0.01*mm; //default 10um

  updateWorldLength();
}



void DetectorConstruction::defineMaterials()
{
	G4double a, z, density;
	G4int nelements;
	G4double atomicNumber;
  	G4double massOfMole;
  	G4double temperature;
  	G4double pressure;

  //--------- Material definition ---------
  G4NistManager* man = G4NistManager::Instance();

  G4Element *C = man->FindOrBuildElement("C");
  G4Element *H = man->FindOrBuildElement("H");
  G4Element *Si = man->FindOrBuildElement("Si");
  G4Element *O = man->FindOrBuildElement("O");
  G4Element *Sb = man->FindOrBuildElement("Sb");
  G4Element *Rb = man->FindOrBuildElement("Rb");
  G4Element *Cs = man->FindOrBuildElement("Cs");
  G4Element *Lu = man->FindOrBuildElement("Lu");
  G4Element *Y = man->FindOrBuildElement("Y");
  G4Element *Ce = man->FindOrBuildElement("Ce");
  G4Element *La = man->FindOrBuildElement("La");
  G4Element *Br = man->FindOrBuildElement("Br");
  G4Element *Na = man->FindOrBuildElement("Na");
  G4Element *I = man->FindOrBuildElement("I");
  G4Element *Tl = man->FindOrBuildElement("Tl");

  //------------------------------
  // Aluminum
  G4Material* Aluminum = man->FindOrBuildMaterial("G4_Al");
  Aluminum->SetName("Aluminum");

  //------------------------------
  // Steel
  G4Material* Steel = man->FindOrBuildMaterial("G4_STAINLESS-STEEL");
  Steel->SetName("Steel");

  //------------------------------
  // Air
  G4Material* Air = man->FindOrBuildMaterial("G4_AIR");
  Air->SetName("Air");
  const G4int numentries = 2;
  G4double energies[numentries] = { 0.1*eV, 10.0*eV };
  G4double vacrindices[numentries] = { 1., 1. };
  G4double airabsorpti[numentries] = { 100.*m, 100.*m }; // avoid infinite light-paths
  G4MaterialPropertiesTable* airprop = new G4MaterialPropertiesTable();
  airprop->AddProperty("ABSLENGTH", energies, airabsorpti, numentries);
  airprop->AddProperty("RINDEX", energies, vacrindices, numentries);
  Air->SetMaterialPropertiesTable(airprop);


  //Absolute vacuum does not exist. It is a gas at very low density !
  //Vacuum - interGalactic
  G4Material* Vacuum=new G4Material("Vacuum",atomicNumber=1.,massOfMole=1.008*g/mole,density=1.e-25*g/cm3,kStateGas,temperature=2.73*kelvin,pressure=3.e-18*pascal);


  //------------------------------
  // Epoxy NOA61
  // -------------
  G4Material* Epoxy = new G4Material("Epoxy", density = 1.3*g / cm3, nelements = 3);
  Epoxy->AddElement(C, 15);
  Epoxy->AddElement(H, 44);
  Epoxy->AddElement(O, 7);

  std::vector<G4double> epoxywavelength = {1240., 1000., 700., 600., 550., 500., 450., 400., 350., 300., 250.};

  std::vector<G4double> epoxyenergies;
  std::vector<G4double> epoxyindices;
  std::vector<G4double> epoxybsorptionlength;
  for (auto it = epoxywavelength.begin(); it != epoxywavelength.end(); it++)
  {
	  G4double energy = OptService::nm2eV(*it) * CLHEP::eV;
	  epoxyenergies.push_back(energy);
	  G4double r = 1.5375 + 8290.45 / pow(*it, 2.) - 2.11046E+8 / pow(*it, 4.);
	  epoxyindices.push_back(r);
	  epoxybsorptionlength.push_back(42.*cm);
  }

  G4MaterialPropertiesTable *EpoxyProperties = new G4MaterialPropertiesTable();
  EpoxyProperties->AddProperty("RINDEX", epoxyenergies.data(), epoxyindices.data(), epoxywavelength.size());
  EpoxyProperties->AddProperty("ABSLENGTH", epoxyenergies.data(), epoxybsorptionlength.data(), epoxywavelength.size());
  Epoxy->SetMaterialPropertiesTable(EpoxyProperties);

  //-----------------------------
  // CsI refraction index:  http://www.nist.gov/data/PDFfiles/jpcrd81.pdf
  // timing and light yeild: http://www.hep.caltech.edu/~zhu/papers/93_nim_csi.pdf

  G4Material* CsI = new G4Material("CsI", density = 4.509*g / cm3, nelements = 2);
  CsI->AddElement(Cs, 1);
  CsI->AddElement(I,  1);

  std::vector<G4double> csienergies;
  std::vector<G4double> csirindices;
  std::vector<G4double> csiabsorptionlength;
  std::vector<G4double> csicomp;
  std::vector<G4double> csicompenergies;

  //red emission spectrum data
  OptService::ReadParameters("OPTICS/csi_spectrum.txt", csicompenergies, csicomp, true, CLHEP::eV);
  //read refraction indexes data
  OptService::ReadParameters("OPTICS/csi_rindex.txt", csienergies, csirindices, true, CLHEP::eV);

  //fill absoption length data
  for (auto it = csienergies.begin(); it != csienergies.end(); it++)
  {
	  csiabsorptionlength.push_back(42.*cm);
  }

  G4MaterialPropertiesTable *CsIProperties = new G4MaterialPropertiesTable();
  CsIProperties->AddProperty("RINDEX", csienergies.data(), csirindices.data(), csienergies.size());
  CsIProperties->AddProperty("ABSLENGTH", csienergies.data(), csiabsorptionlength.data(), csienergies.size());

  CsIProperties->AddConstProperty("SCINTILLATIONYIELD", 2000. / MeV); // saint-gobain
  CsIProperties->AddConstProperty("RESOLUTIONSCALE", 1.0);

  CsIProperties->AddProperty("FASTCOMPONENT", csicompenergies.data(), csicomp.data(), csicompenergies.size());
  CsIProperties->AddProperty("SLOWCOMPONENT", csicompenergies.data(), csicomp.data(), csicompenergies.size());

  CsIProperties->AddConstProperty("FASTTIMECONSTANT", 6.5*ns); 
  CsIProperties->AddConstProperty("SLOWTIMECONSTANT", 30.*ns);
  // The relative strength of the fast component as a fraction of total scintillation yield is given by the YIELDRATIO
  CsIProperties->AddConstProperty("YIELDRATIO", 0.3);
  CsI->SetMaterialPropertiesTable(CsIProperties);



  //------------------------------
  // common LYSO
  // http://www.hep.caltech.edu/~zhu/papers/07_TNS_LYSO.pdf
  // According to the manufacturers, the yttrium content is about 5% for the CPI LYSO [8] and about 10% for the Saint-Gobain LYSO[9].
  //
  //----------------------------------------------------------------------------------------------
  // LYSO Absorbtion lenght is taken form: http://iopscience.iop.org/0031-9155/55/6/009/media
  //
  // Optical simulation of monolithic scintillator detectors using GATE/GEANT4
  // D J(Jan) van der Laan1, Dennis R Schaart1, Marnix C Maas1, Freek J Beekman1, 2, Peter Bruyndonckx3 and Carel W E van Eijk1
  // Published 24 February 2010 \95 2010 Institute of Physics and Engineering in Medicine \95 Physics in Medicine and Biology, Volume 55, Number 6


  G4Material* LYSO = new G4Material("LYSO", 7.1*g / cm3, 5, kStateSolid);  //LYSO - 7.1 gr/cm3
  LYSO->AddElement(Lu, 71.43*perCent);
  LYSO->AddElement(Y, 4.03*perCent);
  LYSO->AddElement(Si, 6.37*perCent);
  LYSO->AddElement(O, 18.14*perCent);
  LYSO->AddElement(Ce, 0.02*perCent); // cooke2000

  G4MaterialPropertiesTable* lysoprop = new G4MaterialPropertiesTable();
  G4PhysicsOrderedFreeVector *v;

  std::vector<G4double> lysoenergies;
  std::vector<G4double> lysofastcomp; //relative intencity
  OptService::ReadParameters("OPTICS/LYSO_Spectrum.csv", lysoenergies, lysofastcomp, true, CLHEP::eV);
  v = lysoprop->AddProperty("FASTCOMPONENT", lysoenergies.data(), lysofastcomp.data(), lysoenergies.size());
 // v->SetSpline(true);

  // refractive index from: 
  lysoenergies.clear();
  std::vector<G4double> lysorindices;
  OptService::ReadParameters("OPTICS/LYSO_rindex.txt", lysoenergies, lysorindices, true, CLHEP::eV);
  lysoprop->AddProperty("RINDEX", lysoenergies.data(), lysorindices.data(), lysoenergies.size());

  lysoenergies.clear();
  std::vector<double> dAL;
  OptService::ReadParameters("OPTICS/lyso_abslength.txt", lysoenergies, dAL, true, CLHEP::eV, mm);
  v = lysoprop->AddProperty("ABSLENGTH", lysoenergies.data(), dAL.data(), lysoenergies.size());
  //v->SetSpline(true);
  //v->DumpValues();


  lysoprop->AddConstProperty("SCINTILLATIONYIELD", 25000. / MeV); // saint-gobain
  lysoprop->AddConstProperty("RESOLUTIONSCALE", 4.0);
  lysoprop->AddConstProperty("FASTTIMECONSTANT", 36.0*ns); // saint-gobain
  //lysoprop->AddConstProperty("FASTTIMECONSTANT", 6.0*ns); // saint-gobain
  lysoprop->AddConstProperty("YIELDRATIO", 1.0);
  LYSO->SetMaterialPropertiesTable(lysoprop);

  G4cout << "LYSO G4MaterialPropertiesTable" << G4endl;
  lysoprop->DumpTable();

  // Set the Birks Constant for the scintillator
  LYSO->GetIonisation()->SetBirksConstant(0.0705 * mm / MeV);
  //LYSO->GetIonisation()->SetBirksConstant(0.126 * mm/MeV);
  G4cout << "\n\n  GetMeanExcitationEnergy (keV):  " << LYSO->GetIonisation()->GetMeanExcitationEnergy() / keV << G4endl;
  G4cout << "\n\n  GetMeanEnergyPerIonPair (keV):  " << LYSO->GetIonisation()->GetMeanEnergyPerIonPair() / keV << G4endl;
  //------------------------------


  //------------------------------------------------
  // Polydimethylsiloxane (Grease)
  G4Material* Polydimethylsiloxane = new G4Material("Polydimethylsiloxane", 0.97*g / cm3, 4, kStateLiquid);
  Polydimethylsiloxane->AddElement(Si, 1);
  Polydimethylsiloxane->AddElement(O, 1);
  Polydimethylsiloxane->AddElement(C, 2);
  Polydimethylsiloxane->AddElement(H, 6);

  G4MaterialPropertiesTable* polydimethylsiloxaneprop = new G4MaterialPropertiesTable();

  std::vector<G4double> Genergy;
  std::vector<G4double> Gabsorp;
  std::vector<G4double> Grindex;

  OptService::ReadParameters("OPTICS/Meltmount1582_AbsLength_cm.txt", Genergy, Gabsorp, true, CLHEP::eV, CLHEP::cm);
  polydimethylsiloxaneprop->AddProperty("ABSLENGTH", Genergy.data(), Gabsorp.data(), Genergy.size());
  Genergy.clear();
  OptService::ReadParameters("OPTICS/Meltmount1582_Refraction.txt", Genergy, Grindex, true, CLHEP::eV);
  polydimethylsiloxaneprop->AddProperty("RINDEX", Genergy.data(), Grindex.data(), Genergy.size());
  Polydimethylsiloxane->SetMaterialPropertiesTable(polydimethylsiloxaneprop);
  //------------------------------------------------

  //------------------------------
  // Fused silica
  G4Material* FusedSilica = new G4Material("FusedSilica", 2.201*g / cm3, 2, kStateSolid);
  FusedSilica->AddElement(Si, 1);
  FusedSilica->AddElement(O, 2);

  G4MaterialPropertiesTable* fusedsilicaprop = new G4MaterialPropertiesTable();

  std::vector<G4double> fusedsilicaenergy;
  std::vector<G4double> fusedsilicarindex;
  std::vector<G4double> fusedsilicaabsorp;

  //https://www.filmetrics.com/refractive-index-database/SiO2/Fused-Silica-Silicon-Dioxide-Thermal-Oxide-ThermalOxide
  OptService::ReadParameters("OPTICS/SiO2-refraction.txt", fusedsilicaenergy, fusedsilicarindex, true, CLHEP::eV);
  fusedsilicaprop->AddProperty("RINDEX", fusedsilicaenergy.data(), fusedsilicarindex.data(), fusedsilicaenergy.size());

  for (auto it = fusedsilicaenergy.begin(); it != fusedsilicaenergy.end(); it++)
  {
	  //set absobption length
	  fusedsilicaabsorp.push_back(1.*m);
  }
  fusedsilicaprop->AddProperty("ABSLENGTH", fusedsilicaenergy.data(), fusedsilicaabsorp.data(), fusedsilicaenergy.size());
  FusedSilica->SetMaterialPropertiesTable(fusedsilicaprop);


  //------------------------------


  //------------------------------
  // SiPM
  G4Material* Silicon = new G4Material("Silicon", 2.57*g / cm3, 1, kStateSolid);
  Silicon->AddElement(Si, 1);
  //------------------------------
}


void DetectorConstruction::defineSurfaces()
{
  G4double ener[2] = {.1*eV, 10.*eV};

  polishedAir = new G4OpticalSurface("polishedAir", unified);
  polishedAir->SetType(dielectric_dielectric);
  polishedAir->SetModel(unified);
  //polishedAir->SetFinish(ground); // necessary even for polished surfaces to enable UNIFIED code
  polishedAir->SetFinish(polished); // necessary even for polished surfaces to enable UNIFIED code
  //polishedAir->SetSigmaAlpha(1.3 * degree); // Janecek2010
  polishedAir->SetSigmaAlpha(0); // Janecek2010

  // common paint attributes
  G4double paintspecularlobe[2] = {1.0, 1.0};
  G4double paintspecularspike[2] = {0.0, 0.0};
  G4double paintbackscatter[2] = {0.0, 0.0};
  G4double paintrindex[2] = {1.61, 1.61}; // Janecek2010

  // white painted
  G4double whitepaintrefl[2] = { 0.955, 0.955 }; // Janecek2010
  G4double teflonrefl[2] = { 0.99,  0.99 };  // Janecek2010


  polishedWhitePainted = new G4OpticalSurface("polishedWhitePainted", unified);
  polishedWhitePainted->SetType(dielectric_dielectric);
  polishedWhitePainted->SetModel(unified);
  polishedWhitePainted->SetFinish(groundbackpainted);
  polishedWhitePainted->SetSigmaAlpha(0.1 * degree); // 1.3 Janecek2010
  G4MaterialPropertiesTable* polishedWhitePaintedProperty = new G4MaterialPropertiesTable();
  polishedWhitePaintedProperty->AddProperty("RINDEX", ener, paintrindex, 2);
  polishedWhitePaintedProperty->AddProperty("SPECULARLOBECONSTANT", ener, paintspecularlobe, 2);
  polishedWhitePaintedProperty->AddProperty("SPECULARSPIKECONSTANT", ener, paintspecularspike, 2);
  polishedWhitePaintedProperty->AddProperty("BACKSCATTERCONSTANT", ener, paintbackscatter, 2);
  polishedWhitePaintedProperty->AddProperty("REFLECTIVITY", ener, teflonrefl, 2);
  polishedWhitePainted->SetMaterialPropertiesTable(polishedWhitePaintedProperty);

  groundWhitePainted = new G4OpticalSurface("groundWhitePainted", unified);
  groundWhitePainted->SetType(dielectric_dielectric);
  groundWhitePainted->SetModel(unified);
  groundWhitePainted->SetFinish(groundbackpainted);
  groundWhitePainted->SetSigmaAlpha(12.0 * degree); // Janecek2010
  G4MaterialPropertiesTable* groundWhitePaintedProperty = new G4MaterialPropertiesTable();
  groundWhitePaintedProperty->AddProperty("RINDEX", ener, paintrindex, 2);
  groundWhitePaintedProperty->AddProperty("SPECULARLOBECONSTANT", ener, paintspecularlobe, 2);
  groundWhitePaintedProperty->AddProperty("SPECULARSPIKECONSTANT", ener, paintspecularspike, 2);
  groundWhitePaintedProperty->AddProperty("BACKSCATTERCONSTANT", ener, paintbackscatter, 2);
  groundWhitePaintedProperty->AddProperty("REFLECTIVITY", ener, whitepaintrefl, 2);
  groundWhitePainted->SetMaterialPropertiesTable(groundWhitePaintedProperty);

  // black painted
  G4double blackpaintrefl[2] = { 0.043, 0.043 }; // Dury2006

  polishedBlackPainted = new G4OpticalSurface("polishedBlackPainted", unified);
  polishedBlackPainted->SetType(dielectric_dielectric);
  polishedBlackPainted->SetModel(unified);
  polishedBlackPainted->SetFinish(groundbackpainted);
  polishedBlackPainted->SetSigmaAlpha(1.3 * degree); // Janecek2010
  G4MaterialPropertiesTable* polishedBlackPaintedProperty = new G4MaterialPropertiesTable();
  polishedBlackPaintedProperty->AddProperty("RINDEX", ener, paintrindex, 2);
  polishedBlackPaintedProperty->AddProperty("SPECULARLOBECONSTANT", ener, paintspecularlobe, 2);
  polishedBlackPaintedProperty->AddProperty("SPECULARSPIKECONSTANT", ener, paintspecularspike, 2);
  polishedBlackPaintedProperty->AddProperty("BACKSCATTERCONSTANT", ener, paintbackscatter, 2);
  polishedBlackPaintedProperty->AddProperty("REFLECTIVITY", ener, blackpaintrefl, 2);
  polishedBlackPainted->SetMaterialPropertiesTable(polishedBlackPaintedProperty);

  groundBlackPainted = new G4OpticalSurface("groundBlackPainted", unified);
  groundBlackPainted->SetType(dielectric_dielectric);
  groundBlackPainted->SetModel(unified);
  groundBlackPainted->SetFinish(groundbackpainted);
  groundBlackPainted->SetSigmaAlpha(12.0 * degree); // Janecek2010
  G4MaterialPropertiesTable* groundBlackPaintedProperty = new G4MaterialPropertiesTable();
  groundBlackPaintedProperty->AddProperty("RINDEX", ener, paintrindex, 2);
  groundBlackPaintedProperty->AddProperty("SPECULARLOBECONSTANT", ener, paintspecularlobe, 2);
  groundBlackPaintedProperty->AddProperty("SPECULARSPIKECONSTANT", ener, paintspecularspike, 2);
  groundBlackPaintedProperty->AddProperty("BACKSCATTERCONSTANT", ener, paintbackscatter, 2);
  groundBlackPaintedProperty->AddProperty("REFLECTIVITY", ener, blackpaintrefl, 2);
  groundBlackPainted->SetMaterialPropertiesTable(groundBlackPaintedProperty);


  //----------------------------------------------------------------------------------------------
  CathodeMaterial = new G4OpticalSurface("CathodeMaterial", unified);
  CathodeMaterial->SetType(dielectric_metal);
  CathodeMaterial->SetModel(unified);
  CathodeMaterial->SetFinish(ground);
  CathodeMaterial->SetSigmaAlpha(1.3*degree);
  G4MaterialPropertiesTable *CathodeMaterialProperty = new G4MaterialPropertiesTable();

  std::vector<G4double> silicawavelength = { 1000., 900., 800., 700., 600., 550., 500., 450., 400., 350., 200. };
  std::vector<G4double> silicaenergy;
  std::vector<G4double> silicarindex = { 0.316, 0.322, 0.328, 0.338, 0.354, 0.367, 0.387, 0.419, 0.486, 0.565, 0.3 };
  for (auto it = silicawavelength.begin(); it != silicawavelength.end(); it++)
  {
	  G4double energy = OptService::nm2eV(*it) * CLHEP::eV;
	  silicaenergy.push_back(energy);
  }
  CathodeMaterialProperty->AddProperty("REFLECTIVITY", silicaenergy.data(), silicarindex.data(), silicawavelength.size());

  std::vector<double> dEngEff;
  std::vector<double> dEff;
  //OptService::ReadParameters("OPTICS/S13360-QE.csv", dEngEff, dEff, true, CLHEP::eV);
  OptService::ReadParameters("OPTICS/mppc.dat", dEngEff, dEff, true, CLHEP::eV);
  CathodeMaterialProperty->AddProperty("EFFICIENCY", dEngEff.data(), dEff.data(), dEngEff.size());
  CathodeMaterial->SetMaterialPropertiesTable(CathodeMaterialProperty);

  //----------------------------------------------------------------------------------------------
  polishedReflectorMaterial = new G4OpticalSurface("polishedReflectorMaterial", unified);
  polishedReflectorMaterial->SetType(dielectric_metal);
  polishedReflectorMaterial->SetModel(unified);
  polishedReflectorMaterial->SetFinish(polished);
  polishedReflectorMaterial->SetSigmaAlpha(0.1 * degree);
  G4MaterialPropertiesTable *polishedReflectorProperty = new G4MaterialPropertiesTable();

  std::vector<G4double> AL_energy;
  std::vector<G4double> AL_data;
  OptService::ReadParameters("OPTICS/al_reflectivity.txt", AL_energy, AL_data, true, CLHEP::eV);
  polishedReflectorProperty->AddProperty("REFLECTIVITY", AL_energy.data(), AL_data.data(), AL_energy.size());
  polishedReflectorMaterial->SetMaterialPropertiesTable(polishedReflectorProperty);

  //----------------------------------------------------------------------------------------------
  groundReflectorMaterial = new G4OpticalSurface("groundReflectorMaterial", unified);
  groundReflectorMaterial->SetType(dielectric_metal);
  groundReflectorMaterial->SetModel(unified);
  groundReflectorMaterial->SetFinish(polished);
  groundReflectorMaterial->SetSigmaAlpha(10.0 * degree);
  G4MaterialPropertiesTable *groundReflectorProperty = new G4MaterialPropertiesTable();
  groundReflectorProperty->AddProperty("REFLECTIVITY", AL_energy.data(), AL_data.data(), AL_energy.size());
  groundReflectorMaterial->SetMaterialPropertiesTable(groundReflectorProperty);

}




G4VPhysicalVolume * DetectorConstruction::constructDetector()
{
  G4UImanager* UI = G4UImanager::GetUIpointer();

  //--------- Determine positions ---------
  G4ThreeVector positionWrapper   = G4ThreeVector(0, 0, 0);
  G4ThreeVector positionScint   = G4ThreeVector(0, 0, 0);
  //Right SiPM
  G4ThreeVector positionGreaseRight  = G4ThreeVector(0, 0, fScintDimZ / 2.0 + fGreaseThickness / 2.0);
  G4ThreeVector positionEpoxyRight   = G4ThreeVector(0, 0, fScintDimZ / 2.0 + fGreaseThickness + fEpoxyThickness / 2.0);
  G4ThreeVector positionCathodeRight = G4ThreeVector(0, 0, fScintDimZ / 2.0 + fGreaseThickness + fEpoxyThickness + fCathodeThickness / 2.0);
  //Left SiPM
  G4ThreeVector positionGreaseLeft  = G4ThreeVector(0, 0, -(fScintDimZ / 2.0 + fGreaseThickness / 2.0));
  G4ThreeVector positionEpoxyLeft   = G4ThreeVector(0, 0, -(fScintDimZ / 2.0 + fGreaseThickness + fEpoxyThickness / 2.0));
  G4ThreeVector positionCathodeLeft = G4ThreeVector(0, 0, -(fScintDimZ / 2.0 + fGreaseThickness + fEpoxyThickness + fCathodeThickness / 2.0));

  //Central SiPM
  G4ThreeVector positionGreaseCentral  = G4ThreeVector(0, - fScintDimY / 2.0 - fGreaseThickness / 2.0, 0);
  G4ThreeVector positionEpoxyCentral  = G4ThreeVector(0, - fScintDimY / 2.0 - fGreaseThickness - fEpoxyThickness / 2.0, 0);
  G4ThreeVector positionCathodeCentral = G4ThreeVector(0, - fScintDimY / 2.0 - fGreaseThickness - fEpoxyThickness - fCathodeThickness / 2.0, 0);


  //--------- Definitions of Solids, Logical Volumes, Physical Volumes ---------
  //------------------------------
  // World
  //------------------------------
  G4double HalfWorldLength = 0.25*fWorldLength;
  solidWorld= new G4Box("sworld", HalfWorldLength, HalfWorldLength, HalfWorldLength);
  logicWorld= new G4LogicalVolume( solidWorld, G4Material::GetMaterial("Air"), "lWorld", 0, 0, 0);
  //logicWorld= new G4LogicalVolume( solidWorld, G4Material::GetMaterial("Vacuum"), "lWorld", 0, 0, 0);

  //  Must place the World Physical volume unrotated at (0,0,0).
  //

  physiWorld = new G4PVPlacement(0,               // no rotation
                                 G4ThreeVector(), // at (0,0,0)
                                 logicWorld,      // its logical volume
                                 "pWorld",         // its name
                                 0,               // its mother  volume
                                 false,           // no boolean operations
                                 0);              // copy number


  //------------------------------
  // Air wrapper for rotation
  //------------------------------
  G4Tubs* solidWrapper = new G4Tubs("Wrapper", 0.*cm, (0.5*sqrt(fScintDimX*fScintDimX + fScintDimY*fScintDimY)+0.1*mm), (fScintDimZ/2.+fGreaseThickness+fEpoxyThickness+fCathodeThickness),0., 360.*deg);
  G4LogicalVolume* logicWrapper = new G4LogicalVolume(solidWrapper, G4Material::GetMaterial("Air"), "lWrapper",0,0,0);  //CsI
  //G4VPhysicalVolume* physiWrapper = new G4PVPlacement(rm,                // rotation matrix
  physiWrapper = new G4PVPlacement(fArmRotation,                // rotation matrix
                                  positionWrapper,   // at (x,y,z)
                                  logicWrapper,      // its logical volume
                                  "pWrapper", // its name
                                  logicWorld,      // its mother  volume
                                  false,           // no boolean operations
                                  0);              // copy number


  //------------------------------
  // Scintillator
  //------------------------------
  solidScint = new G4Box("Scintillator", fScintDimX/2, fScintDimY/2, fScintDimZ/2);
  logicScint = new G4LogicalVolume(solidScint, G4Material::GetMaterial("LYSO"), "lScintillator",0,0,0);  //CsI
  physiScint = new G4PVPlacement(0,                // no rotation
                                  positionScint,   // at (x,y,z)
                                  logicScint,      // its logical volume
                                  "pScintillator", // its name
                                  logicWrapper,      // its mother  volume
                                  false,           // no boolean operations
                                  0);              // copy number

  //------------------------------------------------
  // Grease R
  //------------------------------------------------
  solidGreaseRight = new G4Tubs("sgreaseRight", 0.*cm, fGreaseRadius, fGreaseThickness / 2.0, 0.*deg, 360.*deg);
  logicGreaseRight = new G4LogicalVolume(solidGreaseRight, G4Material::GetMaterial("Polydimethylsiloxane"), "lGreaseRight", 0,0,0);
  physiGreaseRight = new G4PVPlacement(0, positionGreaseRight, logicGreaseRight, "pGreaseRight", logicWrapper, false, 0);              // copy number

  //------------------------------------------------
  // Grease L
  //------------------------------------------------
  solidGreaseLeft = new G4Tubs("sgreaseLeft", 0.*cm, fGreaseRadius, fGreaseThickness / 2.0, 0.*deg, 360.*deg);
  logicGreaseLeft = new G4LogicalVolume(solidGreaseLeft, G4Material::GetMaterial("Polydimethylsiloxane"), "lGreaseLeft", 0,0,0);
  physiGreaseLeft = new G4PVPlacement(0, positionGreaseLeft, logicGreaseLeft, "pGreaseLeft", logicWrapper, false, 0);              // copy number

  //------------------------------------------------
  // Grease Central
  //------------------------------------------------
  solidGreaseCentral = new G4Tubs("sgreaseCentral", 0.*cm, fGreaseRadius, fGreaseThickness / 2.0, 0.*deg, 360.*deg);
  logicGreaseCentral = new G4LogicalVolume(solidGreaseCentral, G4Material::GetMaterial("Polydimethylsiloxane"), "lGreaseCentral", 0,0,0);
  G4RotationMatrix *rm = new G4RotationMatrix();
  rm->rotateX(90*deg);
  physiGreaseCentral = new G4PVPlacement(rm, positionGreaseCentral, logicGreaseCentral, "pGreaseCentral", logicWrapper, false, 0);


  //------------------------------------------------
  // Case R
  //------------------------------------------------
  solidEpoxyRight = new G4Box("EpoxyRight", fEpoxyDimX / 2., fEpoxyDimY / 2., fEpoxyThickness / 2.);
  logicEpoxyRight = new G4LogicalVolume(solidEpoxyRight, G4Material::GetMaterial("Epoxy"), "lEpoxyRight", 0, 0, 0);
  physiEpoxyRight = new G4PVPlacement(0,                // no rotation
	  positionEpoxyRight,   // at (x,y,z)
	  logicEpoxyRight,      // its logical volume
	  "pEpoxyRight",        // its name
	  logicWrapper,      // its mother  volume
	  false,           // no boolean operations
	  0);              // copy number

  //------------------------------------------------
  // Case R
  //------------------------------------------------
  solidEpoxyLeft = new G4Box("EpoxyLeft", fEpoxyDimX / 2., fEpoxyDimY / 2., fEpoxyThickness / 2.);
  logicEpoxyLeft = new G4LogicalVolume(solidEpoxyLeft, G4Material::GetMaterial("Epoxy"), "lEpoxyLeft", 0, 0, 0);
  physiEpoxyLeft = new G4PVPlacement(0,                // no rotation
	  positionEpoxyLeft,   // at (x,y,z)
	  logicEpoxyLeft,      // its logical volume
	  "pEpoxyLeft",        // its name
	  logicWrapper,      // its mother  volume
	  false,           // no boolean operations
	  0);              // copy number


  //------------------------------------------------
  // Case Central
  //------------------------------------------------
  solidEpoxyCentral = new G4Box("EpoxyCentral", fEpoxyDimX / 2., fEpoxyDimY / 2., fEpoxyThickness / 2.);
  logicEpoxyCentral = new G4LogicalVolume(solidEpoxyCentral, G4Material::GetMaterial("Epoxy"), "lEpoxyCentral", 0, 0, 0);
  physiEpoxyCentral = new G4PVPlacement(rm,                // no rotation
	  positionEpoxyCentral,   // at (x,y,z)
	  logicEpoxyCentral,      // its logical volume
	  "pEpoxyCentral",        // its name
	  logicWrapper,      // its mother  volume
	  false,           // no boolean operations
	  0);              // copy number


  //------------------------------
  // PMT Cathode R
  //------------------------------
  solidCathodeRight = new G4Box("sCathRight", fCathodeDimX / 2., fCathodeDimY / 2., fCathodeThickness / 2.);
  logicCathodeRight = new G4LogicalVolume(solidCathodeRight, G4Material::GetMaterial("Silicon"), "lCathRight", 0,0,0);
  physiCathodeRight = new G4PVPlacement(0,               // no rotation
                                  positionCathodeRight,  // at (x,y,z)
                                  logicCathodeRight,     // its logical volume
                                  "pCathRight",          // its name
                                  logicWrapper,       // its mother  volume
                                  false,            // no boolean operations
                                  0);               // copy number

  //------------------------------
  // PMT Cathode L
  //------------------------------
  solidCathodeLeft = new G4Box("sCathLeft", fCathodeDimX / 2., fCathodeDimY / 2., fCathodeThickness / 2.);
  logicCathodeLeft = new G4LogicalVolume(solidCathodeLeft, G4Material::GetMaterial("Silicon"), "lCathLeft", 0,0,0);
  physiCathodeLeft = new G4PVPlacement(0,               // no rotation
                                  positionCathodeLeft,  // at (x,y,z)
                                  logicCathodeLeft,     // its logical volume
                                  "pCathLeft",          // its name
                                  logicWrapper,       // its mother  volume
                                  false,            // no boolean operations
                                  0);               // copy number

  //------------------------------
  // PMT Cathode Central
  //------------------------------
  solidCathodeCentral = new G4Box("sCathCentral", fCathodeDimX / 2., fCathodeDimY / 2., fCathodeThickness / 2.);
  logicCathodeCentral = new G4LogicalVolume(solidCathodeCentral, G4Material::GetMaterial("Silicon"), "lCathCentral", 0,0,0);
  physiCathodeCentral = new G4PVPlacement(rm,               // no rotation
                                  positionCathodeCentral,  // at (x,y,z)
                                  logicCathodeCentral,     // its logical volume
                                  "pCathCentral",          // its name
                                  logicWrapper,       // its mother  volume
                                  false,            // no boolean operations
                                  0);               // copy number


  //------------------------------
  //The rest of Si R
  //------------------------------
  solidSiBulkRight = new G4Box("sSiBulkRight", fEpoxyDimX / 2., fEpoxyDimY / 2., fCathodeThickness / 2.);
  G4SubtractionSolid* subSiBulkRight = new G4SubtractionSolid("subSiBulkRight", solidSiBulkRight, solidCathodeRight);
  logicSiBulkRight = new G4LogicalVolume(subSiBulkRight, G4Material::GetMaterial("Silicon"), "lSiBulkRight", 0, 0, 0);
  physiSiBulkRight = new G4PVPlacement(0,               // no rotation
	  positionCathodeRight,  // at (x,y,z)
	  logicSiBulkRight,      // its logical volume
	  "pSiBulkRight",        // its name
	  logicWrapper,       // its mother  volume
	  false,            // no boolean operations
	  0);               // copy number

  //------------------------------
  //The rest of Si L
  //------------------------------
  solidSiBulkLeft = new G4Box("sSiBulkLeft", fEpoxyDimX / 2., fEpoxyDimY / 2., fCathodeThickness / 2.);
  G4SubtractionSolid* subSiBulkLeft = new G4SubtractionSolid("subSiBulkLeft", solidSiBulkLeft, solidCathodeLeft);
  logicSiBulkLeft = new G4LogicalVolume(subSiBulkLeft, G4Material::GetMaterial("Silicon"), "lSiBulkLeft", 0, 0, 0);
  physiSiBulkLeft = new G4PVPlacement(0,               // no rotation
	  positionCathodeLeft,  // at (x,y,z)
	  logicSiBulkLeft,      // its logical volume
	  "pSiBulkLeft",        // its name
	  logicWrapper,       // its mother  volume
	  false,            // no boolean operations
	  0);               // copy number

  //------------------------------
  //The rest of Si Central
  //------------------------------
  solidSiBulkCentral = new G4Box("sSiBulkCentral", fEpoxyDimX / 2., fEpoxyDimY / 2., fCathodeThickness / 2.);
  G4SubtractionSolid* subSiBulkCentral = new G4SubtractionSolid("subSiBulkCentral", solidSiBulkCentral, solidCathodeCentral);
  logicSiBulkCentral = new G4LogicalVolume(subSiBulkCentral, G4Material::GetMaterial("Silicon"), "lSiBulkCentral", 0, 0, 0);
  physiSiBulkCentral = new G4PVPlacement(rm,               // no rotation
	  positionCathodeCentral,  // at (x,y,z)
	  logicSiBulkCentral,      // its logical volume
	  "pSiBulkCentral",        // its name
	  logicWrapper,       // its mother  volume
	  false,            // no boolean operations
	  0);               // copy number


//------------------------------------------------
// Define regions
//------------------------------------------------
  G4Region* aRegion = new G4Region("Scintillator");
  aRegion->AddRootLogicalVolume(logicScint);

  // Define cuts object for the new region and set values
  G4ProductionCuts* cuts = new G4ProductionCuts;
  cuts->SetProductionCut(0.1*mm); // for gamma, e+, e-
  // Assign cuts to region
  aRegion->SetProductionCuts(cuts);


//------------------------------------------------
// Define sensitive detector
//------------------------------------------------

  G4String sensitiveDetectorName2 = "/detector/sensitiveDetector2";
  theScintillatorSD = new ScintillatorSD(sensitiveDetectorName2);
  logicScint->SetSensitiveDetector(theScintillatorSD);


  G4String sensitiveDetectorNameR = "sensitiveDetectorR";
  theCathodeSDRight = new CathodeSD(sensitiveDetectorNameR);
  logicCathodeRight -> SetSensitiveDetector(theCathodeSDRight);

  G4String sensitiveDetectorNameL = "sensitiveDetectorL";
  theCathodeSDLeft = new CathodeSD(sensitiveDetectorNameL);
  logicCathodeLeft -> SetSensitiveDetector(theCathodeSDLeft);

  //G4String sensitiveDetectorName3 = "/detector/sensitiveDetector3";
  //theCathodeSDCentral = new CathodeSD(sensitiveDetectorName3);
  //logicCathodeCentral->SetSensitiveDetector(theCathodeSDCentral);



  //-------------------------------------------------
  // setup surfaces
  //-------------------------------------------------
  new G4LogicalBorderSurface("scint2AirFaceBorderSurface", physiScint, physiWrapper, polishedAir);
  //new G4LogicalBorderSurface("scint2AirFaceBorderSurface", physiScint, physiWorld, polishedAir);
  //new G4LogicalBorderSurface("scint2AirFaceBorderSurface", physiScint, physiWorld, polishedBlackPainted);
  //new G4LogicalBorderSurface("scint2AirFaceBorderSurface", physiScint, physiWorld, polishedWhitePainted);
  //new G4LogicalBorderSurface("scint2AirFaceBorderSurface", physiScint, physiWorld, groundWhitePainted);

  new G4LogicalBorderSurface("grease2AirFaceBorderSurfaceRight", physiGreaseRight, physiWorld, polishedAir);
  new G4LogicalBorderSurface("grease2AirFaceBorderSurfaceLeft", physiGreaseLeft, physiWorld, polishedAir);
  new G4LogicalBorderSurface("grease2AirFaceBorderSurfaceCentral", physiGreaseCentral, physiWorld, polishedAir);

  new G4LogicalBorderSurface("epoxy2AirFaceBorderSurfaceRight", physiEpoxyRight, physiWorld, polishedBlackPainted);
  new G4LogicalBorderSurface("epoxy2AirFaceBorderSurfaceLeft", physiEpoxyLeft, physiWorld, polishedBlackPainted);
  new G4LogicalBorderSurface("epoxy2AirFaceBorderSurfaceCentral", physiEpoxyCentral, physiWorld, polishedBlackPainted);

  new G4LogicalBorderSurface("Epoxy2CathodeBorderSurfaceRight", physiEpoxyRight, physiCathodeRight, CathodeMaterial);
  new G4LogicalBorderSurface("Epoxy2CathodeBorderSurfaceLeft", physiEpoxyLeft, physiCathodeLeft, CathodeMaterial);
  new G4LogicalBorderSurface("Epoxy2CathodeBorderSurfaceCentral", physiEpoxyCentral, physiCathodeCentral, CathodeMaterial);

  new G4LogicalBorderSurface("Epoxy2SiBulkBorderSurfaceRight", physiEpoxyRight, physiSiBulkRight, CathodeMaterial);
  new G4LogicalBorderSurface("Epoxy2SiBulkBorderSurfaceLeft", physiEpoxyLeft, physiSiBulkLeft, CathodeMaterial);
  new G4LogicalBorderSurface("Epoxy2SiBulkBorderSurfaceCentral", physiEpoxyCentral, physiSiBulkCentral, CathodeMaterial);

  //--------- Visualization attributes -------------------------------
  G4VisAttributes* ScintVisAtt = new G4VisAttributes(G4Colour(1.0,0.0,1.0, 0.6));
  G4VisAttributes* GreaseVisAtt = new G4VisAttributes(G4Colour(1.0, 1.0, 0.0, 0.8));
  G4VisAttributes* CathodeVisAtt = new G4VisAttributes(G4Colour(0.0, 1.0, 1.0, 0.5));

  CathodeVisAtt->SetForceWireframe(true);

  logicWorld->SetVisAttributes(G4VisAttributes::Invisible);
  logicScint->SetVisAttributes(ScintVisAtt);
  logicCathodeRight->SetVisAttributes(CathodeVisAtt);
  logicGreaseRight->SetVisAttributes(GreaseVisAtt);
  logicCathodeCentral->SetVisAttributes(CathodeVisAtt);
  logicGreaseCentral->SetVisAttributes(GreaseVisAtt);


  //G4OpticalSurface* opticalSurface = dynamic_cast <G4OpticalSurface*>(muS->GetSurface(physiCone, physiWorld)->GetSurfaceProperty());
  //if (opticalSurface) opticalSurface->DumpInfo();

  // Below is an example of how to set tracking constraints in a given logical volume
   //G4double maxStep   = 0.5*fScintDimZ;
   //G4double maxLength = 2 * HalfWorldLength;
   //G4double maxTime   = 1000.*ns;
   //G4double minEkin   = 250 * eV;
   //logicScint->SetUserLimits(new G4UserLimits(maxStep,  maxLength,  maxTime,  minEkin));


  return physiWorld;
}


void DetectorConstruction::updateWorldLength()
{
	G4double detectorlength = fScintDimX + fScintDimY + fScintDimZ + fGreaseThickness + fEpoxyDimX + fEpoxyDimY + fEpoxyThickness;

	fWorldLength = std::max(detectorlength, fSourceDistance)*2.0;
	fWorldLength *= 4.0;
}


void DetectorConstruction::SetArmAngle(G4double val)
{
     if (!physiWrapper)
    {
        G4cerr << "Detector has not yet been constructed." << G4endl;
        return;
    }
        fArmAngle = val;
        *fArmRotation = G4RotationMatrix();  // make it unit vector
        fArmRotation->rotateX(fArmAngle);
        //fArmRotation->rotateY(fArmAngle);
        //fArmRotation->rotateZ(fArmAngle);
        //auto x = -5.*m * std::sin(fArmAngle);
        //auto z = 5.*m * std::cos(fArmAngle);
        //fSecondArmPhys->SetTranslation(G4ThreeVector(x,0.,z));
        //physiWrapper->SetTranslation(G4ThreeVector(0.,0.,0.));
        // tell G4RunManager that we change the geometry
        G4RunManager::GetRunManager()->GeometryHasBeenModified();
        G4cout << " fArmAngle -> "  << fArmAngle << " deg "<< G4endl;
        G4cout << " GetArmAngle() -> "  << GetArmAngle() << " deg "<< G4endl;
        //G4cout << " &DetectorConstruction::SetArmAngle -> "  << &DetectorConstruction::testAngle << " deg "<< G4endl;
}

void DetectorConstruction::DefineCommands()
{       // Define /B5/detector command directory using generic messenger class
        fMessenger = new G4GenericMessenger(this,"/MTD/detector/","Detector control");
        // armAngle command
        auto& armAngleCmd     = fMessenger->DeclareMethodWithUnit("armAngle","deg", &DetectorConstruction::SetArmAngle,"Set rotation angle of the second arm.");
        armAngleCmd.SetParameterName("angle", true);
        //armAngleCmd.SetRange("angle>=0. && angle<360.");
        armAngleCmd.SetRange("angle>=-90. && angle<90.");
        armAngleCmd.SetDefaultValue("30.");
}
