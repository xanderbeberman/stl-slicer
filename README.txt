Slicer project for Digital Fabrication class, December 2016.
This program inputs an STL file and exports GCODE for controlling a 3D printer. The parameters are set up to print well on a PrintrBot Simple Metal. With the exception of listsort.c, this entire project was done from scratch with no references whatsoever.

HOW TO USE:
1. Make the executable
2. run:
	slicer input_filename [-p perimeter_layers] [-f fill_percent(0-1)] [-l layer_height] [-t extruder_temperature]
3. output is saved in "output.gcode"

Some notes:
- It can accurately print rectilinear infill and multiple perimeter layers, but I attempted to write an offset function from scratch which occasionally messes up.
- Top and bottom 4 layers have 100% infill for surface quality
- The physical prints this program created turned out well, with dimensions only +/1 0.02mm off.
- The perimeter-finding algorithm I wrote can find both internal and external closed perimeters
- It can only read in one solid from the stl file, but if you just put all the facets of another solid into the first solid it would work
