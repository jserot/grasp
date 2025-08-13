* GENERAL

* GUI
- add an option to set the location of the log file when debug mode is set
- enlarge the compiler path popup window
- remove the "Dump model" action (only for debug)
- add a "no check" option for by-passing the RFSM fragment checker
- after adding or deleting an item, reset the tool to the pointer ("select") mode ?
- automatically size the textviewer's popup according to the displayed text (see `textviewer.cpp`)
- restrict the set of IOs of each automaton to those _actually occuring_ in the transitions (instead
  of using the full set of global IOs); for this we'll need an extension of the `fragment checker`
  mechanism returning all read/write variables occuring in a (set of) transition(s)
- add sized ints
- add type_coercions in accepted expressions
- Allow attachement of priorities to transitions
- VHDL and SystemC syntax highlighters
- Export to SCXML ?

* BUILD
- update icons for the Windows distro

* TOOLS
- implement a "server" mode for the `rfsmc` compiler, allowing it to be used for
  checking / extracting semantic informations from syntax fragments (thus generalizing the
  `-check_fragment` option currently used by the `fragmentChecker` class.
  Use `QLocalSocket` (like in example `../socket/localfortune/client`) on the `Grasp` side.
  Use `Caml` implementation of Unix socket on the server (`rfsmc`) side.
  Q: The former will be automatically portable to Windows. How about the second ??
- replace `-dot_no_caption` option by `-dot_caption`  (reverse default behavior) ?

* DOC
- uodate `README.md`
- update `./doc` files
- video tutorials in french and english
- update Windows Installer Guide
