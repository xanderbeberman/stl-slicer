#include "stl.h"



struct list2D {
	line* head;
	line* tail;
	struct list2D* next;
};

typedef struct list2D metaline;


facet* planeIntersect(facet* list, float z);

line* facetIntersect(facet* tri, float z);

line* lineCons(line* head, line* tail);

int freeLine(line* list);

void printLines(line* list);

line* makePerimeter(facet* list, float z);

float det(float ux, float uy, float vx, float vy);

short swap(line* seg);

bool eq(line* tip, line* tail, float t);

line* popLine(line* prev, line* current);

metaline* init(line* in);

metaline* findperims(line* in);

int freeMetaLine(metaline* list);

void printMetaLine(metaline* list);

bool moves(metaline* p, float tol);

//line* offsetLine(line* prev, line* tip, line* tail, float d);

line* offsetLine(line* p, float d);

line* offset(line* p, float d);

unsigned int printPerim(FILE* data, line* perim, float z);

metaline* offsetAll(metaline* p, float d);

void printAllPerim(FILE* data, metaline* perims, float z);

float extrude(line* seg);

line* getTail(line* p);

void printOneLine(line* head);