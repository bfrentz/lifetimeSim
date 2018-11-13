#include "ActionInitialization.hh"

#include "PrimaryGeneratorAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"

#include "G4SystemOfUnits.hh"
using CLHEP::keV;

ActionInitialization::ActionInitialization()
    : G4VUserActionInitialization()
{
    m_energyHistogram = new EnergyHistogram(16384, 0.0*keV, 16384.0*keV);
}


ActionInitialization::~ActionInitialization()
{
    m_energyHistogram->Write("./histogram.dat");
    delete m_energyHistogram;
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
