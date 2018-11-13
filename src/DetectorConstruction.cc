#include "DetectorConstruction.hh"

#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4VisAttributes.hh"

#include "G4SystemOfUnits.hh"
using CLHEP::m;



DetectorConstruction::DetectorConstruction()
    : G4VUserDetectorConstruction()
{
    m_hpgeDetector = new HPGeDetector();
}


DetectorConstruction::~DetectorConstruction()
{
    delete m_hpgeDetector;
}


G4VPhysicalVolume* DetectorConstruction::Construct()
{

    auto worldSolid = new G4Box("worldSolid", 1*m, 1*m, 1*m);
    auto matAir = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");
    auto worldLog = new G4LogicalVolume(worldSolid, matAir, "worldLog");
    worldLog->SetVisAttributes(G4VisAttributes::Invisible);
    auto physWorld = new G4PVPlacement(nullptr, G4ThreeVector(), worldLog, "worldPhys", nullptr, 0, 0);

    m_hpgeDetector->SetMotherVolume(worldLog);
    m_hpgeDetector->Build();

    m_scoringVolume = m_hpgeDetector->GetScoringVolume();


    // return physical world
    return physWorld;
}

