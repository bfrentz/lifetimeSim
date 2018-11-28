#ifndef DetectorConstruction_hh
#define DetectorConstruction_hh

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

#include "HPGeDetector.hh"
#include "TargetChamber.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;


class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
    DetectorConstruction();
    virtual ~DetectorConstruction();

    virtual G4VPhysicalVolume* Construct();

    G4LogicalVolume* GetScoringVolume(size_t iHPGe) const
    {
        return m_scoringVolumes[iHPGe];
    }

    size_t GetMaximumNumberOfHPGe() const {return m_nHPGe_max;}

protected:
    const size_t m_nHPGe_max = 5;
    HPGeDetector **m_hpgeDetector = nullptr;
    TargetChamber *m_targetChamber = nullptr;
    G4LogicalVolume** m_scoringVolumes = nullptr;
};

#endif // #ifndef DetectorConstruction_hh

