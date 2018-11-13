#ifndef PrimaryGeneratorAction_hh
#define PrimaryGeneratorAction_hh

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"

class G4ParticleGun;
class G4Event;

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
    PrimaryGeneratorAction();
    virtual ~PrimaryGeneratorAction();

    virtual void GeneratePrimaries(G4Event* event);
    const G4ParticleGun* GetParticleGun() const
    {
        return m_particleGun;
    }

private:
    G4ParticleGun* m_particleGun = nullptr;
};

#endif // #ifndef PrimaryGeneratorAction_hh
