include ../../config

CWD=`pwd`

ifeq ($(VERSION),)
	VOLUME_NAME=$(APPNAME)
else
	VOLUME_NAME=$(APPNAME)-$(VERSION)
endif

installer:
	/C/Program\ Files\ \(x86\)/Inno\ Setup\ 6/iscc $(APPNAME).iss
