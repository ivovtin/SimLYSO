#include "PhysicsList.hh"

#include "ExtraPhysics.hh"
#include "OpticalPhysics.hh"

#include "G4LossTableManager.hh"

#include "globals.hh"
#include "G4ProcessManager.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleTable.hh"
#include "G4VPhysicsConstructor.hh"

#include "G4PhysListFactory.hh"

#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"

#include "StepMax.hh"

#include "G4ProcessTable.hh"

#include "G4PionDecayMakeSpin.hh"
#include "G4DecayWithSpin.hh"

#include "G4DecayTable.hh"
#include "G4MuonDecayChannelWithSpin.hh"
#include "G4MuonRadiativeDecayChannelWithSpin.hh"

#include "G4RadioactiveDecayPhysics.hh"
#include "G4StepLimiterPhysics.hh"

//#include "/Users/macalex/geant4/geant4.10.02.p02-install/include/Geant4/CLHEP/Units/SystemOfUnits.h"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//#define aParticleIterator ((subInstanceManager.offset[g4vpcInstanceID])._aParticleIterator)
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PhysicsList::PhysicsList(G4String physName) : G4VModularPhysicsList(), fStepMaxProcess(NULL), fOpticalPhysics(NULL), fAbsorptionOn(true)
{
    G4LossTableManager::Instance();

    defaultCutValue  = 1.*mm;
    fCutForGamma     = defaultCutValue;
    fCutForElectron  = defaultCutValue;
    fCutForPositron  = defaultCutValue;

	//Check physics name
	if(!physName.empty())
	{
		G4PhysListFactory factory;
		G4VModularPhysicsList* phys = factory.GetReferencePhysList(physName);
		phys->SetVerboseLevel(3);

		for (G4int i = 0; ; ++i)
		{
			G4VPhysicsConstructor* elem = const_cast<G4VPhysicsConstructor*> (phys->GetPhysics(i));
			if (elem == NULL) break;
			G4cout << "RegisterPhysics: " << elem->GetPhysicsName() << G4endl;
			RegisterPhysics(elem);
		}
	}
	else
	{
		//add default EM physics
	}

	G4StepLimiterPhysics* stepLimiterPhysics = new G4StepLimiterPhysics();
// since 21/11/2017	
	stepLimiterPhysics->SetApplyToAll(true);
	RegisterPhysics(stepLimiterPhysics);

    RegisterPhysics(new ExtraPhysics());
    RegisterPhysics(fOpticalPhysics = new OpticalPhysics(fAbsorptionOn));  //include optic physics

    RegisterPhysics(new G4RadioactiveDecayPhysics());
    fStepMaxProcess = new StepMax();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PhysicsList::~PhysicsList()
{
    if(fStepMaxProcess) delete fStepMaxProcess;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsList::ConstructParticle()
{
	SetVerboseLevel(3);
    G4VModularPhysicsList::ConstructParticle();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsList::ConstructProcess()
{
    G4VModularPhysicsList::ConstructProcess();
    AddStepMax();
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsList::SetCuts()
{
    if (verboseLevel >0) {
        G4cout << "PhysicsList::SetCuts:";
        G4cout << "CutLength : " << G4BestUnit(defaultCutValue,"Length")
               << G4endl;
    }

    // set cut values for gamma at first and for e- second and next for e+,
    // because some processes for e+/e- need cut values for gamma
    SetCutValue(fCutForGamma, "gamma");
    SetCutValue(fCutForElectron, "e-");
    SetCutValue(fCutForPositron, "e+");

    if (verboseLevel>0) DumpCutValuesTable();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsList::SetCutForGamma(G4double cut)
{
    fCutForGamma = cut;
    SetParticleCuts(fCutForGamma, G4Gamma::Gamma());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsList::SetCutForElectron(G4double cut)
{
    fCutForElectron = cut;
    SetParticleCuts(fCutForElectron, G4Electron::Electron());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsList::SetCutForPositron(G4double cut)
{
    fCutForPositron = cut;
    SetParticleCuts(fCutForPositron, G4Positron::Positron());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsList::SetStepMax(G4double step)
{
  fStepMaxProcess->SetStepMax(step);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

StepMax* PhysicsList::GetStepMaxProcess()
{
  return fStepMaxProcess;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsList::AddStepMax()
{
  // Step limitation seen as a process

  auto theParticleIterator=GetParticleIterator();
  auto particleIterator=theParticleIterator;
  particleIterator->reset();
  while ((*particleIterator)()){
      G4ParticleDefinition* particle = particleIterator->value();
      G4ProcessManager* pmanager = particle->GetProcessManager();

      if (fStepMaxProcess->IsApplicable(*particle) && !particle->IsShortLived())
      {
         if (pmanager) pmanager ->AddDiscreteProcess(fStepMaxProcess);
      }
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsList::SetNbOfPhotonsCerenkov(G4int maxNumber)
{
   fOpticalPhysics->SetNbOfPhotonsCerenkov(maxNumber);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsList::SetVerbose(G4int verbose)
{
   fOpticalPhysics->GetCerenkovProcess()->SetVerboseLevel(verbose);
   fOpticalPhysics->GetScintillationProcess()->SetVerboseLevel(verbose);
   fOpticalPhysics->GetAbsorptionProcess()->SetVerboseLevel(verbose);
   fOpticalPhysics->GetRayleighScatteringProcess()->SetVerboseLevel(verbose);
   fOpticalPhysics->GetMieHGScatteringProcess()->SetVerboseLevel(verbose);
   fOpticalPhysics->GetBoundaryProcess()->SetVerboseLevel(verbose);
}
