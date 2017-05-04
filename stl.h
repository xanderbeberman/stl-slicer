#include "listsort.h"


int freeall(facet* list);

facet* cons(facet* new, facet* list);

facet* copy(facet* old);

facet* load(FILE* data);

void printall(facet* list);