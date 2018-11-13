#include "HPGeDetector.hh"

#include "globals.hh"

#include "G4NistManager.hh"
#include "G4Material.hh"
#include "G4Element.hh"

#include "G4Polycone.hh"
#include "G4Tubs.hh"
#include "G4Torus.hh"
#include "G4Sphere.hh"

#include "G4SubtractionSolid.hh"
#include "G4IntersectionSolid.hh"
#include "G4UnionSolid.hh"

#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4MultiFunctionalDetector.hh"
#include "G4PSEnergyDeposit.hh"

#include "CLHEP/Units/SystemOfUnits.h"

using CLHEP::g;
using CLHEP::mm;
using CLHEP::cm;
using CLHEP::cm3;
using CLHEP::perCent;
using CLHEP::deg;

using CLHEP::pi;

#include <string>
#include <stdexcept>
    using std::runtime_error;


// HPGe Detector Geometry
// --------------------------
//
// The reference position (0,0,0) is the front center of the front cap.
//

HPGeDetector::HPGeDetector() :
    GeometryObject("HPGeDetector")
{
        // Outer casing dimensions
        RegisterDimension("outerCasingDiameter", 108.0*mm);
        RegisterDimension("outerCasingLength", 305.0*mm);
        RegisterDimension("outerCasingFrontThickness", 1.0*mm);
        RegisterDimension("outerCasingRadialThickness", 1.0*mm);

        // Inner distance between outer casing and inner casing
        RegisterDimension("innerDetectorPosition", 4.0*mm);

        // thickness of mylar layer in front of detector
        RegisterDimension("mylarThickness", 0.03*mm);

        // Distance between outer casing and detector crystal
        RegisterDimension("detectorWrappingDistance", 1e-3*mm);

        // Inner casing dimensions
        RegisterDimension("innerCasingDiameter", 103.0*mm);
        RegisterDimension("innerCasingFrontThickness", 0.03*mm);
        RegisterDimension("innerCasingRadialThickness", 0.76*mm);
        RegisterDimension("innerCasingBackThickness", 3.2*mm);
        RegisterDimension("innerCasingLength", 130.0*mm + GetDimension("innerCasingFrontThickness") + GetDimension("mylarThickness"));
        RegisterDimension("innerCasingBackHoleDiameter", 22.0*mm);

        // Detector (crystal) dimensions
        RegisterDimension("detectorDiameter", 84.6*mm);
        RegisterDimension("detectorLength", 87.8*mm);
        RegisterDimension("detectorRoundedEdgeRadius", 10.0*mm);
        RegisterDimension("detectorHoleDiameter", 9.7*mm);
        RegisterDimension("detectorHoleDepth", 76.9*mm);
        
        // Crystal dead layer information
        RegisterDimension("detectorDeadLayerFront", 0.7*mm); // facing the target (excluding rounded front edges)
        RegisterDimension("detectorDeadLayerBack", 0.1*mm); // pointing away from the target
        RegisterDimension("detectorDeadLayerOutside", 0.7*mm); // radially outside (including rounded front edges)
        RegisterDimension("detectorDeadLayerInside", 0.3*1e-3*mm); // inner contact
}

HPGeDetector::~HPGeDetector()
{
}

G4VPhysicalVolume* HPGeDetector::Construct() {

    auto nistManager = G4NistManager::Instance();

    auto matAl = nistManager->FindOrBuildMaterial("G4_Al");
    //auto matB  = nistManager->FindOrBuildMaterial("G4_B"); // Boron contact
    auto matGe = nistManager->FindOrBuildMaterial("G4_Ge");
    //auto matLi  = nistManager->FindOrBuildMaterial("G4_Li"); // Lithium contact
    auto matMylar  = nistManager->FindOrBuildMaterial("G4_MYLAR");

    // ----- General parameters for the detector

    G4RotationMatrix noRotation;

    const G4double phiMin = 0.0*deg;
    const G4double phiMax = 360.0*deg;

    const G4double thetaMin = 0.0*deg;
    const G4double thetaMax = 180.0*deg;


    // Outer casing is a hollow cylinder (open on one side) of Aluminum
    const auto outerCasingMaterial = matAl; //

    // At the front of the inner casing is a thin layer of Mylar
    // (even though it's not clear from the drawing, whether it's first Mylar or
    // aluminum)
    const auto mylarMaterial = matMylar;

    // Inner casing is a hollow cylinder (with a hole on the back side) of Al
    const auto innerCasingMaterial = matAl;

    // The detector has a particular shape: a solid cylinder with a rounded
    // front edge and a bore hole from the back
    const auto detectorMaterial = matGe;


    // Construct and place outer casing
    // using G4Polycone
    {
        G4double zSegments[4] = {0.0,
                                 GetDimension("outerCasingFrontThickness"),
                                 GetDimension("outerCasingFrontThickness"),
                                 GetDimension("outerCasingLength")};

        G4double innerRadius[4] = {0.0,
                                   0.0,
                                   0.5*(GetDimension("outerCasingDiameter")-GetDimension("outerCasingRadialThickness")),
                                   0.5*(GetDimension("outerCasingDiameter")-GetDimension("outerCasingRadialThickness"))};

        G4double outerRadius[4] = {0.5*GetDimension("outerCasingDiameter"),
                                   0.5*GetDimension("outerCasingDiameter"),
                                   0.5*GetDimension("outerCasingDiameter"),
                                   0.5*GetDimension("outerCasingDiameter")};

        auto outerCasingSolid =
            new G4Polycone(CreateSolidName("outerCasing"),
                       phiMin, phiMax,
                       4, zSegments, innerRadius, outerRadius);

        auto outerCasingLogical =
            new G4LogicalVolume(outerCasingSolid, outerCasingMaterial, CreateLogicalName("outerCasing"));

        outerCasingLogical->SetVisAttributes(G4VisAttributes(G4Colour::Blue()));

        PlaceVolume(outerCasingLogical, GetMotherVolume(), G4ThreeVector(0, 0, 0));
    }

    // Construct and place inner casing and mylar disc
    // using G4Polycone and G4Tubs
    {
        G4double zSegments[6] = {0.0,
                                 GetDimension("innerCasingFrontThickness")+GetDimension("mylarThickness"),
                                 GetDimension("innerCasingFrontThickness")+GetDimension("mylarThickness"),
                                 GetDimension("innerCasingLength")-GetDimension("innerCasingBackThickness"),
                                 GetDimension("innerCasingLength")-GetDimension("innerCasingBackThickness"),
                                 GetDimension("innerCasingLength")};

        G4double innerRadius[6] = {0.0,
                                   0.0,
                                   0.5*GetDimension("innerCasingDiameter")-GetDimension("innerCasingRadialThickness"),
                                   0.5*GetDimension("innerCasingDiameter")-GetDimension("innerCasingRadialThickness"),
                                   0.5*GetDimension("innerCasingBackHoleDiameter"),
                                   0.5*GetDimension("innerCasingBackHoleDiameter")};

        G4double outerRadius[6] = {0.5*GetDimension("innerCasingDiameter"),
                                   0.5*GetDimension("innerCasingDiameter"),
                                   0.5*GetDimension("innerCasingDiameter"),
                                   0.5*GetDimension("innerCasingDiameter"),
                                   0.5*GetDimension("innerCasingDiameter"),
                                   0.5*GetDimension("innerCasingDiameter")};

        auto innerCasingSolid =
            new G4Polycone(CreateSolidName("innerCasing"),
                       phiMin, phiMax,
                       6, zSegments, innerRadius, outerRadius);

        auto innerCasingLogical =
            new G4LogicalVolume(innerCasingSolid, innerCasingMaterial, CreateLogicalName("innerCasing"));


        auto mylarSolid =
            new G4Tubs(CreateSolidName("mylar"),
                       0, 0.5*GetDimension("innerCasingDiameter"),
                       0.5*GetDimension("mylarThickness"), phiMin, phiMax);

        auto mylarLogical =
            new G4LogicalVolume(mylarSolid, mylarMaterial, CreateLogicalName("mylar"));

        mylarLogical->SetVisAttributes(G4VisAttributes(G4Colour::Green()));

        PlaceVolumeInternal(mylarLogical, innerCasingLogical, G4ThreeVector(0,0,0.5*GetDimension("mylarThickness")));

        PlaceVolume(innerCasingLogical, GetMotherVolume(), G4ThreeVector(0, 0, GetDimension("outerCasingFrontThickness") + GetDimension("innerDetectorPosition")));

        innerCasingLogical->SetVisAttributes(G4VisAttributes(G4Colour::Gray()));
    }

    // Construct and place crystal
    // Shape is generated by boolean solid (G4Torus + G4Tubs + G4Tubs) - G4Sphere - G4Tubs
    {
        auto fullTorusSolid =
            new G4Torus(CreateSolidName("fullTorus"),
                        0.0*cm, // inner radius
                        GetDimension("detectorRoundedEdgeRadius"), // outer radius
                        0.5*GetDimension("detectorDiameter")-GetDimension("detectorRoundedEdgeRadius"), // torus radius
                        phiMin,
                        phiMax);

        auto fullFrontCylinderSolid =
            new G4Tubs(CreateSolidName("fullFrontCylinder"),
                       0.0, // inner radius
                       0.5*GetDimension("detectorDiameter") - GetDimension("detectorRoundedEdgeRadius"), // outer radius
                       GetDimension("detectorRoundedEdgeRadius"),
                       phiMin,
                       phiMax);

        const auto fullBackCylinderHeight = GetDimension("detectorLength") - GetDimension("detectorRoundedEdgeRadius");
        auto fullBackCylinderSolid =
            new G4Tubs(CreateSolidName("fullBackCylinder"),
                       0.0,
                       0.5*GetDimension("detectorDiameter"),
                       0.5*fullBackCylinderHeight,
                       phiMin,
                       phiMax);

        const G4double tolerance = 10*mm; // make sure we cut the hole through

        auto fullBoreholeCylinderSolid =
            new G4Tubs(CreateSolidName("fullBoreholeCylinder"),
                       0.0,
                       0.5*GetDimension("detectorHoleDiameter"),
                       0.5*(GetDimension("detectorHoleDepth")-0.5*GetDimension("detectorHoleDiameter")+tolerance),
                       phiMin, phiMax);

        auto fullBoreholeSphereSolid =
            new G4Sphere(CreateSolidName("fullBoreholeSphere"),
                        0.0,
                        0.5*GetDimension("detectorHoleDiameter"),
                        phiMin, phiMax,
                        thetaMin, thetaMax);

        G4VSolid* fullDetectorSolid =
            new G4UnionSolid(CreateSolidName("detectorStep1"),
                             fullBackCylinderSolid, fullTorusSolid,
                             &noRotation,
                             G4ThreeVector(0, 0, -0.5*fullBackCylinderHeight));

        fullDetectorSolid =
            new G4UnionSolid(CreateSolidName("detectorStep2"),
                             fullDetectorSolid , fullFrontCylinderSolid,
                             &noRotation,
                             G4ThreeVector(0, 0, -0.5*fullBackCylinderHeight));

        fullDetectorSolid =
            new G4SubtractionSolid(CreateSolidName("detectorStep3"),
                                   fullDetectorSolid, fullBoreholeSphereSolid,
                                   &noRotation,
                                   G4ThreeVector(0, 0, 0.5*fullBackCylinderHeight-(GetDimension("detectorHoleDepth")-0.5*GetDimension("detectorHoleDiameter"))));

        fullDetectorSolid =
            new G4SubtractionSolid(CreateSolidName("detectorStep4"),
                                   fullDetectorSolid, fullBoreholeCylinderSolid,
                                   &noRotation,
                                   G4ThreeVector(0, 0, 0.5*fullBackCylinderHeight + fullBoreholeCylinderSolid->GetZHalfLength() - (GetDimension("detectorHoleDepth")-0.5*GetDimension("detectorHoleDiameter"))));

        auto fullDetectorLogical =
            new G4LogicalVolume(fullDetectorSolid, detectorMaterial, CreateLogicalName("detector"));

        fullDetectorLogical->SetVisAttributes(G4VisAttributes(G4Colour::Yellow()));


        // Add dead layers
        // With exception of the front dead layer, we essentially can build a smaller version of the full detector to obtain the active detector.

        auto activeTorusSolid =
            new G4Torus(CreateSolidName("activeTorus"),
                        0.0*cm, // inner radius
                        GetDimension("detectorRoundedEdgeRadius"), // outer radius
                        0.5*GetDimension("detectorDiameter")-GetDimension("detectorRoundedEdgeRadius")-GetDimension("detectorDeadLayerOutside"), // torus radius
                        phiMin,
                        phiMax);

        auto activeFrontCylinderSolid =
            new G4Tubs(CreateSolidName("activeFrontCylinder"),
                       0.0, // inner radius
                       0.5*GetDimension("detectorDiameter")-GetDimension("detectorRoundedEdgeRadius")-GetDimension("detectorDeadLayerOutside"), // outer radius
                       GetDimension("detectorRoundedEdgeRadius"),
                       phiMin,
                       phiMax);

        const auto activeBackCylinderHeight = GetDimension("detectorLength")-GetDimension("detectorRoundedEdgeRadius")-GetDimension("detectorDeadLayerBack");
        auto activeBackCylinderSolid =
            new G4Tubs(CreateSolidName("activeBackCylinder"),
                       0.0,
                       0.5*GetDimension("detectorDiameter")-GetDimension("detectorDeadLayerOutside"),
                       0.5*activeBackCylinderHeight,
                       phiMin,
                       phiMax);

        auto activeBoreholeCylinderSolid =
            new G4Tubs(CreateSolidName("activeBoreholeCylinder"),
                       0.0,
                       0.5*GetDimension("detectorHoleDiameter")+GetDimension("detectorDeadLayerInside"),
                       0.5*(GetDimension("detectorHoleDepth")-0.5*GetDimension("detectorHoleDiameter")+tolerance),
                       phiMin, phiMax);

        auto activeBoreholeSphereSolid =
            new G4Sphere(CreateSolidName("activeBoreholeSphere"),
                        0.0,
                        0.5*GetDimension("detectorHoleDiameter")+GetDimension("detectorDeadLayerInside"),
                        phiMin, phiMax,
                        thetaMin, thetaMax);

        G4VSolid* activeDetectorSolid =
            new G4UnionSolid(CreateSolidName("detectorStep1"),
                             activeBackCylinderSolid, activeTorusSolid,
                             &noRotation,
                             G4ThreeVector(0, 0, -0.5*activeBackCylinderHeight));

        activeDetectorSolid =
            new G4UnionSolid(CreateSolidName("detectorStep2"),
                             activeDetectorSolid , activeFrontCylinderSolid,
                             &noRotation,
                             G4ThreeVector(0, 0, -0.5*activeBackCylinderHeight));

        activeDetectorSolid =
            new G4SubtractionSolid(CreateSolidName("detectorStep3"),
                                   activeDetectorSolid, activeBoreholeSphereSolid,
                                   &noRotation,
                                   G4ThreeVector(0, 0, 0.5*activeBackCylinderHeight-(GetDimension("detectorHoleDepth")-0.5*GetDimension("detectorHoleDiameter"))));

        activeDetectorSolid =
            new G4SubtractionSolid(CreateSolidName("detectorStep4"),
                                   activeDetectorSolid, activeBoreholeCylinderSolid,
                                   &noRotation,
                                   G4ThreeVector(0, 0, 0.5*activeBackCylinderHeight + activeBoreholeCylinderSolid->GetZHalfLength() - (GetDimension("detectorHoleDepth")-0.5*GetDimension("detectorHoleDiameter"))));


        auto deadLayerFrontCylinderSolid =
            new G4Tubs(CreateSolidName("deadLayerFront1"), 0, 0.5*GetDimension("detectorDiameter")+tolerance, 0.5*GetDimension("detectorDeadLayerFront")+tolerance, phiMin, phiMax);
        activeDetectorSolid =
            new G4SubtractionSolid(CreateSolidName("activeDetectorStep1"),
                                    activeDetectorSolid,
                                    deadLayerFrontCylinderSolid,
                                    &noRotation,
                                    G4ThreeVector(0,0,-0.5*activeBackCylinderHeight-GetDimension("detectorRoundedEdgeRadius")-deadLayerFrontCylinderSolid->GetZHalfLength()+GetDimension("detectorDeadLayerFront")));

        auto activeDetectorLogical =
            new G4LogicalVolume(activeDetectorSolid, detectorMaterial, CreateLogicalName("crystal"));
        activeDetectorLogical->SetVisAttributes(G4VisAttributes(G4Colour::Red()));

        m_scoringVolume = activeDetectorLogical;

        PlaceVolumeInternal(activeDetectorLogical, fullDetectorLogical, G4ThreeVector(0,0,-0.5*GetDimension("detectorDeadLayerBack")));

        PlaceVolume(fullDetectorLogical, GetMotherVolume(),
                    G4ThreeVector(0, 0, 0.5*fullBackCylinderHeight+GetDimension("detectorRoundedEdgeRadius")+GetDimension("outerCasingFrontThickness") + GetDimension("innerDetectorPosition") + GetDimension("innerCasingFrontThickness") + GetDimension("mylarThickness") + GetDimension("detectorWrappingDistance")));
    }

    return nullptr;
}


