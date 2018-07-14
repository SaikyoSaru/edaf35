configure:
	cd gawk-3.1.8/ && ./configure
	cd gawk-3.1.8/ && $(MAKE) check

linked:
	cd linked/ &&	$(MAKE) clean
	cd linked/ &&	$(MAKE) all

buddy:
	cd buddy/ && $(MAKE) clean
	cd buddy/ &&	$(MAKE) all

run:
	cd gawk-3.1.8/ && $(MAKE) check

clean:
	cd linked/ &&	$(MAKE) clean
	cd buddy/ && $(MAKE) clean
	rm -f gawk-3.1.8/malloc.*
