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

#include <map>
    using std::map;

class TargetChamber : public GeometryObject
{
    public:
        TargetChamber();
        ~TargetChamber();

        G4VPhysicalVolume *Construct();
        void ConstructSDandField() {}

    private:
};

#endif // HPGeDetector_hh