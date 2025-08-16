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
	#(cd doc/um; make clobber)
	\rm -f *~
	# rm -f *~
	# rm -rf ./dist/macos/*.app
	# rm -rf ./dist/macos/*.dmg
	# rm -f ./dist/windows/*.exe
	# rm -f ./dist/windows/*.dll
	# rm -f ./dist/windows/options_spec.txt
	# rm -f ./dist/windows/grasp.ini
	# rm -rf ./dist/windows/{generic,imageformats,networkinformation,platforms,styles,tls,translations}

.PHONY:
.PHONY: dist
