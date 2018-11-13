import ROOT

with open("histogram.dat") as inFile:
    line = inFile.next()
    nBins, Emin, Emax = line.split()
    histo = ROOT.TH1D("histo", "histo", int(nBins), float(Emin), float(Emax))
    for i in xrange(0, int(nBins)+2):
        histo.SetBinContent(i, float(inFile.next()))
    histo.SetBinContent(1, 0) # remove 0 energy depositions

outFile = ROOT.TFile("histogram.root", "RECREATE")
histo.Write()

histoSmeared = histo.Clone("histoSmeared")
histoSmeared.Reset()

#histo.Rebin(10)
histo.Draw()
#histo.GetXaxis().SetRangeUser(8000, 14000)

ROOT.gPad.SetLogy(1)
ROOT.gPad.SaveAs("./histogram.pdf")


