#include "fill.h"

// returns the length of a list of lines
unsigned int lengthL(line* in) {
	line* temp = in;
	unsigned int c = 0;
	while (temp != NULL) {
		c++;
		temp = temp->next;
	}
	return(c);
}

// returns the total number of lines in of a list of metalines
unsigned int lengthML(metaline* in) {
	metaline* temp = in;
	unsigned int c = 0;
	while (temp != NULL) {
		c += lengthL(temp->head);
		temp = temp->next;
	}
	return(c);
}

// cons point head to tail
point* pointCons(point* head, point* tail) {
	if (head == NULL) return(tail);
	head->next = tail;
	return(head);
}

// take a list of metalines and return a list of all lines in each metaline
line* condenseLines(metaline* perim) {
	if (perim == NULL) return(NULL);
	line* head = perim->head;
	metaline* temp = perim->next;
	while (temp != NULL) {
		lineCons(temp->tail, head);
		head = temp->head;
		temp = temp->next;
	}
	return(head);
}

// given sorted perimeter as a list of lines perim
// return a list of points that intersect the perimeter at height/width h
// height or width determined by xy: xy = 1 ==> x, 0 ==> y
point* lineIntersect(line* perim, float h, bool xy) {
	point* myPoint = NULL;
	line* list = perim;
	point* temp;
	float r;
	// for each line in perimeter
	while (list != NULL) {
		// if the given height is within the line
		if ((lineLow(list, xy) <= h) && (lineHigh(list, xy) >= h)) {
			// create a new point
			temp = malloc(sizeof(point));
			if (temp == NULL) exit(1);
			// assign values to the point based on the intersection of the line and h
			r = (h - list->point[0][xy])/(list->point[1][xy] - list->point[0][xy]);
			temp->pt[0] = list->point[0][0] + r*(list->point[1][0] - list->point[0][0]);
			temp->pt[1] = list->point[0][1] + r*(list->point[1][1] - list->point[0][1]);
			// attach the point to the list of points
			myPoint = pointCons(temp, myPoint);
		} else if (lineLow(list, xy) > h) break;
		// iterate through list
		list = list->next;
	}
	return(myPoint);
}

// given a list of points, return a list of lines connecting every two points
// if we have points a, b, c, d, the lines returned are a-b and c-d, but not b-c
// any odd-hanging point at the end is ignored
line* pointToLine(point* myPoint) {
	point* list = myPoint;
	line* temp;
	line* myLine = NULL;
	// for every other point in the list
	while (list != NULL) {
		// if this is the last point, ignore it
		if (list->next == NULL) break;
		// otherwise create a line
		temp = malloc(sizeof(line));
		// set the tail of the line to the first point
		temp->point[0][0] = list->pt[0];
		temp->point[0][1] = list->pt[1];
		// iterate to the next point in the list
		list = list->next;
		// set the tip of the line to the next point
		temp->point[1][0] = list->pt[0];
		temp->point[1][1] = list->pt[1];
		// add this line to the list of lines
		myLine = lineCons(temp, myLine);
		// iterate through list of points
		list = list->next;
	}
	return(myLine);
}

// frees all points in a list of points
int freePoint(point* list) {
	point* temp;
	while (list != NULL) {
		temp = list->next;
		free(list);
		list = temp;
	}
	return(0);
}

// returns a pointer to a float array giving the
// minimum and maximum for x and y in a list of lines
float* makebox(line* perim) {
	line* temp = perim;
	if (temp == NULL) return(NULL);
	// allocate space for the array
	float* box = malloc(4 * sizeof(float));
	if (box == NULL) exit(1);
	// assign min and max values to first point in the line initially
	// box[0] = min(x), box[1] = min(y), box[2] = max(x), box[3] = max(y)
	box[0] = perim->point[0][0];
	box[1] = perim->point[0][1];
	box[2] = perim->point[0][0];
	box[3] = perim->point[0][1];
	// for x and y
	short i = 0;
	while (i < 2) {
		// start at the head of the list
		temp = perim;
		while (temp != NULL) {
			// assign box values to min of tip and tail of line
			box[i] = fmin(box[i], temp->point[0][i]);
			box[i] = fmin(box[i], temp->point[1][i]);
			// assign box values to max of tip and tail of line
			box[2+i] = fmax(box[2+i], temp->point[0][i]);
			box[2+i] = fmax(box[2+i], temp->point[1][i]);
			// iterate through list
			temp = temp->next;

		}
		i++;
	}
	return(box);
}

// prints all points in a list of points
void printPoints(point* pts) {
	point* temp = pts;
	while (temp != NULL) {
		printf("%f %f\n", temp->pt[0], temp->pt[1]);
		temp = temp->next;
	}
}

// outputs a rectilinear fill pattern to a file
// inputs data: file pointer, perims: list of metalines (perimeter)
// fillPercent: fill percentage from 0 to 1
// xy: boolean 0 ==> y fill lines, 1 ==> x fill lines
unsigned int printFill(FILE* data, metaline* perims, float fillPercent, bool xy) {
	// offset perimeter by fill percentage
	metaline* inner = offsetAll(perims, filamentWidth);
	// condense into one list of lines
	line* boundary = condenseLines(inner);
	// sort list by min(tail(x/y)) ascending
	boundary = linesort(boundary, 0, 0, xy);
	// initialize variables and values
	point* temp;
	line* fill;
	line* fillHead;
	unsigned int c = 0;
	// bounding box
	float* minmax = makebox(boundary);
	// amount to move between lines
	float delta = filamentWidth/fillPercent;
	// current spot
	float current = minmax[xy] + delta/2;
	// maximum height/width
	float max = minmax[2 + xy];
	// this is to optimize movement to some degree and flip lines back and forth
	bool lg = 0;
	// for each height/width value less than max
	while (current < max) {
		// get intersection of line and boundary
		temp = lineIntersect(boundary, current, xy);
		// sort intersection
		temp = pointsort(temp, 0, 0, !xy, lg);
		// convert to list of lines
		fill = pointToLine(temp);
		// save head of list
		fillHead = fill;
		// for each line in list
		while (fill != NULL) {
			// output move and print GCodes to file
			fprintf(data, "G0 X%f Y%f\n", fill->point[0][0], fill->point[0][1]);
			// extrude updates the extrusion variable to
			// tell the printer how much to extrude
			extrude(fill);
			fprintf(data, "G1 X%f Y%f E%f\n", fill->point[1][0], fill->point[1][1], ext);
			// iterate through list
			fill = fill->next;
			c++;
		}
		// free temporary list
		freeLine(fillHead);
		// update height/width
		current += delta;
		// reverse direction of lines
		lg = !lg;
	}
	// free bounding box and offset perimeter
	free(minmax);
	free(inner);
	// return the number of lines output
	return(c);
}

