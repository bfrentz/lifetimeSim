#ifndef GeometryObject_hh
#define GeometryObject_hh

#include "G4VUserDetectorConstruction.hh"
#include "G4UImessenger.hh"

#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4Transform3D.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UIcmdWithAString.hh"

#include <map>
    using std::map;
#include <set>
    using std::set;
#include <stdexcept>
    using std::runtime_error;

class G4PVPlacement;

class GeometryObject : public G4VUserDetectorConstruction, public G4UImessenger {

    public:
        GeometryObject(G4String name);
        ~GeometryObject();

        G4String GetName() {return m_name;}

        virtual void Build();
        virtual G4VPhysicalVolume *Construct() = 0;
        virtual void ConstructSDandField() = 0;

        void SetMotherVolume(G4LogicalVolume *volume) {
            m_motherVolume = volume; }
        G4LogicalVolume* GetMotherVolume() {return m_motherVolume;}


    protected:
        G4ThreeVector GetPosition() {return m_position;}
        G4RotationMatrix *GetRotation() {return m_rotation;}

        G4Transform3D GetTransform3D(G4RotationMatrix ownRotation, G4ThreeVector relativePosition);

        const char *CreateSolidName(G4String inName) {
            return (GetName() + "_" + inName + "_solid").c_str();
        }

        const char *CreateLogicalName(G4String inName) {
            return (GetName() + "_" + inName + "_logical").c_str();
        }

        const char *CreatePhysicalName(G4String inName) {
            return (GetName() + "_" + inName + "_physical").c_str();
        }

        G4PVPlacement *PlaceVolume(G4LogicalVolume *logicalVolume, G4LogicalVolume *motherVolume = NULL, G4ThreeVector position = G4ThreeVector(), G4RotationMatrix rotation = G4RotationMatrix(), int copyNr = 0, bool noExternalRotation = false);
        G4PVPlacement *PlaceVolumeInternal(G4LogicalVolume *logicalVolume, G4LogicalVolume *motherVolume = nullptr, G4ThreeVector position = G4ThreeVector(), G4RotationMatrix rotation = G4RotationMatrix(), int copyNr = 0);

        virtual void SetNewValue(G4UIcommand* command, G4String value);

        void RegisterDimension(G4String name, G4double defaultValue);
        G4double GetDimension(G4String name);
        void SetDimension(G4String name, G4double value);

        void CheckForUnusedDimensions();

    private:
        G4String m_name;
        G4bool m_enable = false;

        G4LogicalVolume *m_motherVolume = nullptr;

        G4ThreeVector m_position;
        G4RotationMatrix *m_rotation;

        G4String m_cmdDirName;
        G4UIdirectory* m_cmdDir;

        G4UIcmdWithABool* m_cmdEnable;
        G4UIcmdWith3VectorAndUnit* m_cmdSetPosition;
        G4UIcmdWithADoubleAndUnit* m_cmdRotateX;
        G4UIcmdWithADoubleAndUnit* m_cmdRotateY;
        G4UIcmdWithADoubleAndUnit* m_cmdRotateZ;

        map<G4String, G4double> m_dimensions;
        set<G4String> m_unusedDimensions;
        G4UIcmdWithAString *m_cmdSetDimension;

        //G4UIcmdWithAString *m_cmdSetMaterial;
};

#endif
