#include "perim.h"

point* pointCons(point* head, point* tail);

line* condenseLines(metaline* perim);

point* lineIntersect(line* perim, float h, bool xy);

line* pointToLine(point* myPoint);

int freePoint(point* list);

float* makebox(line* perim);

unsigned int printFill(FILE* data, metaline* perims, float fillPercent, bool xy);

