#ifndef PrimaryGeneratorAction_hh
#define PrimaryGeneratorAction_hh

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4UImessenger.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"

#include "G4SystemOfUnits.hh"
using CLHEP::second;

class G4ParticleGun;
class G4Event;
class G4UIcmdWithADoubleAndUnit;

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction, public G4UImessenger
{
public:
    PrimaryGeneratorAction();
    virtual ~PrimaryGeneratorAction();

    virtual void GeneratePrimaries(G4Event* event);
    
    const G4ParticleGun* GetParticleGun() const
    {
        return m_particleGun;
    }

    void setNewValue(G4UIcommand *cmd, G4String newValue);

private:
    G4ParticleGun* m_particleGun = nullptr;

    G4double m_lifetime = 1e-15*second;

    G4ThreeVector m_position;
    G4ThreeVector m_surfaceNormal, m_radiusVector1, m_radiusVector2;
    G4double m_radius = 0;

    const bool m_verbose = false;
};

#endif // #ifndef PrimaryGeneratorAction_hh
