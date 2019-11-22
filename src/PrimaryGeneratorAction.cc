#include <iostream>

//#include </Users/macalex/geant4/geant4.10.02.p02-install/include/Geant4/CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/SystemOfUnits.h>

#include "PrimaryGeneratorMessenger.hh"
#include "PrimaryGeneratorAction.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "globals.hh"
#include "Randomize.hh"
#include "Histo.hh"

using namespace CLHEP;
using namespace std;

void PrimaryGeneratorAction::CommonPart()
{
    G4int n_particle = 1;
    particleGun  = new G4ParticleGun(n_particle);

    // default particle kinematic
    namePrimary = "e-";
    //namePrimary = "mu-";
    //namePrimary = "pi-";
    particleTable = G4ParticleTable::GetParticleTable();
    PrimaryDef = particleTable->FindParticle(namePrimary); //gamma e-
    particleGun->SetParticleDefinition(PrimaryDef);
    
    //particleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.));
    particleGun->SetParticleMomentumDirection(G4ThreeVector(0.,1.,0.));   //at y
    //particleGun->SetParticleMomentumDirection(G4ThreeVector(0.,-1.,1.));   //x`
    energyPrimary = 999.*MeV;
    mass = PrimaryDef->GetPDGMass();
        if( energyPrimary<mass )
                energyPrimary = mass;
    particleGun->SetParticleEnergy(energyPrimary); 
    //particleGun->SetParticlePosition(G4ThreeVector(0.*mm ,0.*mm, -200.*cm));
    //particleGun->SetParticlePosition(G4ThreeVector(0.*cm ,0.*cm, -5.*cm));
    particleGun->SetParticlePosition(G4ThreeVector(0.*cm , -5.*cm, 0.*cm));
    //particleGun->SetParticlePosition(G4ThreeVector(0.*cm , 5.*cm, -4.85*cm));
}

double PrimaryGeneratorAction::GetParticleEnergy_keV()
{          
	return particleGun->GetParticleEnergy() / keV;
}

PrimaryGeneratorAction::PrimaryGeneratorAction()
{
  CommonPart();
  xrType = MONO;
  generatorMessenger = new PrimaryGeneratorMessenger(this);
}

PrimaryGeneratorAction::PrimaryGeneratorAction(char* fname)
{
  CommonPart();
  xrType = SPECTER;

  char   buffer[256];
  char   seps[]=" \t;";	  //separatoren

  nMaxDataLines=0;
  dMaxWeight  = 1;


  cout << "Read Specter: " << fname << endl;
  FILE* in =  fopen(fname,"r");
  if(in==NULL)
  {
   cout << "Specter of X-Tube not loaded!!! " << fname << endl;
   return;
  }
  while (!feof(in) && nMaxDataLines<MAX_SPECTER_DATALINES)
  {
    fgets(buffer,sizeof(buffer)-1,in);
    cout << "Read of " << nMaxDataLines << " line: " << buffer <<endl;
    if (strlen(buffer)>2)
    {
    	if (!strstr(buffer,"//"))
    	{
    	  dEnergy[nMaxDataLines]  =atoi (strtok(buffer,seps));
    	  dWeight[nMaxDataLines]  =atof (strtok(NULL,  seps));
          if(dWeight[nMaxDataLines]>dMaxWeight)dMaxWeight = dWeight[nMaxDataLines];
          nMaxDataLines++;
       	}
    }
  }
  cout << "Specter " << fname << "loaded! There are " << nMaxDataLines << " datalines." <<endl;

  fclose(in);

}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete particleGun;
  delete generatorMessenger;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
	double r1;
	double r2;
	double x, y, z;

	//-------------------------------------
	switch (xrType)
	{
	case MONO:
		break;

	case SPECTER:
		do
		{
			r1 = nMaxDataLines*G4UniformRand();
			r2 = dMaxWeight*G4UniformRand();
		} while (r2 > dWeight[(int)r1]);
		particleGun->SetParticleEnergy(r1*keV);//keV
		break;
	}

	//-------------------------------------
	G4ThreeVector xyzPosition = particleGun->GetParticlePosition();
	//do
	//{
    //x = 0*mm; //75.0*(2 * G4UniformRand() - 1)*mm;
    //y = 0*mm; //10.0*(2 * G4UniformRand() - 1)*mm;
	//} while ((abs(x) < 3.6) || (abs(y) < 3.6));
	x = xyzPosition.x();
	y = xyzPosition.y();
	z = xyzPosition.z();
    particleGun->SetParticlePosition(G4ThreeVector(x, y, z));

  particleGun->GeneratePrimaryVertex(anEvent);
  
  positionY=y;
  energyPrimary=particleGun->GetParticleEnergy();
  cout << "New event started! " << anEvent->GetEventID() << " Particle energy: "  << energyPrimary << " MeV. "<< endl; 


  //G4int  n_event = anEvent->GetEventID();
  //if((n_event%100)==0)
  //{
  //  cout << "New event started! " << anEvent->GetEventID() << " Particle energy: "  << particleGun->GetParticleEnergy()/keV  << " keV. "<< endl; 
  //} 
}

//============================
// SetParticle
//============================
void PrimaryGeneratorAction::SetParticle(G4String _name){
        namePrimary = _name;

        PrimaryDef = particleTable->FindParticle(namePrimary);
        particleGun->SetParticleDefinition(PrimaryDef);
        G4cout << "\tSetting name of primary: \"" << particleGun->GetParticleDefinition()->GetParticleName() << "\"" << G4endl;

    return;
}

//============================
// SetEnergy
//============================
void PrimaryGeneratorAction::SetEnergy(double _energy){
        energyPrimary = _energy;

        G4ParticleDefinition *_primaryParticle = particleGun->GetParticleDefinition();
        mass = _primaryParticle->GetPDGMass();
        if( energyPrimary<mass )
                energyPrimary = mass;

    particleGun->SetParticleEnergy(energyPrimary);
        G4cout << "\tSetting energy of primary: " << particleGun->GetParticleEnergy()/MeV << " [MeV]" << G4endl;

    return;
}









