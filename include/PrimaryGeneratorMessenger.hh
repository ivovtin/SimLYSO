#ifndef PrimaryGeneratorMessenger_h
#define PrimaryGeneratorMessenger_h 1

#include <globals.hh>
#include <G4UImessenger.hh>

class G4UIdirectory;
class G4UIcommand;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithAnInteger;
class G4UIcmdWithAString;

class PrimaryGeneratorAction;

class PrimaryGeneratorMessenger: public G4UImessenger{
public:
    PrimaryGeneratorMessenger(PrimaryGeneratorAction *_p);
        ~PrimaryGeneratorMessenger();

public:
    void SetNewValue(G4UIcommand *_command, G4String _values);
    G4String GetCurrentValue(G4UIcommand *_command);

private:
    PrimaryGeneratorAction *parent;

        G4UIdirectory *myGenDirectory;

    G4UIcmdWithADoubleAndUnit *setEnergy;
    G4UIcmdWithAString *setParticle;
};

#endif 

