* GENERAL

* GUI
- fix the `overlaping` and `spurious name editing` bugs  (see `KNOWN-BUGS`)
- restrict the set of IOs of each automaton to those _actually occuring_ in the transitions (instead
  of using the full set of global IOs); for this we'll need an extension of the `fragment checker`
  mechanism returning all read/write variables occuring in a (set of) transition(s)
- use in-bar editing (instead of dialog) to change automatons name (see:
  `https://forum.qt.io/topic/108553/qml-editable-tab-title-within-tabview-on-double-click` for ex)
- add sized ints
- add type_coercions in accepted expressions
- Allow attachement of priorities to transitions
- VHDL and SystemC syntax highlighters
- Allow resizing of state boxes ?
- Export to SCXML ?

* BUILD
- update icons for the Windows distro

* TOOLS
- implement a "server" mode for the `rfsmc` compiler, allowing it to be used for
  checking / extracting semantic informations from syntax fragments (thus generalizing the
  `-check_fragment` option currently used by the `fragmentChecker` class 
- replace `-dot_no_caption` option by `-dot_caption`  (reverse default behavior)

* DOC
- uodate `README.md`
- update `./doc` files
- video tutorials in french and english
- update Windows Installer Guide
