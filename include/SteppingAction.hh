#ifndef SteppingAction_hh
#define SteppingAction_hh

#include "G4UserSteppingAction.hh"
#include "globals.hh"

class EventAction;

class G4LogicalVolume;
class DetectorConstruction;

class SteppingAction : public G4UserSteppingAction
{
public:
    SteppingAction(EventAction* eventAction);
    virtual ~SteppingAction();

    virtual void UserSteppingAction(const G4Step* step);

private:
    EventAction* m_eventAction = nullptr;
    const DetectorConstruction* m_detectorConstruction;
    G4LogicalVolume** m_scoringVolumes = nullptr;
    size_t m_maxNumberHPGe = 0;
    bool m_scoringVolumesInitialized = false;
};

#endif // #ifndef SteppingAction_hh

