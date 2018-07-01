# Makefile for edaf35 project
# Run make followed by either "buddy" or "linked" to run

buddy:
	make clean
	cp buddy.c gawk-3.1.8/my_alloc.c
	make run

linked:
	make clean
	cp linked.c gawk-3.1.8/my_alloc.c
	make run

run:
	cd gawk-3.1.8/ && $(MAKE) clean check

clean:
	rm -f *.o
	rm -f *.exe
	rm -f *.out
	rm -f gawk-3.1.8/my_alloc.c
