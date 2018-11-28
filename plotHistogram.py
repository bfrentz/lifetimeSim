import ROOT

ROOT.gROOT.SetBatch(1)

plotName = "histogram.pdf"

canvas = ROOT.TCanvas()
canvas.Print(plotName + "[", "pdf")

outFile = ROOT.TFile("histograms.root", "RECREATE")

for iHPGe in range(0, 5):
	with open("histogram_{0}.dat".format(iHPGe)) as inFile:
	    line = next(inFile)
	    nBins, Emin, Emax = line.split()
	    histoName = "histo{0}".format(iHPGe)
	    histo = ROOT.TH1D(histoName, histoName, int(nBins), float(Emin), float(Emax))
	    for i in range(0, int(nBins)+2):
	        histo.SetBinContent(i, float(next(inFile)))
	    histo.SetBinContent(1, 0) # remove 0 energy depositions

	histo.Write()

	#histoSmeared = histo.Clone("histoSmeared")
	#histoSmeared.Reset()

	#histo.Rebin(10)
	histo.Draw()
	histo.GetXaxis().SetRangeUser(6500, 7000)

	ROOT.gPad.SetLogy(1)
	canvas.Print(plotName, "pdf")

canvas.Print(plotName + "]", "pdf")


