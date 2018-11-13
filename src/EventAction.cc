#include "EventAction.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"

EventAction::EventAction(EnergyHistogram* energyHistogram)
    : G4UserEventAction(),
      m_energyHistogram(energyHistogram)
{}


EventAction::~EventAction()
{}


void EventAction::BeginOfEventAction(const G4Event* /*event*/)
{
    m_Edep = 0.0;
}


void EventAction::EndOfEventAction(const G4Event* /*event*/)
{
    m_energyHistogram->Fill(m_Edep);
}
