#include "PrimaryGeneratorAction.hh"
#include "DetectorConstruction.hh"

#include "G4LogicalVolume.hh"
#include "G4Box.hh"

#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4ChargedGeantino.hh"
#include "G4IonTable.hh"

#include "G4SystemOfUnits.hh"
using CLHEP::MeV;
using CLHEP::mm;
using CLHEP::second;

#include "Randomize.hh"
#include "G4RandomDirection.hh"
#include "G4RunManager.hh"


PrimaryGeneratorAction::PrimaryGeneratorAction()
    : G4VUserPrimaryGeneratorAction()
{
    const G4int nParticles = 1;
    m_particleGun  = new G4ParticleGun(nParticles);

    // default particle kinematic
    auto particleTable = G4ParticleTable::GetParticleTable();

    // create name
    G4String particleName;

    // create variable for lifetime
    m_lifetime = 1e-15*second;

    auto particleDefinition = particleTable->FindParticle(particleName="chargedgeantino");
    m_particleGun->SetParticleDefinition(particleDefinition);

    m_particleGun->SetParticleEnergy(10.0*MeV);
    m_particleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.));
    m_particleGun->SetParticlePosition(G4ThreeVector()); 

    // Get target chamber to find source position
    const auto detectorConstruction
    = static_cast<const DetectorConstruction*>
      (G4RunManager::GetRunManager()->GetUserDetectorConstruction());

    const auto targetChamber = detectorConstruction->GetTargetChamber();

    m_position = targetChamber->GetTargetSurfacePosition();
    m_surfaceNormal = targetChamber->GetTargetSurfaceNormal();

    if (m_surfaceNormal.mag2() == 0) {
        G4cerr << "Surface normal has length zero!" << G4endl;
    }

    if (m_surfaceNormal.x() == 0 &&  m_surfaceNormal.y() == 0) {
        m_radiusVector1 = G4ThreeVector(1,0,0);
        m_radiusVector2 = G4ThreeVector(0,1,0);
    }
    else {
        m_radiusVector1 = G4ThreeVector(m_surfaceNormal.y(), -m_surfaceNormal.x(), 0).unit();
        m_radiusVector2 = m_surfaceNormal.cross(m_radiusVector1);
    }

    m_radius = 5*mm;

    if (m_verbose)
    {
        G4cout << "(PGA) Source position (center of circle): " << G4endl;
        G4cout << "(PGA)  " << m_position.x()/mm << "mm" << G4endl;
        G4cout << "(PGA)  " << m_position.y()/mm << "mm" << G4endl;
        G4cout << "(PGA)  " << m_position.z()/mm << "mm" << G4endl;
        G4cout << "(PGA)" << G4endl;
        G4cout << "(PGA) Source radius: " << m_radius/mm << "mm" << G4endl;
        G4cout << "(PGA)" << G4endl;
    }

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
        G4cout << "(PGA) Replacing charged geantino with ion." << G4endl;

        //15O
        G4int Z = 8, A = 15;
        G4double ionCharge   = 0.*eplus;
        G4double excitEnergy = 6793.1*keV;

        G4ParticleDefinition* ion = G4IonTable::GetIonTable()->GetIon(Z,A,excitEnergy);
        ion->SetPDGLifeTime(m_lifetime);
        G4cout << "(PGA) Assigning particle lifetime to " << ion->GetPDGLifeTime()/second << " seconds." << G4endl;
        m_particleGun->SetParticleDefinition(ion);
        m_particleGun->SetParticleCharge(ionCharge);
    }


    m_particleGun->SetParticleMomentumDirection(G4ThreeVector(0, 0, 1.0));

    G4double tmp1, tmp2;
    G4int nAttempts = 0;
    while (true)
    {
        nAttempts += 1;

        tmp1 = 0; //(2*G4Random::G4UniformRand())-1;
        tmp2 = 0; //(2*G4Random::G4UniformRand())-1;

        if (tmp1*tmp1 + tmp2*tmp2 < 1) {
            break;
        }
    }

    G4ThreeVector thisPosition = m_position + m_radius*(m_radiusVector1*tmp1 + m_radiusVector2*tmp2);

    if (m_verbose)
    {
        G4cout << "(PG) Sampled position in " << nAttempts << " samples:" << G4endl;
        G4cout << "(PG)  " << thisPosition.x()/mm << "mm" << G4endl;
        G4cout << "(PG)  " << thisPosition.y()/mm << "mm" << G4endl;
        G4cout << "(PG)  " << thisPosition.z()/mm << "mm" << G4endl;
        G4cout << "(PG)" << G4endl;
        G4cout << "(PG) Distance from center: " << (thisPosition - m_position).mag()/mm << "mm" << G4endl;
        G4cout << "(PG) Scalar product with normal vector: " << m_surfaceNormal*(thisPosition-m_position) << " (=0?)" << G4endl;
        G4cout << "(PG)" << G4endl;
    }
    m_particleGun->SetParticlePosition(thisPosition); // Front and center of target

    m_particleGun->GeneratePrimaryVertex(anEvent);
}

