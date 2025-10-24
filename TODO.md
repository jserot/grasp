* GENERAL

* GUI
- remove socket file at closing (either in `MainWindow` or `Compiler`)
- fix `Diagram::exportRfsmInstance()` and `Diagram::exportRfsmModel()` so that not all global IOs
  are used as instance parameters; each instance model should be able to use a subset of the global
  IOs and this subset should be computed from the rd/wr variable sets derived from the transition rules
- restrict the set of IOs of each automaton to those _actually occuring_ in the transitions (instead
  of using the full set of global IOs); for this we'll need an extension of the `fragment checker`
  mechanism returning all read/write variables occuring in a (set of) transition(s)
- share code for checking transitions btw `diagram.cpp` and `transitionProperties.cpp`
- add an option to set the location of the log file when debug mode is set
- use json format for options save file
- enlarge the compiler path popup window
- remove the `Dump model` action (only for debug)
- after adding or deleting an item, reset the tool to the pointer ("select") mode ?
- automatically size the textviewer's popup according to the displayed text (see `textviewer.cpp`)
- add sized ints
- add type_coercions in accepted expressions
- Allow attachement of priorities to transitions
- VHDL and SystemC syntax highlighters
- Export to SCXML ?

* BUILD
- update icons for the Windows distro

* TOOLS
- replace `-dot_no_caption` option by `-dot_caption`  (reverse default behavior) ?

* DOC
- uodate `README.md`
- update `./doc` files
- video tutorials in french and english
- update Windows Installer Guide
