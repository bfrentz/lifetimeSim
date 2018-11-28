#include "SteppingAction.hh"
#include "EventAction.hh"
#include "DetectorConstruction.hh"

#include "G4Step.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolume.hh"


SteppingAction::SteppingAction(EventAction* eventAction)
    : G4UserSteppingAction(),
      m_eventAction(eventAction)
{
        m_detectorConstruction
            = static_cast<const DetectorConstruction*>
              (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
        m_maxNumberHPGe = m_detectorConstruction->GetMaximumNumberOfHPGe();
    
    m_scoringVolumes = new G4LogicalVolume*[m_maxNumberHPGe];
}


SteppingAction::~SteppingAction()
{
    delete[] m_scoringVolumes;
}


void SteppingAction::UserSteppingAction(const G4Step* step)
{
    if (!m_scoringVolumesInitialized)
    {
        for (size_t iHPGe = 0; iHPGe < m_maxNumberHPGe; iHPGe++)
        {
            m_scoringVolumes[iHPGe] = m_detectorConstruction->GetScoringVolume(iHPGe);
            G4cout << "Initializing scoring volume " << iHPGe << ": " << m_scoringVolumes[iHPGe] << G4endl;    
        }

        m_scoringVolumesInitialized = true;
    }

    // get volume of the current step
    G4LogicalVolume* volume
        = step->GetPreStepPoint()->GetTouchableHandle()
          ->GetVolume()->GetLogicalVolume();

    // check if we are in scoring volume
    bool inSensitiveVolume = false;
    size_t thisHPGe;

    for (size_t iHPGe = 0; iHPGe < m_maxNumberHPGe; iHPGe++)
    {
        if (volume == m_scoringVolumes[iHPGe])
        {
            inSensitiveVolume = true;
            thisHPGe = iHPGe;
            break;
        }
    }

    if (!inSensitiveVolume)
    {
        //if (volume->GetName() != "worldLog")
        //{
        //    G4cout << volume << " " << volume->GetName() << " - Not in sensitve volume." << G4endl;
        //    G4cout << " " << m_scoringVolumes[0] << G4endl;
        //    G4cout << " " << m_scoringVolumes[1] << G4endl;
        //    G4cout << G4endl;
        //}
        return;
    }

    // collect energy deposited in this step
    const G4double edepStep = step->GetTotalEnergyDeposit();
    m_eventAction->AddEdep(thisHPGe, edepStep);
}
