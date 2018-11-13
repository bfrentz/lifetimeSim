#ifndef EnergyHistogram_hh
#define EnergyHistogram_hh

#include "G4AutoLock.hh"

#include <string>
using std::string;
#include <fstream>
using std::ofstream;

class EnergyHistogram
{
public:
    EnergyHistogram(const int nBins, const double Emin, const double Emax);
    ~EnergyHistogram();

    void Reset();
    void Fill(const double value);

    int GetNbins() const
    {
        return m_nBins;
    }

    double GetEmin() const
    {
        return m_Emin;
    }

    double GetEmax() const
    {
        return m_Emax;
    }

    void Write(const string fileName) const;

private:
    const int m_nBins;
    const double m_Emin, m_Emax;
    double *m_histogram;
    G4Mutex m_mutex = G4MUTEX_INITIALIZER;
};

#endif // EnergyHistogram_hh
