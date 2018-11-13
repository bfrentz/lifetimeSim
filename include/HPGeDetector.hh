#ifndef HPGeDetector_hh
#define HPGeDetector_hh

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

class HPGeDetector : public GeometryObject
{
    public:
        HPGeDetector();
        ~HPGeDetector();

        G4VPhysicalVolume *Construct();
        void ConstructSDandField() {}

        G4LogicalVolume *GetScoringVolume() {return m_scoringVolume;}

    private:
        G4LogicalVolume *m_scoringVolume = nullptr;
};

#endif // HPGeDetector_hh
