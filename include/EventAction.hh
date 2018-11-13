#ifndef EventAction_hh
#define EventAction_hh

#include "G4UserEventAction.hh"
#include "globals.hh"

#include "EnergyHistogram.hh"


class EventAction : public G4UserEventAction
{
public:
    EventAction(EnergyHistogram* energyHistogram);
    virtual ~EventAction();

    virtual void BeginOfEventAction(const G4Event* /*event*/);
    virtual void EndOfEventAction(const G4Event* /*event*/);

    void AddEdep(const G4double edep)
    {
        m_Edep += edep;
    }

private:
    EnergyHistogram* m_energyHistogram = nullptr;
    G4double m_Edep = 0.0;
};

#endif // #ifndef EventAction_hh

