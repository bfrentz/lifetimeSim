# Writes out Geant4 simulation data to root file and makes initial histogram plot of a user defined region.
# Uses .dat files generated for each detector by the lifetime executable, so this must be run
# after each different Geant4 simulation or data will be lost. Similarly, a user cannot have multiple instances of 
# the lifetime executable running in the same directory, since the files will interfere.
#
# script is invoked with three additional arguments, lifetime, energy, and backing
#
# User should include units as one word in these arguments when passed at the command line. For example,
# py plotHistogram.py 1fs 100keV Ta

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


