# Changing the beam energy of the simulation in run.mac through this command line script
# Program goes to a specific line in the run.mac text file and changes the whole line's string.
# 
# If the user adds or deletes lines from run.mac the user will need to alter the index for where the line is changed
#
# Called by running:
# python3 energyChange.py ENERGY UNIT
# where ENERGY is the energy value
# and UNIT is the energy unit (keV, for example)

import sys


energy = sys.argv[1]
unit = sys.argv[2]

change = '/gun/energy ' + energy + ' ' + unit + '\n'

with open("run.mac") as f:
	lines = f.readlines()

tmp = lines[34]

if (len(change) > len(tmp)):
	for i in range(0, len(change)-len(tmp)):
		lines[34] = lines[34] + ' '

lines[34] = change

#print(change)

with open("run.mac", "w") as f:
	f.writelines(lines)