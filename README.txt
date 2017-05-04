Xander Beberman DigiFab fall 2016 slicer readme!
Wow this was a hard project and I really should have found a partner for it. Oh well.

HOW TO USE:
1. Make the executable
2. run:
	slicer input_filename [-p perimeter_layers] [-f fill_percent(0-1)] [-l layer_height] [-t extruder_temperature]
3. output is saved in "output.gcode"
I only compiled this on my mac so I really hope it works elsewhere. Fingers crossed.

WHAT I ACCOMPLISHED:
- I mean, it works for the most part
- It can accurately print rectilinear infill and multiple perimeter layers
- Top and bottom 4 layers have 100% infill for surface quality
- I did actually print something with it and it turned out well! Even the z-axis thickness was only 0.02mm off!
- It can do perimeters with holes well
- It can take in parameters from the command line
- If all assumptions are followed it really should work

ASSUMPTIONS AND LIMITATIONS:
- this is realy, really easy to break. Fill doesn't work when the perimeters are too close together, if you make the number of perimeter layers too high it will keep printing past the object, etc.
	- Basically, if you assume that everything the user inputs is correct, it doesn't need scaffolding, and that there are no points in the model where the distance between two walls is less than 2(number of perimeter layers)(filament width) it should work
	- If you adjusted the scaler of mm extruded/mm moved (amt in parameters.c) and the filament width, you could do fill arbitrarily small gaps though
- I only tested it on my printrbot, so I don't know if it works on a reprap. It works on a printrbot though!
- I was not able to implement support structures
- It doesn't really work at z=0 or when facets are contained in planes and I can't figure out why. However, it is written so this will rarely happen
- It can only read in one solid from the stl file. I think if you just put all the facets of another solid into the first solid it would work but I haven't tried it
- I did not have time to implement retraction, so you get strings of filament when moving over holes sometimes