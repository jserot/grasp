# Top-level, platform-independant Makefile

include ../config # All platform-dependent defns are here

CONFIG += console #debug
MAKEFILE=Makefile.$(PLATFORM)

DIST_DIR=../dist

EXT_DLLS=\
	/ucrt64/bin/libb2-1.dll \
	/ucrt64/bin/libbrotlicommon.dll \
	/ucrt64/bin/libbrotlidec.dll \
	/ucrt64/bin/libbz2-1.dll \
	/ucrt64/bin/libdouble-conversion.dll \
	/ucrt64/bin/libfreetype-6.dll \
	/ucrt64/bin/libgcc_s_seh-1.dll \
	/ucrt64/bin/libglib-2.0-0.dll \
	/ucrt64/bin/libgraphite2.dll \
	/ucrt64/bin/libharfbuzz-0.dll \
	/ucrt64/bin/libiconv-2.dll \
	/ucrt64/bin/libicudt74.dll \
	/ucrt64/bin/libicuin74.dll \
	/ucrt64/bin/libicuuc74.dll \
	/ucrt64/bin/libintl-8.dll \
	/ucrt64/bin/libmd4c.dll \
	/ucrt64/bin/libpcre2-16-0.dll \
	/ucrt64/bin/libpcre2-8-0.dll \
	/ucrt64/bin/libpng16-16.dll \
	/ucrt64/bin/libstdc++-6.dll \
	/ucrt64/bin/libwinpthread-1.dll \
	/ucrt64/bin/libzstd.dll \
	/ucrt64/bin/zlib1.dll

GRAPHVIZ_DLLS=\
	/ucrt64/bin/libcgraph-6.dll \
	/ucrt64/bin/libgvc-6.dll \
	/ucrt64/bin/libcdt-5.dll \
	/ucrt64/bin/libpathplan-4.dll \
	/ucrt64/bin/libxdot-4.dll \
	/ucrt64/bin/libexpat-1.dll \
	/ucrt64/bin/libltdl-7.dll \
	/ucrt64/bin/config6 \
	/ucrt64/bin/libgvplugin_pango-6.dll \
	/ucrt64/bin/libgvplugin_dot_layout-6.dll 

all: qmake build run dist

bdist: build dist

qmake: $(APPNAME).pro
	$(QMAKE) -o $(MAKEFILE) $(APPNAME).pro

build: $(MAKEFILE)
	make -f $(MAKEFILE)

run:
ifeq ($(PLATFORM_TYPE), windows)
	./release/$(APPNAME).exe
endif
ifeq ($(PLATFORM_TYPE), macos)
	open ./$(APPNAME).app
endif

dist:
ifeq ($(PLATFORM_TYPE), windows)
	rm -f $(DIST_DIR)/windows/*.exe
	rm -f $(DIST_DIR)/windows/*.dll
	rm -rf $(DIST_DIR)/windows/{generic,imageformats,networkinformation,platforms,styles,tls,translations}
	cp release/$(APPNAME).exe $(DIST_DIR)/windows
	(cd $(DIST_DIR)/windows; $(WINDEPLOYQT) $(APPNAME).exe)
	cp $(EXT_DLLS) $(DIST_DIR)/windows
	cp $(EXT_DLLS) $(DIST_DIR)/windows
	cp $(GRAPHVIZ_DLLS) $(DIST_DIR)/windows
	cp rfsm-light.ini options_spec.txt $(DIST_DIR)/windows
	cp $(RFSMC) $(DIST_DIR)/windows
endif
ifeq ($(PLATFORM_TYPE), macos)
	rm -rf $(DIST_DIR)/macos/$(APPNAME).app
	cp -r $(APPNAME).app $(DIST_DIR)/macos
	cp rfsm-light.ini options_spec.txt $(DIST_DIR)/macos/$(APPNAME).app/Contents/MacOS
	cp $(RFSMC) $(DIST_DIR)/macos/$(APPNAME).app/Contents/MacOS
endif

clean:
	make -f $(MAKEFILE) clean
	rm -f moc_*.*	

clobber: 
	rm -f moc_*.*	
	rm -f $(MAKEFILE)
	rm -rf $(MAKEFILE).{debug,release}
	rm -rf {debug,release}
	rm -rf $(APPNAME).app
	rm -f $(APPNAME).pro.user
	rm -f .qmake.stash
	rm -f *.o
	rm -f *~
