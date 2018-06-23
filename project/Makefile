# Makefile for edaf35 project
# Run make followed by either "buddy" or "linked" to run

buddy:
	cp buddy.c gawk-3.1.8/
	make run

linked:
	cp memory.c gawk-3.1.8/
	make run

run:
	cd gawk-3.1.8/ && make check

clean:
	rm -f *.o
	rm -f *.exe
	rm -f *.out
	rm -f gawk-3.1.8/buddy.c
	rm -f gawk-3.1.8/memory.c