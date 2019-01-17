#include "TargetChamber.hh"

#include "globals.hh"

#include "G4NistManager.hh"
#include "G4Material.hh"
#include "G4Element.hh"

#include "G4Polycone.hh"
#include "G4Tubs.hh"
#include "G4Box.hh"
#include "G4CutTubs.hh"
#include "G4Torus.hh"
#include "G4Sphere.hh"
#include "G4VSolid.hh"

#include "G4SubtractionSolid.hh"
#include "G4IntersectionSolid.hh"
#include "G4UnionSolid.hh"

#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4PVPlacement.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4MultiFunctionalDetector.hh"
#include "G4PSEnergyDeposit.hh"

#include "G4UImessenger.hh"
#include "G4UIcmdWithAString.hh"

#include "CLHEP/Units/SystemOfUnits.h"
	using CLHEP::g;
	using CLHEP::nm;
	using CLHEP::um;
	using CLHEP::mm;
	using CLHEP::cm;
	using CLHEP::cm3;
	using CLHEP::perCent;
	using CLHEP::deg;
	using CLHEP::pi;

#include <string>
#include <stdexcept>
    using std::runtime_error;


// Target Chamber Geometry
// --------------------------
//
// The reference position (0,0,0) is the front edge of the chamber cap, just behind the target.
//

TargetChamber::TargetChamber() : GeometryObject("TargetChamber")
{
	// Chamber pipe dimensions
	RegisterDimension("chamberPipeDiameter", 6.05*cm);
	RegisterDimension("chamberPipeLength", 17.5*cm);
	RegisterDimension("chamberPipeRadialThickness", 0.25*cm);
	
	// Chamber upstream dimensions (upstream portion extends out)
	RegisterDimension("chamberUpstreamDiameter", 6.65*cm);
	RegisterDimension("chamberUpstreamLength", 4.0*cm);
	RegisterDimension("chamberUpstreamConnectorLength", 0.5*cm);
	RegisterDimension("chamberUpstreamRadialThickness", 0.35*cm);
	
	// Chamber end dimensions
	RegisterDimension("chamberEndDiameter", 9.0*cm);
	RegisterDimension("chamberEndLength", 0.25*cm);
	RegisterDimension("chamberEndRadialThickness", 1.75*cm);
	
	// Target backing dimensions
	// Union solid to get octagon shape
	RegisterDimension("backingSideLength", 3.8*cm);
	RegisterDimension("backingThickness", 0.50*mm);
	
	// Target dimensions
	RegisterDimension("targetSideX", 1.5*cm);
	RegisterDimension("targetSideY", 2.0*cm);
	RegisterDimension("targetThickness", 0.1*um); // 100 nm
	
	// Chamber cap dimensions
	RegisterDimension("chamberCapDiameter", 8.5*cm);
	RegisterDimension("chamberCapLength", 1.25*cm);
	
	// Water (cooling)
	RegisterDimension("waterCoolingDiameter", 2.75*cm);
	RegisterDimension("waterCoolingThickness", 8.30*mm);
	
	// Water (transport)
	RegisterDimension("waterTransportDiameter", 4.75*mm);
	RegisterDimension("waterTransportThickness", GetDimension("chamberCapDiameter")-1*mm); // So it doesn't extend past edge

	m_backingMaterial = "G4_Au";
	
	//m_cmdSetMaterial = new G4UIcmdWithAString("/TargetChamber/setBackingMaterial", this);
	//m_cmdSetMaterial->SetGuidance("Set new material.");
    //m_cmdSetMaterial->SetParameterName("Name of backing material (G4_Ta, for example).", false);
    //m_cmdSetMaterial->SetToBeBroadcasted(false);
}

TargetChamber::~TargetChamber(){
	//delete m_cmdSetMaterial;
}

/*
void TargetChamber::SetNewValue(G4UIcommand *cmd, G4String newValue)
{
    G4cout << "(TC) TargetChamber::SetNewValue" << G4endl;

    
    if (cmd == m_cmdSetMaterial)
    {
    	m_backingMaterial = newValue;
    	G4cout << "(TC) Received command " << m_cmdSetMaterial << "." << G4endl;
        G4cout << "(TC) Setting Backing Material to " << m_backingMaterial << "." << G4endl;
    }
    else
    {
    	G4cout << "(TC) Unknown command encountered in TC SetNewValue." << G4endl;
    	G4cout << "(TC) Unknown command is: " << m_cmdSetMaterial << "." << G4endl;
        GeometryObject::SetNewValue(cmd, newValue);
    }
}
*/

G4VPhysicalVolume* TargetChamber::Construct() {
    	
	// ----- General angular parameters for the chamber volumes
	G4RotationMatrix noRotation;
	G4RotationMatrix endRotation;
	//endRotation.rotateY(45.0*deg);
	G4RotationMatrix backingRotation;
	backingRotation.rotateZ(45.0*deg);
	
	const G4double phiMin = 0.0*deg;
	const G4double phiMax = 360.0*deg;
	
	//const G4double thetaMin = 0.0*deg;
	//const G4double thetaMax = 180.0*deg;




	// Materials
	auto nistManager = G4NistManager::Instance();
	
	//auto matLi  = nistManager->FindOrBuildMaterial("G4_Li"); // Lithium contact
	auto matCu = nistManager->FindOrBuildMaterial("G4_Cu");
	auto matZn = nistManager->FindOrBuildMaterial("G4_Zn");
	auto matBrass = new G4Material("brass", 8.73*g/cm3, 2);
	matBrass->AddMaterial(matCu, 0.63);
	matBrass->AddMaterial(matZn, 0.37);
	auto matTarget = nistManager->FindOrBuildMaterial("G4_N");

	//Specify backing material
	//auto matBacking;
	if (m_backingMaterial == "")
	{
		G4cerr << "(TC) No backing material has been specified." << G4endl;
	}

	auto matBacking = nistManager->FindOrBuildMaterial(m_backingMaterial);
	G4cout << "(TC) Set backing material to " << m_backingMaterial << "." << G4endl;

	//auto matAir = nistManager->FindOrBuildMaterial("G4_AIR");
	//auto matVacuum = new G4Material("vacuum", 1.56e-13*g/cm3, 1);
	//matVacuum->AddMaterial(matAir, 1.0);
	auto matWater = nistManager->FindOrBuildMaterial("G4_WATER");
	
	// Target chamber is made of brass, as is the cap piece
	const auto chamberMaterial = matBrass;
	
	// Target backing is a thick user defined metal
	const auto backingMaterial = matBacking;
	
	// Target material is carbon foil
	const auto targetMaterial = matTarget;
	
	// Cooling water and its transport line is made from water
	const auto waterMaterial = matWater;
	
	

	// For visualization of materials
	G4VisAttributes* brassVisAtt = new G4VisAttributes(G4Colour(0.71,0.651,0.259)); 
	G4VisAttributes* targetVisAtt = new G4VisAttributes(G4Colour(0.01, 0.99, 0.01));
	G4VisAttributes* backingVisAtt = new G4VisAttributes(G4Colour(0.76,0.79,0.81));
	G4VisAttributes* waterVisAtt = new G4VisAttributes(G4Colour(0.01,0.467,0.745)); 
	//G4VisAttributes* vacuumVisAtt = new G4VisAttributes(G4Colour(0.49,0.56,0.98)); 
	
	// Construct and place end cap with cooling water
	// using G4Tubs
	{
		// Chamber Cap
		auto chamberCapSolid = new G4Tubs(CreateSolidName("chamberCap"),
										  0, 0.5*GetDimension("chamberCapDiameter"),	// inner and outer radius
										  0.5*GetDimension("chamberCapLength"), 		// half length
										  phiMin, phiMax);

		auto chamberCapLogical = new G4LogicalVolume(chamberCapSolid, chamberMaterial, CreateLogicalName("chamberCap"));

		chamberCapLogical->SetVisAttributes(brassVisAtt);

		// Cooling water and transport
		auto waterCoolingSolid = new G4Tubs(CreateSolidName("waterCooling"),
											0, 0.5*GetDimension("waterCoolingDiameter"),
											0.5*GetDimension("waterCoolingThickness"),
											phiMin, phiMax);

		auto waterTransportSolid = new G4Tubs(CreateSolidName("waterTransport"),
											0, 0.5*GetDimension("waterTransportDiameter"),
											0.5*GetDimension("waterTransportThickness"),
											phiMin, phiMax);

		// Combine water
		G4RotationMatrix transportRotation;
		transportRotation.rotateY(90.0*deg);
		G4Transform3D transformWater(transportRotation, G4ThreeVector());
		G4VSolid *waterSolid = new G4UnionSolid(CreateSolidName("CoolingWaterAndTransport"),
										   		waterCoolingSolid,
										   		waterTransportSolid,
										   		transformWater);

		auto waterLogical = new G4LogicalVolume(waterSolid, waterMaterial, CreateLogicalName("water"));
		waterLogical->SetVisAttributes(waterVisAtt);


		// Place
		PlaceVolumeInternal(waterLogical, chamberCapLogical, G4ThreeVector(0, 0, -0.5*(GetDimension("chamberCapLength") - GetDimension("waterCoolingThickness"))));

		PlaceVolume(chamberCapLogical, GetMotherVolume(), G4ThreeVector(0, 0, -(0.5*GetDimension("chamberCapLength"))), noRotation);
	}

	
	//Construct and place the chamber pipe
	// this includes the vacuum and target
	// using G4CutTubs
	{
		auto chamberEndSolid = new G4Tubs(CreateSolidName("chamberEnd"),
										  0.5*GetDimension("chamberEndDiameter") - GetDimension("chamberEndRadialThickness"),
										  0.5*GetDimension("chamberEndDiameter"),
										  0.5*GetDimension("chamberEndLength"),
										  phiMin, phiMax);

		// Target end of chamber
		auto chamberPipeSolid = new G4Tubs(CreateSolidName("chamberPipe"),
										   0.5*GetDimension("chamberPipeDiameter") - GetDimension("chamberPipeRadialThickness"),
										   0.5*GetDimension("chamberPipeDiameter"),
										   0.5*GetDimension("chamberPipeLength"),
										   phiMin, phiMax);

		G4Transform3D transformChamberPipeAndEnd(noRotation, G4ThreeVector(0, 0, 0.5*(GetDimension("chamberEndLength") + GetDimension("chamberPipeLength"))));
		
		G4VSolid *chamberPipeAndEndSolid = new G4UnionSolid(CreateSolidName("PipeAndEnd"),
															chamberPipeSolid,
															chamberEndSolid,
															transformChamberPipeAndEnd);

		// Upstream connecting piece to make another union solid
		G4double zSegments[3] = {0.0,
								-GetDimension("chamberUpstreamConnectorLength"),
								-(GetDimension("chamberUpstreamConnectorLength") + GetDimension("chamberUpstreamLength"))};
		
		G4double innerRadius[3] = {0.5*GetDimension("chamberPipeDiameter")-GetDimension("chamberPipeRadialThickness"),
								   0.5*GetDimension("chamberUpstreamDiameter")-GetDimension("chamberUpstreamRadialThickness"),
								   0.5*GetDimension("chamberUpstreamDiameter")-GetDimension("chamberUpstreamRadialThickness")};
		
		G4double outerRadius[3] = {0.5*GetDimension("chamberPipeDiameter"),
								   0.5*GetDimension("chamberUpstreamDiameter"),
								   0.5*GetDimension("chamberUpstreamDiameter")};
		
		auto chamberUpstream = new G4Polycone(CreateSolidName("chamberUpstream"),
											   phiMin, phiMax,
											   3, zSegments, innerRadius, outerRadius);


		G4Transform3D transformChamberUpstream(G4RotationMatrix(), G4ThreeVector(0, 0, -(0.5*GetDimension("chamberPipeLength"))));

		G4VSolid *chamberSolid = new G4UnionSolid(CreateSolidName("Chamber"),
												  chamberPipeAndEndSolid,
												  chamberUpstream,
												  transformChamberUpstream);

		auto chamberLogical = new G4LogicalVolume(chamberSolid, chamberMaterial, CreateLogicalName("chamber"));

		chamberLogical->SetVisAttributes(brassVisAtt);

		PlaceVolume(chamberLogical, GetMotherVolume(), G4ThreeVector(0, 0, -(GetDimension("chamberCapLength") + GetDimension("chamberEndLength") + 0.5*GetDimension("chamberPipeLength"))), noRotation);

	/*
		auto vacuumSolid1 = new G4CutTubs(CreateSolidName("vacuum1"),
										 0, (0.5*GetDimension("chamberPipeDiameter")-GetDimension("chamberPipeRadialThickness")),
										 0.5*GetDimension("chamberPipeLength")+GetDimension("chamberEndLength")*std::sqrt(2), 
										 phiMin, phiMax,
										 G4ThreeVector(),
										 G4ThreeVector(1, 0, 1));
		*/

		// Create vacuum with inside pipe and upstream piece
		// To ensure no overlap, do the intersection with chamber
		// and then subtract that intersection
		// Upstream connecting piece to make another union solid for vacuum
		/*
		G4double zSegmentsVac[3] = {0.0,
								-GetDimension("chamberUpstreamConnectorLength"),
								-(GetDimension("chamberUpstreamConnectorLength") + GetDimension("chamberUpstreamLength"))};
		
		G4double innerRadiusVac[3] = {0,
								   0,
								   0};
		
		G4double outerRadiusVac[3] = {0.5*GetDimension("chamberPipeDiameter")-GetDimension("chamberPipeRadialThickness"),
								   0.5*GetDimension("chamberUpstreamDiameter")-GetDimension("chamberUpstreamRadialThickness"),
								   0.5*GetDimension("chamberUpstreamDiameter")-GetDimension("chamberUpstreamRadialThickness")};

		auto vacuumSolid2 = new G4Polycone(CreateSolidName("vacuum2"),
											   phiMin, phiMax,
											   3, zSegmentsVac, innerRadiusVac, outerRadiusVac);

		G4Transform3D transformChamberVacuum(endRotation, G4ThreeVector());
		G4Transform3D transformVacuum(G4RotationMatrix(), G4ThreeVector(0, 0, -(0.5*GetDimension("chamberPipeLength"))));
		G4VSolid *vacuumUnionSolid = new G4UnionSolid(CreateSolidName("VacuumUnion"),
												  vacuumSolid1,
												  vacuumSolid2,
												  transformVacuum);

		G4VSolid *vacuumIntSolid = new G4IntersectionSolid(CreateSolidName("vacuumInt"),
														   vacuumUnionSolid,
														   chamberSolid,
														   transformChamberVacuum);

		G4VSolid *vacuumSolid = new G4SubtractionSolid(CreateSolidName("vacuum"),
													   vacuumUnionSolid,
													   vacuumIntSolid);

		auto vacuumLogical = new G4LogicalVolume(vacuumSolid, vacuumMaterial, CreateLogicalName("Vacuum"));

		vacuumLogical->SetVisAttributes(vacuumVisAtt);
	*/

		auto backingSolid1 = new G4Box(CreateSolidName("backing1"),
									   GetDimension("backingSideLength")*0.5,
									   GetDimension("backingSideLength")*0.5,
									   GetDimension("backingThickness")*0.5);

		auto backingSolid2 = new G4Box(CreateSolidName("backing2"),
									   GetDimension("backingSideLength")*0.5,
									   GetDimension("backingSideLength")*0.5,
									   GetDimension("backingThickness")*0.5);

		G4Transform3D transformBackingIntersection(backingRotation, G4ThreeVector());
		G4VSolid *backingSolid = new G4IntersectionSolid(CreateSolidName("backingSolid"),
														 backingSolid1,
														 backingSolid2,
														 transformBackingIntersection);

		auto backingLogical = new G4LogicalVolume(backingSolid, backingMaterial, CreateLogicalName("backing"));

		backingLogical->SetVisAttributes(backingVisAtt);

		//PlaceVolumeInternal(backingLogical, vacuumLogical, G4ThreeVector(0, 0, 0.5*GetDimension("chamberPipeLength")+std::sqrt(2)*(GetDimension("chamberEndLength")-0.5*GetDimension("backingThickness"))), endRotation);
		PlaceVolume(backingLogical, GetMotherVolume(), G4ThreeVector(0, 0, -(GetDimension("chamberCapLength")+0.5*GetDimension("backingThickness"))), endRotation);

		auto targetSolid = new G4Box(CreateSolidName("target"),
									 0.5*GetDimension("targetSideX"),
									 0.5*GetDimension("targetSideY"),
									 0.5*GetDimension("targetThickness"));

		auto targetLogical = new G4LogicalVolume(targetSolid, targetMaterial, CreateLogicalName("target"));

		targetLogical->SetVisAttributes(targetVisAtt);

		//PlaceVolumeInternal(targetLogical, vacuumLogical, G4ThreeVector(0, 0, 0.5*GetDimension("chamberPipeLength")+std::sqrt(2)*(GetDimension("chamberEndLength")-GetDimension("backingThickness")-0.5*GetDimension("targetThickness"))), endRotation);

		G4ThreeVector targetPosition = G4ThreeVector(0, 0, -((GetDimension("chamberCapLength")+GetDimension("backingThickness"))+0.5*GetDimension("targetThickness")));

		m_targetSurfaceNormal = G4ThreeVector(0, 0, 1);
		m_targetSurfaceNormal = (*GetRotation())*(endRotation*m_targetSurfaceNormal);

		G4cout << "(TN) Target normal vector: " << G4endl;
		G4cout << "(TN) " << m_targetSurfaceNormal.x() << G4endl;
		G4cout << "(TN) " << m_targetSurfaceNormal.y() << G4endl;
		G4cout << "(TN) " << m_targetSurfaceNormal.z() << G4endl;
		G4cout << G4endl;
		

		m_targetSurfacePosition = (*GetRotation())*targetPosition + m_targetSurfaceNormal*0.5*(GetDimension("targetThickness")+1*nm);

		PlaceVolume(targetLogical, GetMotherVolume(), targetPosition, endRotation);

		//PlaceVolume(vacuumLogical, GetMotherVolume(), G4ThreeVector(-(GetDimension("chamberCapLength")/std::sqrt(2)), 0, -(GetDimension("chamberCapLength")/std::sqrt(2) + GetDimension("chamberEndLength")*std::sqrt(2) + 0.5*GetDimension("chamberPipeLength"))), noRotation);

	}


	// Could do vacuum as find intersection of vacuum with chamber?
	// then take that intersection as a piece to subtract from vacuum. 
	// Place vacuum inside of chamber -> Boom done with vacuum -> Later;


	return nullptr;
}




