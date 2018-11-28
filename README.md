# lifetimeSim
Geant4 Simulation of the 14N(p,g) lifetime measurement experiment

This project is the source code for the Geant4 Simulation. 
In order to run, must create a build directory and make the project. 
This is done outside the scope of the github repository. 

v0.0.1: Initial project contains only the HPGe detector and target chamber. 

v1.0: First stable working simulation including support for multiple detectors and the ability to adjust the 15O lifetime to monitor its impact.

Each of the detectors are created and placed within the runtime macros: vis.mac and run.mac.
Similarly, the relevant physics is changed in PrimaryGeneratorAction.cc. In this file a user can change the lifetime, beam energy, direction, momentum, etc.

The simulation outputs the array of histograms into five separate text files, entitled histogram_0.dat, histogram_1.dat...
The python script plotHistogram.py (which was written for python3) converts these .dat files into a single root file containing all five histograms and additionally prints a .pdf file of each histogram over the range that the user specifies.  
