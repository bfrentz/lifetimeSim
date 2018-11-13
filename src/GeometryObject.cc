#include "GeometryObject.hh"

#include "G4NistManager.hh"
#include "G4Material.hh"
#include "G4Element.hh"

#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4Tubs.hh"
#include "G4Trd.hh"

#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4VPhysicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"

GeometryObject::GeometryObject(G4String name) :
    m_name(name),
    m_position(0, 0, 0),
    m_rotation(new G4RotationMatrix()),
    m_cmdDirName("/Geometry/" + name + "/"),
    m_cmdDir(new G4UIdirectory(m_cmdDirName)),
    m_cmdEnable(new G4UIcmdWithABool((m_cmdDirName + "enable").c_str(), static_cast<G4UImessenger*>(this))),
    m_cmdSetPosition(new G4UIcmdWith3VectorAndUnit((m_cmdDirName + "position").c_str(), static_cast<G4UImessenger*>(this))),
    m_cmdRotateX(new G4UIcmdWithADoubleAndUnit((m_cmdDirName + "rotateX").c_str(), static_cast<G4UImessenger*>(this))),
    m_cmdRotateY(new G4UIcmdWithADoubleAndUnit((m_cmdDirName + "rotateY").c_str(), static_cast<G4UImessenger*>(this))),
    m_cmdRotateZ(new G4UIcmdWithADoubleAndUnit((m_cmdDirName + "rotateZ").c_str(), static_cast<G4UImessenger*>(this))),
    m_cmdSetDimension(new G4UIcmdWithAString((m_cmdDirName + "setDimension").c_str(), static_cast<G4UImessenger*>(this)))
{
    m_cmdDir->SetGuidance(("commands for " + m_name + " geometry.").c_str());

    m_cmdEnable->SetGuidance(("Enable building " + m_name + " .").c_str());
    m_cmdEnable->SetParameterName("enable", true);
    m_cmdEnable->SetDefaultValue(true);

    m_cmdSetPosition->SetGuidance(("Position of " + m_name + " .").c_str());
    m_cmdSetPosition->SetParameterName("X", "Y", "Z", false);
    m_cmdSetPosition->SetUnitCategory("Length");

    m_cmdRotateX->SetGuidance(("Rotate " + m_name + " around x-axis.").c_str());
    m_cmdRotateX->SetParameterName("angĺeX", false);
    m_cmdRotateX->SetUnitCategory("Angle");

    m_cmdRotateY->SetGuidance(("Rotate " + m_name + " around y-axis.").c_str());
    m_cmdRotateY->SetParameterName("angĺeY", false);
    m_cmdRotateY->SetUnitCategory("Angle");

    m_cmdRotateZ->SetGuidance(("Rotate " + m_name + " around z-axis.").c_str());
    m_cmdRotateZ->SetParameterName("angĺeZ", false);
    m_cmdRotateZ->SetUnitCategory("Angle");

    m_cmdSetDimension->SetGuidance("Set dimension, use e.g. \"/Geometry/TargetChamber55/setDimension beamSpotDiameter 0.5 mm\".");
    m_cmdSetDimension->SetParameterName("Name of dimension, number and unit - separated by spaces.", false);
    m_cmdSetDimension->SetToBeBroadcasted(false);
}

GeometryObject::~GeometryObject() {
    delete m_rotation;
    delete m_cmdDir;
    delete m_cmdSetPosition;
    delete m_cmdRotateX;
    delete m_cmdRotateY;
    delete m_cmdRotateZ;
}

void GeometryObject::Build() {
    if (m_enable) {
        Construct();
        ConstructSDandField();
        CheckForUnusedDimensions();
    }
}

G4Transform3D GeometryObject::GetTransform3D(G4RotationMatrix ownRotation, G4ThreeVector relativePosition) {
    return G4Transform3D(((*GetRotation())*ownRotation), GetPosition() + (*GetRotation())(relativePosition));
}

G4PVPlacement* GeometryObject::PlaceVolume(G4LogicalVolume *logicalVolume, G4LogicalVolume *motherVolume, G4ThreeVector position, G4RotationMatrix rotation, int copyNr, bool noExternalRotation) {
    auto tmpStart = logicalVolume->GetName().find_first_of('_');
    auto tmpEnd = logicalVolume->GetName().find_last_of('_');

    auto baseName = (logicalVolume->GetName()).substr(tmpStart + 1, tmpEnd-tmpStart-1);
    //G4cout << "baseName: " << baseName << G4endl;

    if (motherVolume == nullptr)
        motherVolume = GetMotherVolume();

    G4PVPlacement *phyVol = nullptr;

    if (noExternalRotation) {
    phyVol =
        new G4PVPlacement(G4Transform3D(rotation, position),
                          logicalVolume, CreatePhysicalName(baseName),
                          motherVolume, false, copyNr, false);
    }
    else {
    phyVol =
        new G4PVPlacement(GetTransform3D(rotation, position),
                          logicalVolume, CreatePhysicalName(baseName),
                          motherVolume, false, copyNr, false);
    }

    // check for overlaps
    if (phyVol->CheckOverlaps()) {
        G4cerr << "Overlaps found!" << G4endl;
        throw runtime_error("Overlapping volumes!");
    }

    return phyVol;
}

G4PVPlacement* GeometryObject::PlaceVolumeInternal(G4LogicalVolume *logicalVolume, G4LogicalVolume *motherVolume, G4ThreeVector position, G4RotationMatrix rotation, int copyNr) {
    return PlaceVolume(logicalVolume, motherVolume, position, rotation, copyNr, true);
}


void GeometryObject::SetNewValue(G4UIcommand* command, G4String newValue)
{
    if (command == m_cmdEnable) {
        m_enable =  static_cast<G4UIcmdWithABool*>(command)->GetNewBoolValue(newValue);
    }
    else if (command == m_cmdSetPosition) {
        m_position = static_cast<G4UIcmdWith3VectorAndUnit*>(command)->GetNew3VectorValue(newValue);}
    else if (command == m_cmdRotateX){
        m_rotation->rotateX(static_cast<G4UIcmdWithADoubleAndUnit*>(command)->GetNewDoubleValue(newValue));}
    else if (command == m_cmdRotateY){
        m_rotation->rotateY(static_cast<G4UIcmdWithADoubleAndUnit*>(command)->GetNewDoubleValue(newValue));}
    else if (command == m_cmdRotateZ){
        m_rotation->rotateZ(static_cast<G4UIcmdWithADoubleAndUnit*>(command)->GetNewDoubleValue(newValue));}
    else if (command == m_cmdSetDimension){
        const G4String dimensionName = newValue.substr(0, newValue.find(" "));
        const G4double dimensionValue = G4UIcmdWithADoubleAndUnit::GetNewDoubleValue(G4String(newValue.substr(newValue.find(" "), newValue.size()-newValue.find(" "))));
        SetDimension(dimensionName, dimensionValue);
    }
    else
        G4cerr << "Unknown command in GeometryObject!" << G4endl;
}


void GeometryObject::RegisterDimension(G4String name, G4double defaultValue) {
    if (m_dimensions.find(name) != m_dimensions.end()) {
        G4String errorMessage = "Tried to register dimension '" + name + "' twice.";
        throw runtime_error(errorMessage.c_str());
    }
    m_dimensions[name] = defaultValue;
    m_unusedDimensions.insert(name);
}

G4double GeometryObject::GetDimension(G4String name) {
    if (m_dimensions.find(name) == m_dimensions.end()) {
        G4String errorMessage = "Did not find dimension '" + name + "' in GetDimension.";
        throw runtime_error(errorMessage.c_str());
    }

    auto it = m_unusedDimensions.find(name);
    if (it != m_unusedDimensions.end()) {
        m_unusedDimensions.erase(it);
    }

    return m_dimensions[name];
}

void GeometryObject::SetDimension(G4String name, G4double value) {
    if (m_dimensions.find(name) == m_dimensions.end()) {
        G4String errorMessage = "Did not find dimension '" + name + "' in SetDimension.";
        throw runtime_error(errorMessage.c_str());
    }

    m_dimensions[name] = value;
}

void GeometryObject::CheckForUnusedDimensions() {
    if (m_unusedDimensions.size() != 0) {
        G4cerr << "Found unused dimensions in geometry " << m_name << ":" << G4endl;
        for (const auto x : m_unusedDimensions) {
            G4cerr << " - " << x << G4endl;
        }
        throw runtime_error("Unused dimensions.");
    }
}
