#!/usr/bin/python3

# written by Doug Fraker 2018
# this program takes csv output from Tiled
# and turns it into a C style array that can be included
# This version is for multi-room csv files and it breaks
# it into 16 wide rooms (file should be a multiple of 16 wide)
# NES programming...cc65


import sys
import csv
import os
import re


filename = sys.argv[1]
newname = filename[0:-4] + ".c"
newname2 = os.path.basename(filename)
newname2 = newname2[0:-4]

# Extract world and level numbers if they exist in the format w1l1
match = re.match(r'w(\d+)l(\d+)_(\w+)', newname2)
if match:
    world_num = match.group(1)
    level_num = match.group(2)
    layer_type = match.group(3)
    # Create a more descriptive variable name
    newname2 = f"w{world_num}l{level_num}_{layer_type}"
else:
    # If the filename doesn't match the pattern, use the original name
    pass

with open(filename, 'r') as oldfile:
  reader = csv.reader(oldfile)
  your_list = list(reader)

newfile = open(newname, 'w')  # warning, this may overwrite old file !!!!!!!!!!!!!!!!!!!!!


rows = len(your_list)
columns = len(your_list[0])

newfile.write("// Y, room, X, object #\n\n")

for i in range(0, columns):
	for j in range(0, rows):
		a = str(your_list[j][i])
		if (a != "-1"):
			newfile.write(str(hex(j)) + "0, ")
			k = i // 16
			newfile.write(str(k) + ", ")
			k = i % 16
			newfile.write(str(hex(k)) + "0, ")
			newfile.write("obj" + a + ", \n")

newfile.write("TURN_OFF")
newfile.write("\n\n")			

	
print("Done.")
oldfile.close
newfile.close