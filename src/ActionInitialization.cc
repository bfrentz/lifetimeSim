#include "ActionInitialization.hh"

#include "PrimaryGeneratorAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"
#include "DetectorConstruction.hh"

#include <string>
    using std::to_string;

#include "G4SystemOfUnits.hh"
using CLHEP::keV;
#include "G4RunManager.hh"

ActionInitialization::ActionInitialization()
    : G4VUserActionInitialization()
{
    const DetectorConstruction* detectorConstruction
            = static_cast<const DetectorConstruction*>
              (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
        m_maxNumberHPGe = detectorConstruction->GetMaximumNumberOfHPGe();

    m_energyHistogram = new EnergyHistogram*[m_maxNumberHPGe];
    for (size_t iHPGe = 0; iHPGe < m_maxNumberHPGe; iHPGe++)
    {
		m_energyHistogram[iHPGe] = new EnergyHistogram(16384, 0.0*keV, 16384.0*keV);
    }
}


ActionInitialization::~ActionInitialization()
{
    for (size_t iHPGe = 0; iHPGe < m_maxNumberHPGe; iHPGe++)
    {
    	m_energyHistogram[iHPGe]->Write("./histogram_" + to_string(iHPGe) + ".dat");
		delete m_energyHistogram[iHPGe];
    }
    delete[] m_energyHistogram;
}


void ActionInitialization::BuildForMaster() const
{
}


void ActionInitialization::Build() const
{
    SetUserAction(new PrimaryGeneratorAction());

    auto eventAction = new EventAction(m_energyHistogram);
    SetUserAction(eventAction);

    SetUserAction(new SteppingAction(eventAction));
}
