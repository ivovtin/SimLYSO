#include "LightCone/DetectorConstruction.hh"
#include "CathodeSD.hh"
#include "ScintillatorSD.hh"
#include "PrimaryGeneratorAction.hh"

#include <CLHEP/Units/SystemOfUnits.h>

#include <G4Material.hh>
#include <G4Box.hh>
#include <G4Tubs.hh>
#include <G4Cons.hh>
#include <G4Trd.hh>
#include <G4Orb.hh>
#include <G4Hype.hh>
#include <G4Sphere.hh>
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

#include <string>
#include <sstream>
#include <limits>

#include "OptService.h"

using namespace CLHEP;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
DetectorConstruction::DetectorConstruction()
:solidWorld(0),  logicWorld(0),  physiWorld(0),
 solidCathode(0),logicCathode(0), physiCathode(0),
 fWorldLength(0.), fSourceDistance(0.), 
 theScintillatorSD(0),	theCathodeSD(0), fRegion(0)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
DetectorConstruction::~DetectorConstruction()
{
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
  fSourceDistance              = 20.0*cm;

  fTopThickness                = 0.01 *mm;

  fScintDimX                   = 1.50*mm;
  fScintDimY                   = 1.50*mm;
  fScintDimZ                   = 3.50*mm;

  fLensRadius                  = 0.75*mm;
  fConeProtectionSize          = 2.0 *mm;

  fConeRadiusTop               = 0.80*mm;
  fConeRadiusBot               = 0.50*mm;
  fConeLength                  = 1.5*mm;


  fGreaseRadius                = 0.5*sqrt(fScintDimX*fScintDimX + fScintDimY*fScintDimY);
  fGreaseThickness             = 0.05*mm; //default 100um

  fEpoxyDimX                   = 2.0*mm;
  fEpoxyDimY                   = 2.0*mm;
  fEpoxyThickness              = 0.3*mm;

  fCathodeDimX                 = 1.2 * mm;
  fCathodeDimY                 = 1.2 * mm;
  fCathodeThickness            = 1.0*um,
  
  updateWorldLength();
}

void DetectorConstruction::defineMaterials()
{
	G4double a, z, density;
	G4int nelements;

	//--------- Material definition ---------
	G4NistManager* man = G4NistManager::Instance();

	G4Element *C = man->FindOrBuildElement("C");
	G4Element *H = man->FindOrBuildElement("H");
	G4Element *Si = man->FindOrBuildElement("Si");
	G4Element *Al = man->FindOrBuildElement("Al");
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

	//------------------------------
	// Epoxy NOA61
	// -------------
	G4Material* Epoxy = new G4Material("Epoxy", density = 1.3*g / cm3, nelements = 3);
	Epoxy->AddElement(C, 15);
	Epoxy->AddElement(H, 44);
	Epoxy->AddElement(O, 7);

	std::vector<G4double> epoxywavelength = { 1240., 1000., 700., 600., 550., 500., 450., 400., 350., 300., 250. };

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
	CsI->AddElement(I, 1);

	std::vector<G4double> csienergies;
	std::vector<G4double> csirindices;
	std::vector<G4double> csiabsorptionlength;
	std::vector<G4double> csicomp;
	std::vector<G4double> csicompenergies;

	//red emission spectrum data
	OptService::ReadParameters("OPTICS\\csi_spectrum.txt", csicompenergies, csicomp, true, CLHEP::eV);
	//read refraction indexes data
	OptService::ReadParameters("OPTICS\\csi_rindex.txt", csienergies, csirindices, true, CLHEP::eV);

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
	// Published 24 February 2010 • 2010 Institute of Physics and Engineering in Medicine • Physics in Medicine and Biology, Volume 55, Number 6


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
	OptService::ReadParameters("OPTICS\\LYSO_Spectrum.csv", lysoenergies, lysofastcomp, true, CLHEP::eV);
	v = lysoprop->AddProperty("FASTCOMPONENT", lysoenergies.data(), lysofastcomp.data(), lysoenergies.size());
	// v->SetSpline(true);

	// refractive index from: 
	lysoenergies.clear();
	std::vector<G4double> lysorindices;
	OptService::ReadParameters("OPTICS\\LYSO_rindex.txt", lysoenergies, lysorindices, true, CLHEP::eV);
	lysoprop->AddProperty("RINDEX", lysoenergies.data(), lysorindices.data(), lysoenergies.size());

	lysoenergies.clear();
	std::vector<double> dAL;
	OptService::ReadParameters("OPTICS\\lyso_abslength.txt", lysoenergies, dAL, true, CLHEP::eV, mm);
	v = lysoprop->AddProperty("ABSLENGTH", lysoenergies.data(), dAL.data(), lysoenergies.size());

	//v->SetSpline(true);
	//v->DumpValues();


	lysoprop->AddConstProperty("SCINTILLATIONYIELD", 25000. / MeV); // saint-gobain
	lysoprop->AddConstProperty("RESOLUTIONSCALE", 4.0);
	lysoprop->AddConstProperty("FASTTIMECONSTANT", 36.0*ns); // saint-gobain
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

	OptService::ReadParameters("OPTICS\\Meltmount1582_AbsLength_cm.txt", Genergy, Gabsorp, true, CLHEP::eV, CLHEP::cm);
	polydimethylsiloxaneprop->AddProperty("ABSLENGTH", Genergy.data(), Gabsorp.data(), Genergy.size());
	Genergy.clear();
	OptService::ReadParameters("OPTICS\\Meltmount1582_Refraction.txt", Genergy, Grindex, true, CLHEP::eV);
	polydimethylsiloxaneprop->AddProperty("RINDEX", Genergy.data(), Grindex.data(), Genergy.size());
	Polydimethylsiloxane->SetMaterialPropertiesTable(polydimethylsiloxaneprop);
	//------------------------------------------------

	//------------------------------
	// Fused silica
	G4Material* FusedSilica = new G4Material("FusedSilica", 2.201*g / cm3, 2, kStateSolid);
	FusedSilica->AddElement(Si, 1);
	FusedSilica->AddElement(O, 2);

	G4MaterialPropertiesTable* fusedsilicaprop = new G4MaterialPropertiesTable();

	std::vector<G4double> fusedsilicawavelength = { 656., 546., 436., 302., 280., 214., 200., 185., 170. };
	std::vector<G4double> fusedsilicaenergy;
	std::vector<G4double> fusedsilicarindex;
	std::vector<G4double> fusedsilicaabsorp;
	for (auto it = fusedsilicawavelength.begin(); it != fusedsilicawavelength.end(); it++)
	{
		//get energy
		G4double energy = OptService::nm2eV(*it) * CLHEP::eV;
		fusedsilicaenergy.push_back(energy);

		//for REFRACTION we use the data from http://refractiveindex.info/?shelf=glass&book=fused_silica&page=Malitson
		std::vector<OptService::RATIONAL> vParams;

		std::vector<double> a1 = { 1. }; std::vector<double> b1 = { 1. };
		OptService::RATIONAL R1(a1, b1);
		vParams.push_back(R1);

		std::vector<double> a2 = { 0., 0., 0.6961663 }; std::vector<double> b2 = { -pow(0.0684043, 2.), 0., 1. };
		OptService::RATIONAL R2(a2, b2);
		vParams.push_back(R2);

		std::vector<double> a3 = { 0., 0., 0.4079426 }; std::vector<double> b3 = { -pow(0.1162414, 2.), 0., 1. };
		OptService::RATIONAL R3(a3, b3);
		vParams.push_back(R3);

		std::vector<double> a4 = { 0., 0., 0.8974794 }; std::vector<double> b4 = { -pow(9.896161, 2.), 0., 1. };
		OptService::RATIONAL R4(a4, b4);
		vParams.push_back(R4);

		G4double rindex = GetRefractiveIndex(*it, 2, vParams);
		fusedsilicarindex.push_back(rindex);

		//set absobption length
		fusedsilicaabsorp.push_back(2.*m);
	}

	fusedsilicaprop->AddProperty("ABSLENGTH", fusedsilicaenergy.data(), fusedsilicaabsorp.data(), fusedsilicawavelength.size());
	fusedsilicaprop->AddProperty("RINDEX", fusedsilicaenergy.data(), fusedsilicarindex.data(), fusedsilicawavelength.size());
	FusedSilica->SetMaterialPropertiesTable(fusedsilicaprop);
	//------------------------------

	//------------------------------
	// Sapphire
	G4Material* Sapphire = new G4Material("Sapphire", 3.98*g / cm3, 2, kStateSolid);
	Sapphire->AddElement(Al, 2);
	Sapphire->AddElement(O, 3);

	G4MaterialPropertiesTable* Sapphireprop = new G4MaterialPropertiesTable();

	std::vector<G4double> Sapphirewavelength = { 656., 546., 436., 302., 280., 214., 200., 185., 170. };
	std::vector<G4double> Sapphireenergy;
	std::vector<G4double> Sapphirerindex;
	std::vector<G4double> Sapphireabsorp;
	for (auto it = Sapphirewavelength.begin(); it != Sapphirewavelength.end(); it++)
	{
		//get energy
		G4double energy = OptService::nm2eV(*it) * CLHEP::eV;
		Sapphireenergy.push_back(energy);

		//for REFRACTION we use the data from http://refractiveindex.info/?shelf=main&book=Al2O3&page=Malitson-o
		std::vector<OptService::RATIONAL> vParams;

		std::vector<double> a1 = { 1. }; std::vector<double> b1 = { 1. };
		OptService::RATIONAL R1(a1, b1);
		vParams.push_back(R1);

		std::vector<double> a2 = { 0., 0., 1.4313493 }; std::vector<double> b2 = { -pow(0.0726631, 2.), 0., 1. };
		OptService::RATIONAL R2(a2, b2);
		vParams.push_back(R2);

		std::vector<double> a3 = { 0., 0., 0.65054713 }; std::vector<double> b3 = { -pow(0.1193242, 2.), 0., 1. };
		OptService::RATIONAL R3(a3, b3);
		vParams.push_back(R3);

		std::vector<double> a4 = { 0., 0., 5.3414021 }; std::vector<double> b4 = { -pow(18.028251, 2.), 0., 1. };
		OptService::RATIONAL R4(a4, b4);
		vParams.push_back(R4);

		G4double rindex = GetRefractiveIndex(*it, 2, vParams);
		Sapphirerindex.push_back(rindex);

		//set absobption length
		Sapphireabsorp.push_back(2.*m);
	}

	Sapphireprop->AddProperty("ABSLENGTH", Sapphireenergy.data(), Sapphireabsorp.data(), Sapphirewavelength.size());
	Sapphireprop->AddProperty("RINDEX", Sapphireenergy.data(), Sapphirerindex.data(), Sapphirewavelength.size());
	Sapphire->SetMaterialPropertiesTable(Sapphireprop);
	//------------------------------



	//------------------------------
	// SiPM
	G4Material* Silicon = new G4Material("Silicon", 2.57*g / cm3, 1, kStateSolid);
	Silicon->AddElement(Si, 1);
	//------------------------------
}


void DetectorConstruction::defineSurfaces()
{
	G4double ener[2] = { .1*eV, 10.*eV };

	polishedAir = new G4OpticalSurface("polishedAir", unified);
	polishedAir->SetType(dielectric_dielectric);
	polishedAir->SetModel(unified);
	polishedAir->SetFinish(ground); // necessary even for polished surfaces to enable UNIFIED code
	polishedAir->SetSigmaAlpha(1.3 * degree); // Janecek2010

											  // common paint attributes
	G4double paintspecularlobe[2] = { 1.0, 1.0 };
	G4double paintspecularspike[2] = { 0.0, 0.0 };
	G4double paintbackscatter[2] = { 0.0, 0.0 };
	G4double paintrindex[2] = { 1.61, 1.61 }; // Janecek2010

											       // white painted
	G4double whitepaintrefl[2] = { 0.955, 0.955 }; // Janecek2010
	G4double teflonrefl[2] = { 0.99,  0.99 };      // Janecek2010
	G4double alumrefl[2] = { 0.85,  0.85 };        // 


	polishedWhitePainted = new G4OpticalSurface("polishedWhitePainted", unified);
	polishedWhitePainted->SetType(dielectric_dielectric);
	polishedWhitePainted->SetModel(unified);
	polishedWhitePainted->SetFinish(groundbackpainted);
	polishedWhitePainted->SetSigmaAlpha(1.3 * degree); // Janecek2010
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
	CathodeMaterial->SetFinish(polished);
	CathodeMaterial->SetSigmaAlpha(0.);
	G4MaterialPropertiesTable *CathodeMaterialProperty = new G4MaterialPropertiesTable();

	std::vector<G4double> silicawavelength = { 1000., 900., 800., 700., 600., 550., 500., 450., 400., 350. };
	std::vector<G4double> silicaenergy;
	std::vector<G4double> silicarindex = { 0.316, 0.322, 0.328, 0.338, 0.354, 0.367, 0.387, 0.419, 0.486, 0.565 };
	for (auto it = silicawavelength.begin(); it != silicawavelength.end(); it++)
	{
		G4double energy = OptService::nm2eV(*it) * CLHEP::eV;
		silicaenergy.push_back(energy);
	}
	CathodeMaterialProperty->AddProperty("REFLECTIVITY", silicaenergy.data(), silicarindex.data(), silicawavelength.size());

	std::vector<double> dEngEff;
	std::vector<double> dEff;
	OptService::ReadParameters("OPTICS\\S13360-QE.csv", dEngEff, dEff, true, CLHEP::eV);
	CathodeMaterialProperty->AddProperty("EFFICIENCY", dEngEff.data(), dEff.data(), dEngEff.size());
	CathodeMaterial->SetMaterialPropertiesTable(CathodeMaterialProperty);


	//----------------------------------------------------------------------------------------------
	polishedReflector = new G4OpticalSurface("ReflectorMaterial", unified);
	polishedReflector->SetType(dielectric_metal);
	polishedReflector->SetModel(unified);
	polishedReflector->SetFinish(polished);
	polishedReflector->SetSigmaAlpha(1.0 * degree);
	G4MaterialPropertiesTable *polishedReflectorProperty = new G4MaterialPropertiesTable();
	polishedReflectorProperty->AddProperty("SPECULARLOBECONSTANT", ener, paintspecularlobe, 2);
	polishedReflectorProperty->AddProperty("SPECULARSPIKECONSTANT", ener, paintspecularspike, 2);
	polishedReflectorProperty->AddProperty("BACKSCATTERCONSTANT", ener, paintbackscatter, 2);
	polishedReflectorProperty->AddProperty("REFLECTIVITY", ener, alumrefl, 2);
	polishedReflector->SetMaterialPropertiesTable(polishedReflectorProperty);


}

G4VPhysicalVolume * DetectorConstruction::constructDetector()
{
  G4UImanager* UI = G4UImanager::GetUIpointer();

  //--------- Determine positions ---------
  G4ThreeVector positionTop     = G4ThreeVector(0, 0, -fScintDimZ / 2.0 - fTopThickness / 2.0);
  G4ThreeVector positionScint   = G4ThreeVector(0, 0, 0);
  G4ThreeVector positionCone    = G4ThreeVector(0, 0, fScintDimZ / 2.0 + fConeLength / 2.0);
  G4ThreeVector positionLens = G4ThreeVector(0, 0, fScintDimZ / 2.0);// +fLensRadius);
  G4ThreeVector positionGrease  = G4ThreeVector(0, 0, fScintDimZ / 2.0 + fConeLength + fGreaseThickness / 2.0);
  G4ThreeVector positionEpoxy   = G4ThreeVector(0, 0, fScintDimZ / 2.0 + fConeLength + fGreaseThickness + fEpoxyThickness / 2.0);
  G4ThreeVector positionCathode = G4ThreeVector(0, 0, fScintDimZ / 2.0 + fConeLength + fGreaseThickness + +fEpoxyThickness + fCathodeThickness / 2.0);

  //--------- Definitions of Solids, Logical Volumes, Physical Volumes ---------
  //------------------------------
  // World
  //------------------------------
  G4double HalfWorldLength = 0.5*fWorldLength;
  solidWorld= new G4Box("sworld", HalfWorldLength, HalfWorldLength, HalfWorldLength);
  logicWorld= new G4LogicalVolume( solidWorld, G4Material::GetMaterial("Air"), "lWorld", 0, 0, 0);

  //  Must place the World Physical volume unrotated at (0,0,0).
  //
  physiWorld = new G4PVPlacement(0,               // no rotation
                                 G4ThreeVector(), // at (0,0,0)
                                 logicWorld,      // its logical volume
                                 "pWorld",         // its name
                                 0,               // its mother  volume
                                 false,           // no boolean operations
                                 0);              // copy number

  // Set user cuts to avoid deadlocks
  G4double maxStep = 10.0*m, maxLength = 10.0*m, maxTime = 100.0*ns, minEkin = 0.5*eV;
  logicWorld->SetUserLimits(new G4UserLimits(maxStep,maxLength,maxTime,minEkin));

  //------------------------------
  // Scintillator
  //------------------------------
  solidScint = new G4Box("Scintillator", fScintDimX/2, fScintDimY/2, fScintDimZ/2);
  logicScint = new G4LogicalVolume(solidScint, G4Material::GetMaterial("LYSO"), "lScintillator",0,0,0);
  physiScint = new G4PVPlacement(0,                // no rotation
                                  positionScint,   // at (x,y,z)
                                  logicScint,      // its logical volume
                                  "pScintillator", // its name
                                  logicWorld,      // its mother  volume
                                  false,           // no boolean operations
                                  0);              // copy number

  //------------------------------------------------
  // Protective top and side
  //------------------------------------------------
  solidTop = new G4Box("Top", fScintDimX/2, fScintDimY/2, fTopThickness/2);
  logicTop = new G4LogicalVolume(solidTop, G4Material::GetMaterial("Epoxy"), "lTop", 0, 0, 0);
  physiTop = new G4PVPlacement(0,                // no rotation
	  positionTop,   // at (x,y,z)
	  logicTop,      // its logical volume
	  "pTop",        // its name
	  logicWorld,      // its mother  volume
	  false,           // no boolean operations
	  0);              // copy number

  solidSide = new G4Box("Side", fScintDimX/2+fTopThickness, fScintDimY/2+fTopThickness, fScintDimZ/2);
  G4SubtractionSolid* subSideProtection = new G4SubtractionSolid("subSideProtection", solidSide, solidScint);
  logicSide = new G4LogicalVolume(subSideProtection, G4Material::GetMaterial("Epoxy"), "lSide", 0, 0, 0);
  physiSide = new G4PVPlacement(0,                // no rotation
	  positionScint,   // at (x,y,z)
	  logicSide,       // its logical volume
	  "pSide",         // its name
	  logicWorld,      // its mother  volume
	  false,           // no boolean operations
	  0);              // copy number


  //------------------------------------------------
  // Grease
  //------------------------------------------------
  solidGrease = new G4Tubs("sgrease", 0.*cm, fGreaseRadius, fGreaseThickness / 2.0, 0.*deg, 360.*deg);
  logicGrease = new G4LogicalVolume(solidGrease, G4Material::GetMaterial("Polydimethylsiloxane"), "lGrease", 0,0,0);
  physiGrease = new G4PVPlacement(0, 
	  positionGrease, 
	  logicGrease, 
	  "pGrease", 
	  logicWorld, 
	  false, 
	  0);              // copy number

  //------------------------------------------------
  // Case
  //------------------------------------------------
  solidEpoxy = new G4Box("Epoxy", fEpoxyDimX / 2., fEpoxyDimY / 2., fEpoxyThickness / 2.);
  logicEpoxy = new G4LogicalVolume(solidEpoxy, G4Material::GetMaterial("Epoxy"), "lEpoxy", 0, 0, 0);
  physiEpoxy = new G4PVPlacement(0,                // no rotation
	  positionEpoxy,   // at (x,y,z)
	  logicEpoxy,      // its logical volume
	  "pEpoxy",        // its name
	  logicWorld,      // its mother  volume
	  false,           // no boolean operations
	  0);              // copy number

  
  //------------------------------
  // PMT Cathode
  //------------------------------
  solidCathode = new G4Box("sCath", fCathodeDimX / 2., fCathodeDimY / 2., fCathodeThickness / 2.);
  logicCathode = new G4LogicalVolume(solidCathode, G4Material::GetMaterial("Silicon"), "lCath", 0,0,0);
  physiCathode = new G4PVPlacement(0,               // no rotation
                                  positionCathode,  // at (x,y,z)
                                  logicCathode,     // its logical volume 
                                  "pCath",          // its name
                                  logicWorld,       // its mother  volume
                                  false,            // no boolean operations
                                  0);               // copy number
  //------------------------------
  //The rest of Si
  //------------------------------
  solidSiBulk = new G4Box("sSiBulk", fEpoxyDimX / 2., fEpoxyDimY / 2., fCathodeThickness / 2.);
  G4SubtractionSolid* subSiBulk = new G4SubtractionSolid("subSiBulk", solidSiBulk, solidCathode);
  logicSiBulk = new G4LogicalVolume(subSiBulk, G4Material::GetMaterial("Silicon"), "lSiBulk", 0, 0, 0);
  physiSiBulk = new G4PVPlacement(0,               // no rotation
	  positionCathode,  // at (x,y,z)
	  logicSiBulk,      // its logical volume 
	  "pSiBulk",        // its name
	  logicWorld,       // its mother  volume
	  false,            // no boolean operations
	  0);               // copy number


  ////------------------------------
  //// Light cone
  ////------------------------------
  ////  solidCone = new G4Trd("sCone", fConeRadiusTop, fConeRadiusBot, fConeRadiusTop, fConeRadiusBot, fConeLength / 2.0);
  solidCone = new G4Hype("sCone", 0, fConeRadiusTop, 0, 1.0*rad, fConeLength / 2.);
//  G4Box* solidConeCropper = new G4Box("solidConeCropper", fConeRadiusTop, fConeRadiusTop, fConeLength/2.);
//  G4SubtractionSolid* subSolidCone = new G4SubtractionSolid("subSolidCone", solidCone, solidConeCropper, NULL, G4ThreeVector(0.,0., fConeLength/2.));

  //logicCone = new G4LogicalVolume(solidCone, G4Material::GetMaterial("Epoxy"), "lCone", 0, 0, 0);
  //physiCone = new G4PVPlacement(0,
	 // positionCone, 
	 // logicCone,
	 // "pCone",
	 // logicWorld,
	 // false,
	 // 0);

  //------------------------------
  // Outer Light protection cone 
  //------------------------------
  solidConeProtection = new G4Cons("sConeProtection", 0, fConeProtectionSize, 0, fConeProtectionSize, fConeLength / 2.0, 0.*deg, 360.*deg); //fLensRadius / 2
  G4SubtractionSolid* subConeProtection = new G4SubtractionSolid("subConeProtection", solidConeProtection, solidCone);
  logicConeProtection = new G4LogicalVolume(subConeProtection, G4Material::GetMaterial("Aluminum"), "lConeProtection", 0, 0, 0);
  //physiConeProtection = new G4PVPlacement(0,
	 // positionCone,
	 // logicConeProtection,
	 // "pConeProtection",
	 // logicWorld,
	 // false,
	 // 0);

  ////------------------------------
  //// Inner sphere lens
  ////------------------------------
  //solidLens = new G4Sphere("sLens",0 ,fLensRadius,0, 2*pi, 0, pi/2); // G4Orb("sCone", fLensRadius);
  //logicLens = new G4LogicalVolume(solidLens, G4Material::GetMaterial("FusedSilica"), "lLens", 0, 0, 0); //Sapphire //FusedSilica
  //physiLens = new G4PVPlacement(0, 
  // positionLens, 
  // logicLens, 
  // "pLens", 
  // logicWorld, 
  // false, 
  // 0);


  //------------------------------------------------
  // Define regions
  //------------------------------------------------
  G4Region* aRegion = new G4Region("Scintillator");
  aRegion->AddRootLogicalVolume(logicScint);

  //------------------------------------------------
  // Define sensitive detector
  //------------------------------------------------
  G4String sensitiveDetectorName = "/detector/sensitiveDetector";
  theCathodeSD = new CathodeSD(sensitiveDetectorName);
  SetSensitiveDetector("lCath", theCathodeSD);
  //logicCathode->SetSensitiveDetector(theCathodeSD);

  G4String sensitiveDetectorName2 = "/detector/sensitiveDetector2";
  theScintillatorSD = new ScintillatorSD(sensitiveDetectorName2);
  SetSensitiveDetector("lScintillator", theScintillatorSD);
  //  logicScint->SetSensitiveDetector(theScintillatorSD);




  //-------------------------------------------------
  // setup surfaces
  //-------------------------------------------------
  //new G4LogicalBorderSurface("scint2AirFaceBorderSurface", physiScint, physiWorld, polishedWhitePainted);
  new G4LogicalBorderSurface("scint2TopFaceBorderSurface", physiScint, physiTop, polishedWhitePainted);
  new G4LogicalBorderSurface("scint2SideFaceBorderSurface", physiScint, physiSide , polishedWhitePainted);
  new G4LogicalBorderSurface("grease2AirFaceBorderSurface", physiGrease, physiWorld, polishedBlackPainted);

  //new G4LogicalBorderSurface("Air2AlBorderSurface",         physiWorld, physiConeProtection, polishedReflector);


  //new G4LogicalBorderSurface("scint2ConeProtectionBorderSurface", physiScint, physiConeProtection, polishedWhitePainted);
  //new G4LogicalBorderSurface("cone2ConeProtectionBorderSurface", physiCone, physiConeProtection, polishedReflector);

  new G4LogicalBorderSurface("Epoxy2CathodeBorderSurface", physiEpoxy, physiCathode, CathodeMaterial);
  new G4LogicalBorderSurface("Epoxy2SiBulkBorderSurface", physiEpoxy, physiSiBulk, CathodeMaterial);
  new G4LogicalBorderSurface("Epoxy2AirBorderSurface", physiEpoxy, physiWorld, polishedBlackPainted);


  //--------- Visualization attributes -------------------------------
  G4VisAttributes* ScintVisAtt = new G4VisAttributes(G4Colour(1.0,0.0,1.0, 0.6));
  G4VisAttributes* GreaseVisAtt = new G4VisAttributes(G4Colour(1.0, 1.0, 0.0, 0.8));
  G4VisAttributes* CathodeVisAtt = new G4VisAttributes(G4Colour(1.0, 0.0, 1.0, 0.5));
  G4VisAttributes* LensVisAtt = new G4VisAttributes(G4Colour(0.0, 1.0, 0.0, 0.8));

  CathodeVisAtt->SetForceWireframe(true);

  logicWorld->SetVisAttributes(G4VisAttributes::Invisible);
  logicScint->SetVisAttributes(ScintVisAtt);
  logicCathode->SetVisAttributes(CathodeVisAtt);
  logicGrease->SetVisAttributes(GreaseVisAtt);
  //logicLens->SetVisAttributes(LensVisAtt);


  //G4OpticalSurface* opticalSurface = dynamic_cast <G4OpticalSurface*>(muS->GetSurface(physiCone, physiWorld)->GetSurfaceProperty());
  //if (opticalSurface) opticalSurface->DumpInfo();


  return physiWorld;
}


void DetectorConstruction::updateWorldLength()
{
	G4double detectorlength = fScintDimX + fScintDimY + fScintDimZ + fGreaseThickness + fEpoxyDimX + fEpoxyDimY + fEpoxyThickness;

	fWorldLength = std::max(detectorlength, fSourceDistance)*2.0;
}



