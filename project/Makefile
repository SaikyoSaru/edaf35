configure:
	cd gawk-3.1.8/ && ./configure

cp_linked:
	cp linked/malloc.* gawk-3.1.8/

cp_buddy:
	cp buddy/malloc.* gawk-3.1.8/


gcc: gawk-3.1.8/malloc.o
	cd gawk-3.1.8/ &&	gcc -c malloc.c

check:
	cd gawk-3.1.8/ && make check

clean:
	rm -f gawk-3.1.8/malloc.*

magic:
	bash edit.sh

lazy_linked:
	make clean
	make cp_linked
	make configure
	make check
	make magic
	make check

lazy_buddy:
	make clean
	make cp_buddy
	make configure
	make check
	make magic
	make check
