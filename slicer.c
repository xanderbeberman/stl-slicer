#include "slicer.h"

// get the highest point in a sorted list of facets
float highz(facet* list) {
	facet* temp = list;
	if (temp == NULL) return(0);
	while (temp->next != NULL) {
		temp = temp->next;
	}
	return(temp->zmax);
}

// print usage message, exit program
void usage() {
	fprintf(stderr, "usage: slicer input_filename [-p perimeter_layers]\n\t[-f fill_percent(0-1)] [-l layer_height] [-t extruder_temperature]\n");
	exit(0);
}

// read command line commands and update values accordingly
int commands(int argc, char* argv[]) {
	if (argc < 2) usage();
	int i = 2;
	bool p = 0;
	bool f = 0;
	bool l = 0;
	bool t = 0;
	while (i < argc) {
		if (!strcmp(argv[i], "-p")) {
			if ((i+1) == argc) usage();
			perimeterLayers = strtoul(argv[i+1], NULL, 0);
			p = 1;
			i += 2;
		} else if (!strcmp(argv[i], "-f")) {
			if ((i+1) == argc) usage();
			fillPercent = strtof(argv[i+1], NULL);
			f = 1;
			i += 2;
		} else if (!strcmp(argv[i], "-l")) {
			if ((i+1) == argc) usage();
			layerHeight = strtof(argv[i+1], NULL);
			l = 1;
			i += 2;
		} else if (!strcmp(argv[i], "-t")) {
			if ((i+1) == argc) usage();
			temperature = strtof(argv[i+1], NULL);
			t = 1;
			i += 2;
		} else usage();
	}
	if (!l) layerHeight = 0.29;
	if (!f) fillPercent = 0.2;
	if (!p) perimeterLayers = 2;
	if (!t) temperature = 208;
	return(p+f+l);
}

int main(int argc, char* argv[]) {
	/*
	float layerHeight;
	float fillPercent;
	unsigned long int perimeterLayers;
	*/
	commands(argc, argv);
	// open stl file
	FILE* data = fopen(argv[1], "r");
	if (data == NULL) {
		fprintf(stderr, "error opening file\n");
		return(1);
	}
	// load stl file
	facet* stl = load(data);
	stl = facetsort(stl, 0, 0);
	fclose(data);
	// open output file
	FILE* out = fopen("output.gcode", "w");
	if (out == NULL) {
		fprintf(stderr, "error writing file\n");
		return(1);
	}
	// print header GCode
	fprintf(out, "M109 S%f\nG21\nG90\nM82\nM107\nG28 X0 Y0\nG28 Z0\nG29\nG1 Z15.0 F4200\nG92 E0\nG1 F200 E3\n", temperature);
	fprintf(out, "G92 E0\nG1 F4200\nM117 Printing...\n\nM107\nG0 F1600\n");
	// get layer height and max layer height
	float z = layerHeight/2;
	float max = highz(stl);
	// initialize variables
	unsigned int layer = 0;
	unsigned int pLayer = 0;
	unsigned int nlayers = floor(max/layerHeight);
	facet* slice;
	line* intersection;
	metaline* perimeters;
	metaline* prev;
	bool xy = 0;
	// for each layer
	while (layer < nlayers) {
		fprintf(out, ";LAYER %u\n", layer);
		if (layer >= 4) fprintf(out, "M106 S255\n");
		// get set of facets that intersect
		slice = planeIntersect(stl, z);
		// get intersection of facets and plane
		intersection = makePerimeter(slice, z);
		// find perimeters
		perimeters = findperims(intersection);
		// print perimeters to file
		printAllPerim(out, perimeters, z);
		pLayer = 0;
		// for each additional perimeter layer
		while (pLayer < (perimeterLayers - 1)) {
			// set a pointer to the current perimeter
			prev = perimeters;
			// offset perimeter inwards
			perimeters = offsetAll(perimeters, filamentWidth/2);
			// print offset perimeter to file
			printAllPerim(out, perimeters, z);
			// free previous perimeter
			freeMetaLine(prev);
			pLayer++;
		}
		// print fill
		if (layer < 4) printFill(out, perimeters, 1, xy);
		else if (layer > (nlayers - 4)) printFill(out, perimeters, 1, xy);
		else printFill(out, perimeters, fillPercent, xy);
		// update and free variables
		xy = !xy;
		z += layerHeight;
		layer += 1;
		freeall(slice);
		freeMetaLine(perimeters);
	}
	// print footer gcode
	fprintf(out, "M107\nG0 F4200 X70 Y0 Z10.000\nM104 S0\nM140 S0\nG91\nG1 E-1 F300\nG1 Z+0.5 E-5 X-20 Y-20 F4200\nG28 X0 Y0\nM84\nG90");
	// clean up
	freeall(stl);
	fclose(out);
	fprintf(stderr, "Successfully output toolpath in output.gcode\n");
	return(0);
}


	