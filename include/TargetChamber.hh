#ifndef TargetChamber_hh
#define TargetChamber_hh

#include "GeometryObject.hh"

class G4MultiFunctionalDetector;
class G4VPrimitiveScorer;

#include "CLHEP/Units/SystemOfUnits.h"
    using CLHEP::um;
    using CLHEP::mm;

#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAString.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"

#include <map>
    using std::map;


class TargetChamber : public GeometryObject
{
    public:
        TargetChamber();
        ~TargetChamber();

        G4VPhysicalVolume *Construct();
        void ConstructSDandField() {}

        G4ThreeVector GetTargetSurfacePosition() const {return m_targetSurfacePosition;}
        G4ThreeVector GetTargetSurfaceNormal() const {return m_targetSurfaceNormal;}

        //virtual void SetNewValue(G4UIcommand* command, G4String value);

    private:

    	G4ThreeVector m_targetSurfacePosition;
    	G4ThreeVector m_targetSurfaceNormal;
        G4String m_backingMaterial = "";
        //G4UIcmdWithAString *m_cmdSetMaterial;
};

#endif // HPGeDetector_hh