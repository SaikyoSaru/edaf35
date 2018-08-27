configure:
	cd gawk-3.1.8/ && ./configure

cp_linked:
	cp linked/my_malloc.* gawk-3.1.8/

cp_buddy:
	cp buddy/my_malloc.* gawk-3.1.8/


gcc: gawk-3.1.8/malloc.o
	cd gawk-3.1.8/ &&	gcc -c malloc.c

check:
	cd gawk-3.1.8/ && make check

clean:
	rm -f gawk-3.1.8/malloc.*

magic:
	bash edit.sh

com:
	cd gawk-3.1.8/ && echo > .deps/my_malloc.Po

lazy_linked:
	make clean
	make cp_linked
	make gcc
	make configure
	make check
	make magic
	make check

lazy_buddy:
	make clean
	make cp_buddy
	make gcc
	make configure
	make check
	make magic
	make check

correct_linked:
	make clean
	make configure
	make cp_linked
	make magic
	make com
	make check

correct_buddy:
	make clean
	make configure
	make cp_buddy
	make magic
	make com
	make check
