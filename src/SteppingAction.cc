#include "SteppingAction.hh"
#include "EventAction.hh"
#include "DetectorConstruction.hh"

#include "G4Step.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolume.hh"


SteppingAction::SteppingAction(EventAction* eventAction)
    : G4UserSteppingAction(),
      m_eventAction(eventAction),
      m_scoringVolume(nullptr)
{}


SteppingAction::~SteppingAction()
{}


void SteppingAction::UserSteppingAction(const G4Step* step)
{
    if (!m_scoringVolume)
    {
        const DetectorConstruction* detectorConstruction
            = static_cast<const DetectorConstruction*>
              (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
        m_scoringVolume = detectorConstruction->GetScoringVolume();
    }

    // get volume of the current step
    G4LogicalVolume* volume
        = step->GetPreStepPoint()->GetTouchableHandle()
          ->GetVolume()->GetLogicalVolume();

    // check if we are in scoring volume
    if (volume != m_scoringVolume)
    {
        return;
    }

    // collect energy deposited in this step
    const G4double edepStep = step->GetTotalEnergyDeposit();
    m_eventAction->AddEdep(edepStep);
}
