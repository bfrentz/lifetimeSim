#include "EnergyHistogram.hh"

#include "globals.hh"

#include "G4SystemOfUnits.hh"
using CLHEP::keV;

EnergyHistogram::EnergyHistogram(const int nBins, const double Emin, const double Emax) : m_nBins(nBins), m_Emin(Emin), m_Emax(Emax)
{
    m_histogram = new double[m_nBins+2];
    Reset();
}

EnergyHistogram::~EnergyHistogram()
{
    delete[] m_histogram;
}


void EnergyHistogram::Reset()
{
    G4AutoLock lock(&m_mutex);
    m_histogram = new double[m_nBins+2];
    for (int i = 0; i <= m_nBins+1; i++)
    {
        m_histogram[i] = 0;
    }
}

void EnergyHistogram::Fill(const double energy)
{
    G4AutoLock lock(&m_mutex);
    if (energy < m_Emin)
    {
        m_histogram[0] += 1;
    }
    else if (energy > m_Emax)
    {
        m_histogram[m_nBins+1] += 1;
    }
    else
    {
        m_histogram[int(m_nBins*(energy-m_Emin)/(m_Emax-m_Emin)) + 1] += 1;
    }

    //if (energy != 0)
    //    G4cout << energy / keV << "keV - bin " << int(m_nBins*(energy-m_Emin)/(m_Emax-m_Emin)) + 1 << G4endl;
}

void EnergyHistogram::Write(const string fileName) const
{
    ofstream fout(fileName);
    fout << m_nBins << "\t" << m_Emin/keV << "\t" << m_Emax/keV << "\n";
    for (int i = 0; i <= m_nBins+1; i++)
    {
        fout << m_histogram[i] << "\n";
    }
}
