#include "listsort.h"

// this is a slightly modified mergesort implementation for linked lists
// that I use for this project. See below for reference:

/*
 * This file is copyright 2001 Simon Tatham.
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.  IN NO EVENT SHALL SIMON TATHAM BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


// gets the lowest z value of a facet
float low(facet* triangle) {
	return(fmin(triangle->vertex[0][2], fmin(triangle->vertex[1][2], triangle->vertex[2][2])));
}

// gets the highest z value of a facet
float high(facet* triangle) {
	return(fmax(triangle->vertex[0][2], fmax(triangle->vertex[1][2], triangle->vertex[2][2])));
}

// gets the lowest x/y value of a line
float lineLow(line* seg, bool xy) {
	return(fmin(seg->point[0][xy], seg->point[1][xy]));
}

// gets the highest x/y value of a line
float lineHigh(line* seg, bool xy) {
	return(fmax(seg->point[0][xy], seg->point[1][xy]));
}

// returns 0 if lowest vertex of l lower than lowest vertex of r
// not used
short compare_lowest(facet* l, facet* r) {
	float lowest = l->vertex[0][2];
	short which = 0;
	short i = 0;
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

// returns 1 if the lowest vertex of facet r is lower
// than the lowest vertex of facet l
int cmpf(element* l, element* r) {
	return(l->zmin > r->zmin);
}

// returns 1 if the lowest x/y value of line r is lower
// than the lowest x/y value of line l
int cmpl(line* l, line* r, bool xy) {
	return(lineLow(l, xy) > lineLow(r, xy));
}

// if lg = 1, returns 1 if the lowest x/y value of line r 
// is lower than the lowest x/y value of line l
int cmpp(point* l, point* r, bool xy, bool lg) {
	int cmp = l->pt[xy] > r->pt[xy];
	return(lg ? cmp:!cmp);
}

// sort functions for facets, lines, and points

/*
 * This is the actual sort function. Notice that it returns the new
 * head of the list. (It has to, because the head will not
 * generally be the same element after the sort.) So unlike sorting
 * an array, where you can do
 * 
 *     sort(myarray);
 * 
 * you now have to do
 * 
 *     list = listsort(mylist);
 */
element *facetsort(element *list, int is_circular, int is_double) {
    element *p, *q, *e, *tail, *oldhead;
    int insize, nmerges, psize, qsize, i;

    /*
     * Silly special case: if `list' was passed in as NULL, return
     * NULL immediately.
     */
    if (!list)
	return NULL;

    insize = 1;

    while (1) {
        p = list;
		oldhead = list;		       /* only used for circular linkage */
        list = NULL;
        tail = NULL;

        nmerges = 0;  /* count number of merges we do in this pass */

        while (p) {
            nmerges++;  /* there exists a merge to be done */
            /* step `insize' places along from p */
            q = p;
            psize = 0;
            for (i = 0; i < insize; i++) {
                psize++;
				if (is_circular) q = (q->next == oldhead ? NULL : q->next);
				else q = q->next;
                if (!q) break;
            }

            /* if q hasn't fallen off end, we have two lists to merge */
            qsize = insize;

            /* now we have two lists; merge them */
            while (psize > 0 || (qsize > 0 && q)) {

                /* decide whether next element of merge comes from p or q */
                if (psize == 0) {
		    		/* p is empty; e must come from q. */
			    	e = q; q = q->next; qsize--;
			    	// if (is_circular && q == oldhead) q = NULL;
				} else if (qsize == 0 || !q) {
			    	/* q is empty; e must come from p. */
			    	e = p; p = p->next; psize--;
			    	// if (is_circular && p == oldhead) p = NULL;
				} else if (cmpf(p,q) <= 0) {
			    /* First element of p is lower (or same);
			     * e must come from p. */
			    	e = p; p = p->next; psize--;
			    	// if (is_circular && p == oldhead) p = NULL;
				} else {
			    /* First element of q is lower; e must come from q. */
			    	e = q; q = q->next; qsize--;
			    	// if (is_circular && q == oldhead) q = NULL;
				}

	                /* add the next element to the merged list */
				if (tail) {
			    	tail->next = e;
				} else {
				    list = e;
				}
				/*
				if (is_double) {
				    // Maintain reverse pointers in a doubly linked list.
				    e->prev = tail;
				}
				*/
				tail = e;
            }

            /* now p has stepped `insize' places along, and q has too */
            p = q;
        }
    	
		if (is_circular) {
		    tail->next = list;
		    // if (is_double) list->prev = tail;
		} else tail->next = NULL;

	    /* If we have done only one merge, we're finished. */
	    if (nmerges <= 1)   /* allow for nmerges==0, the empty list case */
	        return list;

	    /* Otherwise repeat, merging lists twice the size */
	    insize *= 2;
    }
}

line *linesort(line *list, int is_circular, int is_double, bool xy) {
    line *p, *q, *e, *tail, *oldhead;
    int insize, nmerges, psize, qsize, i;

    /*
     * Silly special case: if `list' was passed in as NULL, return
     * NULL immediately.
     */
    if (!list)
	return NULL;

    insize = 1;

    while (1) {
        p = list;
		oldhead = list;		       /* only used for circular linkage */
        list = NULL;
        tail = NULL;

        nmerges = 0;  /* count number of merges we do in this pass */

        while (p) {
            nmerges++;  /* there exists a merge to be done */
            /* step `insize' places along from p */
            q = p;
            psize = 0;
            for (i = 0; i < insize; i++) {
                psize++;
				if (is_circular) q = (q->next == oldhead ? NULL : q->next);
				else q = q->next;
                if (!q) break;
            }

            /* if q hasn't fallen off end, we have two lists to merge */
            qsize = insize;

            /* now we have two lists; merge them */
            while (psize > 0 || (qsize > 0 && q)) {

                /* decide whether next element of merge comes from p or q */
                if (psize == 0) {
		    		/* p is empty; e must come from q. */
			    	e = q; q = q->next; qsize--;
			    	// if (is_circular && q == oldhead) q = NULL;
				} else if (qsize == 0 || !q) {
			    	/* q is empty; e must come from p. */
			    	e = p; p = p->next; psize--;
			    	// if (is_circular && p == oldhead) p = NULL;
				} else if (cmpl(p,q, xy) <= 0) {
			    /* First element of p is lower (or same);
			     * e must come from p. */
			    	e = p; p = p->next; psize--;
			    	// if (is_circular && p == oldhead) p = NULL;
				} else {
			    /* First element of q is lower; e must come from q. */
			    	e = q; q = q->next; qsize--;
			    	// if (is_circular && q == oldhead) q = NULL;
				}

	                /* add the next element to the merged list */
				if (tail) {
			    	tail->next = e;
				} else {
				    list = e;
				}
				/*
				if (is_double) {
				    // Maintain reverse pointers in a doubly linked list.
				    e->prev = tail;
				}
				*/
				tail = e;
            }

            /* now p has stepped `insize' places along, and q has too */
            p = q;
        }
    	
		if (is_circular) {
		    tail->next = list;
		    // if (is_double) list->prev = tail;
		} else tail->next = NULL;

	    /* If we have done only one merge, we're finished. */
	    if (nmerges <= 1)   /* allow for nmerges==0, the empty list case */
	        return list;

	    /* Otherwise repeat, merging lists twice the size */
	    insize *= 2;
    }
}

point* pointsort(point *list, int is_circular, int is_double, bool xy, bool lg) {
    point *p, *q, *e, *tail, *oldhead;
    int insize, nmerges, psize, qsize, i;

    /*
     * Silly special case: if `list' was passed in as NULL, return
     * NULL immediately.
     */
    if (!list)
	return NULL;

    insize = 1;

    while (1) {
        p = list;
		oldhead = list;		       /* only used for circular linkage */
        list = NULL;
        tail = NULL;

        nmerges = 0;  /* count number of merges we do in this pass */

        while (p) {
            nmerges++;  /* there exists a merge to be done */
            /* step `insize' places along from p */
            q = p;
            psize = 0;
            for (i = 0; i < insize; i++) {
                psize++;
				if (is_circular) q = (q->next == oldhead ? NULL : q->next);
				else q = q->next;
                if (!q) break;
            }

            /* if q hasn't fallen off end, we have two lists to merge */
            qsize = insize;

            /* now we have two lists; merge them */
            while (psize > 0 || (qsize > 0 && q)) {

                /* decide whether next element of merge comes from p or q */
                if (psize == 0) {
		    		/* p is empty; e must come from q. */
			    	e = q; q = q->next; qsize--;
			    	// if (is_circular && q == oldhead) q = NULL;
				} else if (qsize == 0 || !q) {
			    	/* q is empty; e must come from p. */
			    	e = p; p = p->next; psize--;
			    	// if (is_circular && p == oldhead) p = NULL;
				} else if (cmpp(p,q, xy, lg) <= 0) {
			    /* First element of p is lower (or same);
			     * e must come from p. */
			    	e = p; p = p->next; psize--;
			    	// if (is_circular && p == oldhead) p = NULL;
				} else {
			    /* First element of q is lower; e must come from q. */
			    	e = q; q = q->next; qsize--;
			    	// if (is_circular && q == oldhead) q = NULL;
				}

	                /* add the next element to the merged list */
				if (tail) {
			    	tail->next = e;
				} else {
				    list = e;
				}
				/*
				if (is_double) {
				    // Maintain reverse pointers in a doubly linked list.
				    e->prev = tail;
				}
				*/
				tail = e;
            }

            /* now p has stepped `insize' places along, and q has too */
            p = q;
        }
    	
		if (is_circular) {
		    tail->next = list;
		    // if (is_double) list->prev = tail;
		} else tail->next = NULL;

	    /* If we have done only one merge, we're finished. */
	    if (nmerges <= 1)   /* allow for nmerges==0, the empty list case */
	        return list;

	    /* Otherwise repeat, merging lists twice the size */
	    insize *= 2;
    }
}