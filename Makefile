# Top-level, platform-independant Makefile

include ./config # All platform-dependent defns are here

MAKEFILE=Makefile.$(PLATFORM)

clean:
	(cd src; make -f $(MAKEFILE) clean)

clobber: 
	(cd src; make clobber)
	rm -f *~
	rm -rf ./dist/macos/*.app
	rm -rf ./dist/macos/*.dmg
	rm -f ./dist/windows/*.exe
	rm -f ./dist/windows/*.dll
	rm -f ./dist/windows/options_spec.txt
	rm -f ./dist/windows/grasp.ini
	rm -rf ./dist/windows/{generic,imageformats,networkinformation,platforms,styles,tls,translations}
