#ifndef ActionInitialization_hh
#define ActionInitialization_hh

#include "G4VUserActionInitialization.hh"
#include "EnergyHistogram.hh"

class ActionInitialization : public G4VUserActionInitialization
{
public:
    ActionInitialization();
    virtual ~ActionInitialization();

    virtual void BuildForMaster() const;
    virtual void Build() const;

private:
	size_t m_maxNumberHPGe = 0;
    EnergyHistogram **m_energyHistogram = nullptr;
};

#endif // #ifndef ActionInitialization_hh

