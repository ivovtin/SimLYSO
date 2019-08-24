#ifndef TrackingAction_h
#define TrackingAction_h 1

#include "G4UserTrackingAction.hh"

class DetectorConstruction;

class TrackingAction : public G4UserTrackingAction
{

  public:

    TrackingAction(DetectorConstruction* myDC);
    virtual ~TrackingAction() {};

    virtual void PreUserTrackingAction(const G4Track*);
    virtual void PostUserTrackingAction(const G4Track*);

  private:
    DetectorConstruction* myDetector;

};

#endif

