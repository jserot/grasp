#!include(../config) { error("Cannot open config file. Run configure script in top directory") }

CONFIG   += debug
QT       += widgets

HEADERS += include/nlohmann_json.h \
           globals.h \
           transition.h  \
           state.h  \
           iov.h  \
           diagram.h  \
           diagramProperties.h  \
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
           modelProperties.h \
           stimulus.h \
           stimuli.h \
           command.h \
           imageViewer.h \
           textViewer.h \
           textsViewer.h \
           textProperties.h \
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
           diagram.cpp \
           diagramProperties.cpp  \
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
           modelProperties.cpp \
           stimulus.cpp \
           stimuli.cpp \
           command.cpp \
           syntaxHighlighters.cpp \
           compilerPaths.cpp \
           compilerOption.cpp \
           compilerOptions.cpp \
           textViewer.cpp \
           textsViewer.cpp \
           textProperties.cpp \
           imageViewer.cpp \
           debug.cpp \
           main.cpp \
           mainwindow.cpp

RESOURCES = resources.qrc

# install
target.path = .
INSTALLS += target
