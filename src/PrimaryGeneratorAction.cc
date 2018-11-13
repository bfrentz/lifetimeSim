#include "PrimaryGeneratorAction.hh"

#include "G4LogicalVolume.hh"
#include "G4Box.hh"

#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"

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

    auto particleDefinition = particleTable->FindParticle("gamma");
    m_particleGun->SetParticleDefinition(particleDefinition);

    m_particleGun->SetParticleEnergy(1*MeV);
    m_particleGun->SetParticlePosition(G4ThreeVector(0,0,0));
}


PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
    delete m_particleGun;
}


void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
    m_particleGun->SetParticleMomentumDirection(G4RandomDirection());
    m_particleGun->GeneratePrimaryVertex(anEvent);
}

