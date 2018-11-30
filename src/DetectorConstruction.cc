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
    m_hpgeDetector = new HPGeDetector*[m_nHPGe_max];
    m_scoringVolumes = new G4LogicalVolume*[m_nHPGe_max];

    for (size_t iHPGe = 0; iHPGe < m_nHPGe_max; iHPGe++)
    {
        m_hpgeDetector[iHPGe] = new HPGeDetector(iHPGe);
        m_scoringVolumes[iHPGe] = nullptr;
    }
    m_targetChamber = new TargetChamber();
}


DetectorConstruction::~DetectorConstruction()
{
    for (size_t iHPGe = 0; iHPGe < m_nHPGe_max; iHPGe++)
    {
        delete m_hpgeDetector[iHPGe];
    }
    delete[] m_hpgeDetector;
    delete[] m_scoringVolumes;
    
    delete m_targetChamber;
}


G4VPhysicalVolume* DetectorConstruction::Construct()
{

    auto worldSolid = new G4Box("worldSolid", 1*m, 1*m, 1*m);
    auto matAir = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");
    auto worldLog = new G4LogicalVolume(worldSolid, matAir, "worldLog");
    worldLog->SetVisAttributes(G4VisAttributes::Invisible);
    auto physWorld = new G4PVPlacement(nullptr, G4ThreeVector(), worldLog, "worldPhys", nullptr, 0, 0);

    for (size_t iHPGe = 0; iHPGe < m_nHPGe_max; iHPGe++)  
    {  
        m_hpgeDetector[iHPGe]->SetMotherVolume(worldLog);
        m_hpgeDetector[iHPGe]->Build();
    }

    for (size_t iHPGe = 0; iHPGe < m_nHPGe_max; iHPGe++)
    {
        m_scoringVolumes[iHPGe] = m_hpgeDetector[iHPGe]->GetScoringVolume();
    }

    m_targetChamber->SetMotherVolume(worldLog);
    m_targetChamber->Build();


    // return physical world
    return physWorld;
}

