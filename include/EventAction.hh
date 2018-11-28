#ifndef EventAction_hh
#define EventAction_hh

#include "G4UserEventAction.hh"
#include "globals.hh"

#include "EnergyHistogram.hh"

#include "G4SystemOfUnits.hh"
using CLHEP::keV;


class EventAction : public G4UserEventAction
{
public:
    EventAction(EnergyHistogram** energyHistogram);
    virtual ~EventAction();

    virtual void BeginOfEventAction(const G4Event* /*event*/);
    virtual void EndOfEventAction(const G4Event* /*event*/);

    void AddEdep(const size_t iHPGe, const G4double edep)
    {
        m_Edep[iHPGe] += edep;
        //G4cout << "Adding " << edep/keV << "keV to detector " << iHPGe << G4endl;
    }

private:
    EnergyHistogram** m_energyHistogram = nullptr;
    G4double* m_Edep = nullptr;
    size_t m_maxNumberHPGe = 0;
};

#endif // #ifndef EventAction_hh

