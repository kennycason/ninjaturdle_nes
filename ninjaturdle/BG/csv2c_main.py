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

if(columns < 16):
	print("Error. File too small. Use CSV2C.py")
	exit()
if((columns & 0x0f) != 0):
	print("Warning, expected width divisible by 16.")

loops = columns // 16 # force int division
a = 0

for h in range(0, loops):
	newfile.write("const unsigned char " + newname2 + "_" + str(h) + "[]={\n")
	for i in range(0, rows):
		for j in range(0, 16):
			a = j + (h * 16)
			if i == rows-1 and j == 15:  # Last item in the array
				newfile.write(your_list[i][a])  # No comma for last item
			else:
				newfile.write(your_list[i][a] + ",")
		newfile.write("\n")	
	newfile.write("};\n\n")
	

newfile.write("\nconst unsigned char * const " + newname2 + "_list[]={\n")	
for h in range(0, loops):
	if(h == loops-1):
		newfile.write(newname2 + "_" + str(h))
	else:
		newfile.write(newname2 + "_" + str(h) + ",")
	
newfile.write("\n};\n\n")	
	
print("Done.")
oldfile.close
newfile.close