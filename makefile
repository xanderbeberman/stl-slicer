slicer: slicer.o fill.o perim.o stl.o listsort.o parameters.o
	gcc -o slicer slicer.o fill.o perim.o stl.o listsort.o parameters.o

parameters.o: parameters.c parameters.h
	gcc -c parameters.c

listsort.o: listsort.c listsort.h parameters.h
	gcc -c listsort.c

stl.o: stl.c stl.h listsort.h parameters.h
	gcc -c stl.c

perim.o: perim.c perim.h stl.h listsort.h parameters.h
	gcc -c perim.c

fill.o: fill.c fill.h perim.h stl.h listsort.h parameters.h
	gcc -c fill.c

slicer.o: slicer.c slicer.h fill.h perim.h stl.h listsort.h parameters.h
	gcc -c slicer.c

clean:
	rm slicer *.o
