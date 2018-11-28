#include "PrimaryGeneratorAction.hh"

#include "G4LogicalVolume.hh"
#include "G4Box.hh"

#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4ChargedGeantino.hh"
#include "G4IonTable.hh"

#include "G4SystemOfUnits.hh"
using CLHEP::MeV;

#include "Randomize.hh"
#include "G4RandomDirection.hh"


PrimaryGeneratorAction::PrimaryGeneratorAction()
    : G4VUserPrimaryGeneratorAction()
{
    const G4int nParticles = 1;
    m_particleGun  = new G4ParticleGun(nParticles);

    // default particle kinematic
    auto particleTable = G4ParticleTable::GetParticleTable();

    // create name
    G4String particleName;

    auto particleDefinition = particleTable->FindParticle(particleName="chargedgeantino");
    m_particleGun->SetParticleDefinition(particleDefinition);

    m_particleGun->SetParticleEnergy(1*MeV);
    m_particleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.));
    m_particleGun->SetParticlePosition(G4ThreeVector(-1.25*cm/std::sqrt(2), 
                                                    0, 
                                                    -std::sqrt(2)*(1.25*cm/2 + 0.50*mm))); // Front and center of target
}


PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
    //delete particleDefinition;
    delete m_particleGun;
}


void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
    // Beginning of an event
    G4ParticleDefinition* particle = m_particleGun->GetParticleDefinition();
    if (particle == G4ChargedGeantino::ChargedGeantino()) {
        //15O
        G4int Z = 8, A = 15;
        G4double ionCharge   = 0.*eplus;
        G4double excitEnergy = 6793.1*keV;

        G4ParticleDefinition* ion = G4IonTable::GetIonTable()->GetIon(Z,A,excitEnergy);
        ion->SetPDGLifeTime(1e-15*second);
        m_particleGun->SetParticleDefinition(ion);
        m_particleGun->SetParticleCharge(ionCharge);
  }


    m_particleGun->SetParticleMomentumDirection(G4ThreeVector(0, 0, 1.0));
    m_particleGun->SetParticlePosition(G4ThreeVector(-1.25*cm/std::sqrt(2), 
                                                     0, 
                                                     -std::sqrt(2)*(1.25*cm/2 + 0.50*mm))); // Front and center of target
    m_particleGun->GeneratePrimaryVertex(anEvent);
}

