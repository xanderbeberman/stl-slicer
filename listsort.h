#include "parameters.h"

#define FALSE 0
#define TRUE 1



struct stlFacet {
	float normal[3]; // x,y,z for normal vector
	float vertex[3][3]; // [vertex #][x,y,z]
	float zmin;
	float zmax;
	struct stlFacet* next;
};

typedef struct stlFacet facet;

typedef facet element;

struct line2D {
	float point[2][2];
	float normal[2];
	struct line2D* next;
};

typedef struct line2D line;

struct point2D {
	float pt[2];
	struct point2D* next;
};

typedef struct point2D point;


//typedef struct stl_facet facet;

#define FALSE 0
#define TRUE 1


float low(facet* triangle);

float high(facet* triangle);

short compare_lowest(facet* l, facet* r);

int cmp(element* l, element* r);

element *facetsort(element *list, int is_circular, int is_double);

line* linesort(line *list, int is_circular, int is_double, bool xy);

point* pointsort(point *list, int is_circular, int is_double, bool xy, bool lg);

float lineLow(line* seg, bool xy);

float lineHigh(line* seg, bool xy);

int cmpf(element* l, element* r);

int cmpl(line* l, line* r, bool xy);

int cmpp(point* l, point* r, bool xy, bool lg);
