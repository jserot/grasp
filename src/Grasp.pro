!include(../config) { error("Cannot open config file. Run configure script in top directory") }

QT       += core widgets gui

QMAKE_PROJECT_NAME = rfsmlight
QMAKE_MACOSX_DEPLOYMENT_TARGET = 12.6

TARGET = $$APPNAME
TEMPLATE = app

equals(USE_QGV,"yes") {
message("Building with QGV support")
message($$QGVDIR)
INCLUDEPATH += $$QGVLIBDIR
LIBS += -L$$QGVLIBDIR -lQGVCore
DEPENDPATH += $$QGVLIBDIR
QMAKE_CXXFLAGS += -DUSE_QGV
} else {
message("Building without QGV support")
}


!include(./GraphViz.pri) { error("Cannot open GraphViz.pri file") }

HEADERS += include/nlohmann_json.h \
           globals.h \
           transition.h  \
           state.h  \
           iov.h  \
           nameInputDialog.h  \
           automaton.h  \
           automatonPanel.h  \
           model.h  \
           commandExec.h \
           compiler.h \
           fragmentChecker.h \
           dynamicPanel.h \
           stateValuations.h \
           stateProperties.h \
           transitionGuards.h \
           transitionActions.h \
           transitionProperties.h \
           iovPanel.h \
           modelPanel.h \
           stimulus.h \
           stimuli.h \
           command.h \
           imageviewer.h \
           textviewer.h \
           syntaxHighlighters.h \
           compilerPaths.h \
           compilerOption.h \
           compilerOptions.h \
           debug.h \
           mainwindow.h
SOURCES += transition.cpp \
           globals.cpp \
           state.cpp \
           iov.cpp  \
           nameInputDialog.cpp  \
           automaton.cpp \
           automatonPanel.cpp  \
           model.cpp \
           commandExec.cpp \
           compiler.cpp \
           fragmentChecker.cpp \
           dynamicPanel.cpp \
           stateValuations.cpp \
           stateProperties.cpp \
           transitionGuards.cpp \
           transitionActions.cpp \
           transitionProperties.cpp \
           iovPanel.cpp \
           modelPanel.cpp \
           stimulus.cpp \
           stimuli.cpp \
           command.cpp \
           syntaxHighlighters.cpp \
           compilerPaths.cpp \
           compilerOption.cpp \
           compilerOptions.cpp \
           textviewer.cpp \
           imageviewer.cpp \
           debug.cpp \
           main.cpp \
           mainwindow.cpp
equals(USE_QGV,"yes") {
HEADERS += dotviewer.h
SOURCES += dotviewer.cpp
}

RESOURCES += resources.qrc
