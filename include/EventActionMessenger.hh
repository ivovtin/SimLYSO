#ifndef EventActionMessenger_h
#define EventActionMessenger_h 1

#include <globals.hh>
#include <G4UImessenger.hh>

class G4UIdirectory;
class G4UIcommand;
class G4UIcmdWithAString;
class G4UIcmdWithABool;

class EventAction;

class EventActionMessenger : public G4UImessenger{
public:
        EventActionMessenger(EventAction *_p);
        ~EventActionMessenger();

public:
    void SetNewValue(G4UIcommand *_command, G4String _value);
    G4String GetCurrentValue(G4UIcommand *_command);

private:
        EventAction *parent;

        G4UIdirectory *myActionDirectory;

        G4UIcmdWithAString *setNameRootFile;

        G4UIcmdWithABool *setZoomMode;
};

#endif // EventActionMessenger_h

