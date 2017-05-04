#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "stl.h"

// frees all facets in a list of facets
int freeall(facet* list) {
	if (list == NULL) return 0;
	facet* next = list->next;
	free(list);
	return(freeall(next));
}

// cons facet new onto facet list
facet* cons(facet* new, facet* list) {
	if (new == NULL) return (list);
	new->next = list;
	return new;
}

// copy everything but next pointer of a facet to a new facet
facet* copy(facet* old) {
	if (old == NULL) return(NULL);
	// allocate space for new facet
	facet* new = malloc(sizeof(facet));
	if (new == NULL) return(NULL);
	// assign normals and vertices
	short i = 0;
	while (i < 3) {
		new->normal[i] = old->normal[i];
		short j = 0;
		while (j < 3) {
			new->vertex[i][j] = old->vertex[i][j];
			j++;
		}
		i++;
	}
	// assign zmin and zmax
	new->zmin = old->zmin;
	new->zmax = old->zmax;
	// do not assign next pointer
	new->next = NULL;
	return(new);
}

// loads all facets in a solid in an ascii stl file into memory
// only supports one solid
facet* load(FILE* data) {
	// initialize variables
	// count number of facets
	unsigned int triangles = 0;
	facet* head = NULL;
	// check for start of solid
	fscanf(data, " solid %*s");
	float x, y, z;
	// check for facet and normalnormal
	while (fscanf(data, " facet normal %f %f %f", &x, &y, &z) != 0) {
		// create new facet
		facet* new = malloc(sizeof(facet));
		if (new == NULL) {
			freeall(head);
			exit(1);
		}
		// assign normal to new facet
		new->normal[0] = x;
		new->normal[1] = y;
		new->normal[2] = z;
		// scan for vertices and assign to new facet
		fscanf(data, " outer loop\n vertex %f %f %f", &(new->vertex[0][0]), &(new->vertex[0][1]), &(new->vertex[0][2]));
		fscanf(data, " vertex %f %f %f", &(new->vertex[1][0]), &(new->vertex[1][1]), &(new->vertex[1][2]));
		fscanf(data, " vertex %f %f %f", &(new->vertex[2][0]), &(new->vertex[2][1]), &(new->vertex[2][2]));
		// scan for end of facet
		fscanf(data, " endloop endfacet");
		// compute and assign zmin and zmax
		new->zmin = low(new);
		new->zmax = high(new);
		// add new facet to list of facets
		head = cons(new, head);
		// increment triangles
		triangles++;
	}
	// scan for end of solid
	fscanf(data, " endsolid");
	// return list of facets
	return head;
}



/*
// returns 0 if lowest vertex of l lower than lowest vertex of r
unsigned short compare_lowest(facet* l, facet* r) {
	float lowest = l->vertex[0][2];
	unsigned short which = 0;
	unsigned short i = 0;
	while (i < 3) {
		if (l->vertex[i][2] < lowest) {
			lowest = l->vertex[i][2];
			which = 0;
		}
		if (r->vertex[i][2] < lowest) {
			lowest = r->vertex[i][2];
			which = 1;
		}
		i++;
	}
	return which;
}
*/

// prints all facets to screen
void printall(facet* list) {
	if (list != NULL) {
		printf("Normal: %f %f %f\n", list->normal[0], list->normal[1], list->normal[2]);
		int i = 0;
		while (i < 3) {
			printf("Facet: %f %f %f\n", list->vertex[i][0], list->vertex[i][1], list->vertex[i][2]);
			i++;
		}
		printf("\n");
		printall(list->next);
	}
}

