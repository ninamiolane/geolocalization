.PHONY: clean

.SUFFIXES:


all: compute_gist.c gist.o standalone_image.o
	gcc -Wall -g $^ -lfftw3f -lcv -lhighgui -o compute_gist


gist.o: gist.c gist.h standalone_image.h
	gcc -c -Wall -g $< -DUSE_GIST -DSTANDALONE_GIST

standalone_image.o: standalone_image.c standalone_image.h
	gcc -c -Wall -g $<


clean: 
	rm -f *.o compute_gist

