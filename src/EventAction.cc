#include "EventAction.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"
#include "DetectorConstruction.hh"

EventAction::EventAction(EnergyHistogram** energyHistogram)
    : G4UserEventAction(),
      m_energyHistogram(energyHistogram)
{
        const DetectorConstruction* detectorConstruction
            = static_cast<const DetectorConstruction*>
              (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
        m_maxNumberHPGe = detectorConstruction->GetMaximumNumberOfHPGe();

        m_Edep = new G4double[m_maxNumberHPGe];
}


EventAction::~EventAction()
{
	delete[] m_Edep;
}


void EventAction::BeginOfEventAction(const G4Event* /*event*/)
{
	for (size_t iHPGe = 0; iHPGe < m_maxNumberHPGe; iHPGe++)
	{
		m_Edep[iHPGe] = 0.0;
	}
}


void EventAction::EndOfEventAction(const G4Event* /*event*/)
{
 	for (size_t iHPGe = 0; iHPGe < m_maxNumberHPGe; iHPGe++)
	{
		m_energyHistogram[iHPGe]->Fill(m_Edep[iHPGe]);
	}
}
