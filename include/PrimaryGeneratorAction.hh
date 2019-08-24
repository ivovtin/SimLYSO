#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include <stdlib.h>
#include <globals.hh>
#include "G4VUserPrimaryGeneratorAction.hh"
#include <G4ParticleGun.hh>
#include <G4INCLParticle.hh>

#define  MAX_SPECTER_DATALINES  150

enum XRAY_TYPE
{
	MONO,
	SPECTER
};

class G4ParticleGun;
class G4Event;

class PrimaryGeneratorMessenger;

//extern double ex_energy;

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  private:
        void CommonPart();
        G4ParticleGun *particleGun;
        PrimaryGeneratorMessenger *generatorMessenger;
        
        //------------------------------------------
        // Energy of primary particle
        //------------------------------------------
        double energyPrimary; 
        //------------------------------------------
        // Type of primary particle
        //------------------------------------------
        G4String namePrimary; 
 
        double positionY; 
        double mass; 
               
        G4ParticleTable *particleTable;
        G4ParticleDefinition *PrimaryDef;
	
	XRAY_TYPE xrType;
        int       nMaxDataLines;
        double    dEnergy[MAX_SPECTER_DATALINES];
        double    dWeight[MAX_SPECTER_DATALINES];
        double    dMaxWeight;
  public:
        PrimaryGeneratorAction();              //mono
        PrimaryGeneratorAction(char* fname);   //specter
        virtual ~PrimaryGeneratorAction();

  public:
        virtual void   GeneratePrimaries(G4Event* anEvent);
        
        // Generator messanger <-> primary energy
        void SetEnergy(double _energy);
        double GetEnergy() const { return energyPrimary; }
        double GetMass() const { return mass; }
        double GetPositionY() { return positionY; }
        
        // Generator messanger <-> type of primary particle
        void SetParticle(G4String _name);
        const G4String& GetParticle() const {
                return particleGun->GetParticleDefinition()->GetParticleName();
        }

	double GetParticleEnergy_keV();
	void   ReadSpectum(const char* fname);
	
};

#endif


