#include "perim.h"

// inputs list of facets, z value, returns a list of facets
// that intersect with the plane at the z value
facet* planeIntersect(facet* list, float z) {
	// initialize variables
	facet* intersection = NULL;
	facet* head = list;
	facet* temp;
	int c = 0;
	int helpme = 0;
	// for each facet in the list
	while (head != NULL) {
		// if the facet lies entirely on the z plane, ignore it
		if ((head->zmin == z) && (head->zmax == z));
		// if the facet intersects the plane
		else if (head->zmin <= z && head->zmax >= z) {
			// duplicate the facet
			temp = copy(head);
			if (temp == NULL) {
				freeall(list);
				freeall(head);
				exit(1);
			}
			// and add it to the list of intersecting facets
			intersection = cons(temp, intersection);
		}
		// if the minimum z value of the facet is above the plane
		// break, since there will be no more intersections
		// if the list is properly sorted
		else if (head->zmin > z) {
			break;
		}
		// iterate through list
		head = head->next;
		c++;
	}
	return(intersection);
}

// returns the determinant of u(x,y) and v(x,y)
float det(float ux, float uy, float vx, float vy) {
	return(ux*vy - uy*vx);
}

// determines if a line is in the correct orientation
// as part of a perimeter based on cross product
// If not, flip it
short swap(line* seg) {
	if (seg == NULL) exit(1);
	// get vector from line
	float ux = seg->point[1][0] - seg->point[0][0];
	float uy = seg->point[1][1] - seg->point[0][1];
	// take cross product (= determinant in 2d) of
	// line vector and normal vector
	if (det(ux, uy, seg->normal[0], seg->normal[1]) < 0) {
		// if in the incorrect orientation,
		// swap tip and tail of line
		ux = seg->point[0][0];
		uy = seg->point[0][1];
		seg->point[0][0] = seg->point[1][0];
		seg->point[0][1] = seg->point[1][1];
		seg->point[1][0] = ux;
		seg->point[1][1] = uy;
		return(1);
	}
	return(0);
}

// get the interstection of a facet with a plane
// in the form of a line
line* facetIntersect(facet* tri, float z) {
	// get z(vertex) - z(plane) for each vertex in facet
	float ztest[3];
	short odd;
	for (short i = 0; i < 3; i++) {
		ztest[i] = tri->vertex[i][2] - z;
	}
	// determine which two intersection to compute to create a line based on 
	// where the vertices are with respect to the plane
	// if all 3 on the plane return NULL
	if ((fabs(ztest[0]) < 0.0001) && (fabs(ztest[1]) < 0.0001) && (fabs(ztest[2]) < 0.0001)) return(NULL);
	if ((ztest[0] < 0) ^ (ztest[1] < 0)) {
		if ((ztest[0] < 0) ^ (ztest[2] < 0)) {
			odd = 0;
		} else odd = 1;
	} else odd = 2;
	// odd is the index of vertices that is on the opposite side of the plane as the other two
	// create new line
	line* myLine = malloc(sizeof(line));
	if (myLine == NULL) exit(1);
	// assign values based on intersection
	for (short j = 1; j < 3; j++) {
		short temp = (odd + j) % 3;
		for (short i = 0; i < 2; i++) {
			myLine->point[j-1][i] = (ztest[odd]/(tri->vertex[odd][2] - tri->vertex[temp][2]))*(tri->vertex[odd][i] - tri->vertex[temp][i]) + tri->vertex[temp][i];
		}
	}
	// renormalize normal vector
	float norm = sqrt(pow(tri->normal[0], 2)+pow(tri->normal[1], 2));
	// and assign it
	myLine->normal[0] = tri->normal[0]/norm;
	myLine->normal[1] = tri->normal[1]/norm;
	// assign next to null
	myLine->next = NULL;
	// check if in correct orientation and swap
	swap(myLine);
	return(myLine);
}

// cons line head onto line list tail
line* lineCons(line* head, line* tail) {
	if (head == NULL) return tail;
	head->next = tail;
	return(head);
}

// frees all lines in a list of lines
int freeLine(line* list) {
	if (list == NULL) return 0;
	line* next = list->next;
	free(list);
	return(freeLine(next));
}

// prints one line
void printOneLine(line* list) {
	if (list != NULL) {
		printf("%f %f\n", list->point[0][0], list->point[0][1]);
		printf("%f %f\n\n", list->point[1][0], list->point[1][1]);
	}
}

// prints a list of lines
void printLines(line* list) {
	line* temp = list;
	while (temp != NULL) {
		printf("%f %f\n", temp->point[0][0], temp->point[0][1]);
		printf("%f %f\n\n", temp->point[1][0], temp->point[1][1]);
		temp = temp->next;
	}
}

// gets a list of all lines that intersect the objects at plane z
// input list of facets that intersect plane already
line* makePerimeter(facet* list, float z) {
	// initialize variables
	line* perimeter = NULL;
	line* temp;
	int c = 0;
	// for each facet in list
	while (list != NULL) {
		// get intersection line
		temp = facetIntersect(list, z);
		if (temp != NULL) {
			// and add it to list of lines
			perimeter = lineCons(temp, perimeter);
		}
		// iterate through list
		list = list->next;
	}
	return(perimeter);
}

// approximate equals for floats
bool eq(line* tip, line* tail, float t) {
	return((fabs(tip->point[1][0] - tail->point[0][0]) + fabs(tip->point[1][1] - tail->point[0][1])) < t );
}

// removes a line from a list, is not used
line* popLine(line* prev, line* current) {
	if (current == NULL) return(NULL);
	if (prev == NULL) return(current->next);
	prev->next = current->next;
	return(current->next);
}

// copies a line to a new allocation
line* lineCopy(line* in) {
	if (in == NULL) return(NULL);
	// allocate space for ne wline
	line* new = malloc(sizeof(line));
	// assign parameters accordingly
	for (short i = 0; i < 2; i++) {
		for (short j = 0; j < 2; j++) {
			new->point[i][j] = in->point[i][j];
		}
		new->normal[i] = in->normal[i];
	}
	new->next = in->next;
	return(new);
}

// initialize a new metaline
// a metaline is a linked list structure that stores both the head and tail
// of a list of lines, and the next linked metaline
// assumes that line is a singular line and not a list
metaline* init(line* in) {
	// allocate space for new metaline
	metaline* p = malloc(sizeof(metaline));
	if (p == NULL) return(NULL);
	// copy line to new space
	line* new = lineCopy(in);
	if (new == NULL) exit(1);
	new->next = NULL;
	// set metaline head and tail to line
	p->head = new;
	p->tail = new;
	// set metaline next to null
	p->next = NULL;
	return(p);
}

// moves is essentially a subroutine for the algorithm that sorts a list of
// lines into a list of perimeters, and will probably make more sense if
// you read it after you read the next function

// inputs a list of metalines and a tolerance, and does one move:
// checks if the tail of any current perimeter comes immediately
// before the head of any other current perimieter
// if so, merge the two and remove the unnecessary metaline
bool moves(metaline* p, float tol) {
	// initialize variables
	metaline* pcur;
	metaline* pprev = p;
	metaline* ptemp;
	int i = 0;
	int j;
	// for each metaline (perimeter) in the list
	while (pprev != NULL) {
		// set the current and next metalines
		pcur = pprev;
		ptemp = pprev->next;
		j = i+1;
		// for each metaline after the current one
		while (ptemp != NULL) {
			// if the tail of the current base metaline (pprev) lines up
			// with the head of the current auxillary metaline (ptemp),
			if (eq(pprev->tail, ptemp->head, tol)) {
				// merge the two
				pprev->tail->next = ptemp->head;
				pprev->tail = ptemp->tail;
				pcur->next = ptemp->next;
				// and remove the auxillary metaline
				free(ptemp);
				// return that a move was made
				return(1);
			}
			// same thing but head of base metaline and tail of aux metaline
			if (eq(ptemp->tail, pprev->head, tol)) {
				ptemp->tail->next = pprev->head;
				pprev->head = ptemp->head;
				pcur->next = ptemp->next;
				free(ptemp);
				return(1);
			}
			// iterate through list of metalines after base
			pcur = ptemp;
			ptemp = ptemp->next;
		}
		// iterate through all metalines in list
		pprev = pprev->next;
		i++;
	}
	// if we get to this point, there are no possible moves, so
	// return that a move has not been made
	return(0);
}


// findperims takes a list of lines and sorts it into perimeters
// in a true perimeter, the tip of each segment will always correspond to the
// tail of another segment. This function uses that fact to create new
// perimeters and merge them until all lines have been used and all possible
// merges have been made
// a metaline generally corresponds to a list of perimeters
metaline* findperims(line* in) {
	// intialize variables
	float tol = 0.001;
	line* list = in;
	if (list == NULL) return(NULL);
	// initialize the first metaline in the list of perimeters
	metaline* p = init(list);
	list = list->next;
	p->tail->next = NULL;
	// initialize temporary metaline pointers
	metaline* ptemp = p;
	metaline* pprev;
	bool mov;
	// for each line in the list
	while (list != NULL) {
		// reset the base metaline to the first one
		ptemp = p;
		pprev = p;
		// for each metaline
		while (pprev != NULL) {
			// if at the end, create a new perimeter for this line
			if (ptemp == NULL) {
				// initialize metaline
				ptemp = init(list);
				// make sure pointers work OK
				list = list->next;
				ptemp->tail->next = NULL;
				pprev->next = ptemp;
				// no moves can be made in this case
				mov = 0;
				// go back to the first loop
				break;
			}
			// if the current line is subsequent to the
			// end of the current metaline
			if (eq(ptemp->tail, list, tol)) {
				// add it to the end of the perimeter
				ptemp->tail->next = list;
				ptemp->tail = ptemp->tail->next;
				list = list->next;
				ptemp->tail->next = NULL;
				// moves can be made in this case
				mov = 1;
				// go on to the next step
				break;
			}
			// iterate through the list
			pprev = ptemp;
			ptemp = ptemp->next;
		}
		// while a move is possible
		while (mov) {
			// check for and do moves
			mov = moves(p, tol);
		}
		// do this for all lines in the list of perimeters
		// this is slow but I'm pretty sure it's guaranteed to
		// return a list of all connected lines 
	}
	return(p);
}

// frees all metalines in a list of metalines and all lines
// in the lists of lines contained therein
int freeMetaLine(metaline* list) {
	metaline* temp;
	while (list != NULL) {
		temp = list->next;
		if (list->head != NULL) freeLine(list->head);
		free(list);
		list = temp;
	}
	return(0);
}

// prints a metaline
void printMetaLine(metaline* list) {
	metaline* temp = list;
	int i = 0;
	while (temp != NULL) {
		printf("\nperimeter %d\n", i);
		printLines(temp->head);
		temp = temp->next;
		i++;
	}
}


// part of faking offsetting a line
// turns out this is a hard problem and this is a quick
// solution that works well enough
// given a line and a distance d
line* offsetLine(line* old, float d) {
	// allocate space for new line
	line* new = malloc(sizeof(line));
	if (new == NULL) exit(1);
	// set tail point to old tail moved by d*normal
	new->point[0][0] = old->point[0][0] - d*old->normal[0];
	new->point[0][1] = old->point[0][1] - d*old->normal[1];
	// set normal to old normal
	new->normal[0] = old->normal[0];
	new->normal[1] = old->normal[1];
	// set next to null
	new->next = NULL;
	return(new);
}

// returns the tail of a list of lines
line* getTail(line* p) {
	line* temp = p;
	while (temp != NULL) {
		if (temp->next == NULL) break;
		temp = temp->next;
	}
	return(temp);
}

// I thought I knew how to offset things
// It didn't work
/*
line* offsetLine(line* prev, line* tip, line* tail, float d) {
	if (tip == NULL || tail == NULL) return(NULL);
	line* new = malloc(sizeof(line));
	if (new == NULL) exit(1);
	float v[2];
	v[0] = tip->normal[0] + tail->normal[0];
	v[1] = tip->normal[1] + tail->normal[1];	
	float n = sqrt(pow(v[0], 2) + pow(v[1], 2));
	new->normal[0] = v[0]/n;
	new->normal[1] = v[1]/n;
	new->point[1][0] = tip->point[1][0] - d*new->normal[0];
	new->point[1][1] = tip->point[1][1] - d*new->normal[1];
	new->point[0][0] = prev->point[1][0];
	new->point[0][1] = prev->point[1][1];
	return(new);
}

line* offset(line* p, float d) {
	if (p == NULL) return(NULL);
	line* old = p;
	line* prev;
	//line* temp;
	line* tail;
	line* head;
	if (old->next == NULL) {
		head = malloc(sizeof(line));
		head->point[0][0] = old->point[0][0] - d*old->normal[0];
		head->point[0][1] = old->point[0][1] - d*old->normal[1];
		head->point[1][0] = old->point[1][0] - d*old->normal[0];
		head->point[1][1] = old->point[1][1] - d*old->normal[1];
		head->normal[0] = old->normal[0];
		head->normal[1] = old->normal[0];
		head->next = NULL;
		return(head);
	} else {
		head = offsetLine(old, old, old->next, d);
		prev = head;
		old = old->next;
		tail = head;
	}
	while (old->next != NULL) {
		tail->next = offsetLine(prev, old, old->next, d);
		prev = tail;		
		tail = tail->next;
		old = old->next;
	}
	tail->next = offsetLine(prev, old, head, d);
	tail = tail->next;
	head->point[0][0] = tail->point[1][0];
	head->point[0][1] = tail->point[1][1];
	return(head);
}
*/

// fakes offsetting a connected perimeter
// given a perimeter p and a distance
line* offset(line* p, float d) {
	if (p == NULL) return(NULL);
	// initialize variables
	line* old = p;
	line* temp;
	// create first offset line
	line* head = offsetLine(p, d);
	// if only 1 line in perimeter, offset entirely by normal
	if (old->next == NULL) {
		head->point[1][0] = old->point[1][0] - d*old->normal[0];
		head->point[1][1] = old->point[1][1] - d*old->normal[1];
		return(head);
	}
	// otherwise, offset by the normal of the next line
	head->point[1][0] = old->point[1][0] - d*old->next->normal[0];
	head->point[1][1] = old->point[1][1] - d*old->next->normal[1];
	// update pointers
	line* tail = head;
	old = old->next;
	// iterate through perimeter
	while (old->next != NULL) {
		// create a new line
		tail->next = offsetLine(old, d);
		// update pointers
		tail = tail->next;
		// and offset tip by normal of next line
		tail->point[1][0] = old->point[1][0] - d*old->next->normal[0];
		tail->point[1][1] = old->point[1][1] - d*old->next->normal[1];
		old = old->next;
	}
	// offset tip of last line to normal of first line
	tail->next = offsetLine(old, d);
	tail = tail->next;
	tail->point[1][0] = old->point[1][0] - d*head->normal[0];
	tail->point[1][1] = old->point[1][1] - d*head->normal[1];
	return(head);
}
// this ends up with not quite an offset, and twists the perimeter slightly,
// but works OK when just making perimeter walls


// calls offset on each line in a metaline
// and creates a new, offset metaline
metaline* offsetAll(metaline* p, float d) {
	metaline* old = p;
	metaline* prev = NULL;
	metaline* temp;
	while (old != NULL) {
		temp = init(old->head);
		temp->head->next = old->head->next;
		temp->head = offset(temp->head, d);
		temp->tail = getTail(temp->head);
		temp->next = prev;
		prev = temp;
		old = old->next; 
	}
	return(prev);
}

// determines how much to extrude based linearly
// on the length of a line segment
float extrude(line* seg) {
	float x = (seg->point[1][0] - seg->point[0][0]);
	float y = (seg->point[1][1] - seg->point[0][1]);
	float len = sqrt(pow(x, 2)+pow(y, 2));
	ext += len*amt;
	return(ext);
}

// outputs GCode to print a perimeter to the output file
unsigned int printPerim(FILE* data, line* perim, float z) {
	if (data == NULL) return(0);
	unsigned int c = 0;
	if (perim == NULL) return(0);
	// go to the start location of the perimeter
	fprintf(data, "G0 X%f Y%f Z%f\n", perim->point[0][0], perim->point[0][1], z);
	// for all lines in the perimeter
	while (perim != NULL) {
		// extrude the correct amount
		extrude(perim);
		// while moving to the tip of the line
		// the tail is excluded because this is a connected perimeter
		fprintf(data, "G1 X%f Y%f E%f\n", perim->point[1][0], perim->point[1][1], ext);
		// iterate through list
		perim = perim->next;
		c++;
	}
	// return number of paths
	return(c);
}


// outputs GCode to print all perimeters in a metaline to output file
void printAllPerim(FILE* data, metaline* perims, float z) {
	while (perims != NULL) {
		printPerim(data, perims->head, z);
		perims = perims->next;
	}
}