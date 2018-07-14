configure:
	cd gawk-3.1.8/ && ./configure

linked: linked/malloc.o
	make clean
	cd linked/ &&	gcc -c malloc.c

buddy: buddy/malloc.o
	make clean
	cd buddy/ &&	gcc -c malloc.c

check:
	cd gawk-3.1.8/ && make check

clean:
	rm -f linked/*.o
	rm -f buddy/*.o
	rm -f gawk-3.1.8/malloc.*

magic:
	bash edit.sh

lazy_linked:
	make configure
	make check
	make linked
	cp linked/malloc.* gawk-3.1.8/
	make configure
	make magic
	make check

lazy_buddy:
	make configure
	make check
	make buddy
	cp buddy/malloc.* gawk-3.1.8/
	make configure
	make magic
	make check
