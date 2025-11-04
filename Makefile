# Top-level, platform-independant Makefile

include ./config # All platform-dependent defns are here

MAKEFILE=Makefile.$(PLATFORM)

build:
	(cd src; qmake; make)

run:
	make -f $(MAKEFILE) run

dist:
	make -f $(MAKEFILE) dist

install:
	make -f $(MAKEFILE) install

installer:
	make -f $(MAKEFILE) installer

clean:
	make -f $(MAKEFILE) clean

clobber: 
	make -f $(MAKEFILE) clobber
	\rm -f *~

.PHONY:
.PHONY: dist
