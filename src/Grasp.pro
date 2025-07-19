!include(../config) { error("Cannot open config file. Run configure script in top directory") }

CONFIG+=sdk_no_version_check
QT       += widgets

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

RESOURCES = resources.qrc

# install
target.path = .
INSTALLS += target
