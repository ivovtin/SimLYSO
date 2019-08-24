#include <G4UIdirectory.hh>
#include <G4UIcmdWithAString.hh>
#include <G4UIcmdWithABool.hh>

#include "EventActionMessenger.hh"
#include "EventAction.hh"

EventActionMessenger::EventActionMessenger(EventAction *_p): parent(_p)
{
	G4String ActionDir = "/MTDAction/";

	myActionDirectory = new G4UIdirectory(ActionDir.c_str());
	myActionDirectory->SetGuidance("MTD EventAction control commands.");

	// set name of rootFile
	G4String _l_nameRootFile = ActionDir + "nameRootFile";
	setNameRootFile = new G4UIcmdWithAString(_l_nameRootFile.c_str(), this);
	setNameRootFile->SetGuidance("Specify the name of the output root file (by default \"MTD.root\"");
	setNameRootFile->SetParameterName("file.root", true);
    	setNameRootFile->SetDefaultValue("MTD.root");
	setNameRootFile->AvailableForStates(G4State_PreInit, G4State_Init, G4State_Idle);

	// set zoomMode
	G4String _l_zoomMode = ActionDir + "zoomMode";
	setZoomMode = new G4UIcmdWithABool(_l_zoomMode.c_str(), this);
	setZoomMode->SetParameterName("zoomMode", false, false);
        setZoomMode->SetDefaultValue(false);
}

EventActionMessenger::~EventActionMessenger(){
	delete setNameRootFile;
	delete setZoomMode;
    	delete myActionDirectory;
}

void EventActionMessenger::SetNewValue(G4UIcommand *_command, G4String _value){
	if( _command==setNameRootFile ){
		parent->SetNameRootFile(_value);
		parent->OpenRootFile();
	} else if( _command==setZoomMode ){
		parent->SetZoomMode(setZoomMode->GetNewBoolValue(_value));
	}
}

G4String EventActionMessenger::GetCurrentValue(G4UIcommand *_command){
	G4String _value = "";

    if( _command==setNameRootFile )
		_value = parent->GetNameRootFile();

	return _value;
}

