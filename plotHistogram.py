import ROOT
import sys

ROOT.gROOT.SetBatch(1)

lifetime = sys.argv[1]
energy = sys.argv[2]
backing = sys.argv[3]
plotName = "./output/histograms_" + lifetime + "_" + energy + "_" + backing 

canvas = ROOT.TCanvas()
canvas.Print(plotName + ".pdf[")

fileName = plotName + ".root"
outFile = ROOT.TFile(fileName, "RECREATE")

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
	histo.GetXaxis().SetRangeUser(6400, 7200)

	ROOT.gPad.SetLogy(1)
	canvas.Print(plotName + ".pdf")

canvas.Print(plotName + ".pdf]")


