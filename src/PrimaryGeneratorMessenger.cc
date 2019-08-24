
#include <G4UIdirectory.hh>
#include <G4UIcmdWithAString.hh>
#include <G4UIcmdWithAnInteger.hh>
#include <G4UIcmdWithADoubleAndUnit.hh>

#include "PrimaryGeneratorMessenger.hh"
#include "PrimaryGeneratorAction.hh"

PrimaryGeneratorMessenger::PrimaryGeneratorMessenger(PrimaryGeneratorAction *_p) : parent(_p){
	G4String GenDir = "/MTDGenerator/";

	myGenDirectory = new G4UIdirectory(GenDir.c_str());
	myGenDirectory->SetGuidance("MTD PrimaryGenerator control commands.");

	//------------------------------------------
	// Energy of primary particle
	//------------------------------------------
	G4String _lEnergy = GenDir + "energy";
	setEnergy = new G4UIcmdWithADoubleAndUnit(_lEnergy.c_str(), this);
        setEnergy->SetGuidance("Mean energy of primaries");
	setEnergy->SetParameterName("energy", true);
	setEnergy->SetRange("energy>=0.");
	setEnergy->SetDefaultValue(1800.);
	setEnergy->SetDefaultUnit("MeV");

	//------------------------------------------
	// Type of primary particle
	//------------------------------------------
        G4String _lParticle = GenDir + "particle";
	setParticle = new G4UIcmdWithAString(_lParticle.c_str(), this);
	setParticle->SetGuidance("Name of primaries");
	setParticle->SetParameterName("particle", true);
	setParticle->SetDefaultValue("e-");
}

PrimaryGeneratorMessenger::~PrimaryGeneratorMessenger(){
        delete setParticle;
	delete setEnergy;

        delete myGenDirectory;
}

void PrimaryGeneratorMessenger::SetNewValue(G4UIcommand *_command, G4String _value){
    if( _command==setEnergy )
		parent->SetEnergy( setEnergy->GetNewDoubleValue(_value) );
	else if( _command==setParticle )
		parent->SetParticle(_value);		
}

G4String PrimaryGeneratorMessenger::GetCurrentValue(G4UIcommand *_command){
	G4String _value = "";

    if( _command==setEnergy )
		_value = setEnergy->ConvertToString(parent->GetEnergy(), "MeV");
	if( _command==setParticle )
		_value = parent->GetParticle();

    return _value;
}


