Known bugs and limitations
--------------------------

* Multi-events FSM models are not supported by the VHDL backend

* Synchronous interpretation of actions is not (yet) supported by the CTask and SystemC backends

* Types are limited to `event`, `int` and `bool` (sized ints available in the "full" RFSM language
  are not supported in particular)

* The concept of transition priority used in RFSM is not supported; as a result, simulation may fail due to
  non-deterministic situations.

* A portable way of building on Linux distros is still missing

* When running on a Mac M1 under Mac OS 12 - but this may also occur on other versions - when the Rfsm-Light, 
  app is launched by double-clicking, it seems unable to launch the gtkwave application. This does _not_ happen 
  when the app is launched from the terminal (by invoking
  `/Applications/Rfsm-light.app/Contents/MacOS/rfsm-light` or `open /Applications/Rfsm-light.app`).

* When running on a Mac M1 under Mac OS 12 - but this may also occur on other versions - 
  the right-click emulation sequence on a trackpad (two fingers + click) on an item to
  edit it triggers a bug (the GUI becomes essentially non-responsive).
  Here's a sequence to reproduce the bug :
  - start a new design
  - add a new state, validate it with `Done`
  - select the `select` (arrow) tool and right-click on the state
  - hit `Cancel`
  It is now impossible to change to tool, add an input, etc (keyboard-triggered actions like Save,
    ... are still operational though)
   *Note 1* : this bug does _not_ show if item editing is performed by using the `Ctl+Click` sequence 
   *Note 2* : when this bug occurs, it is possible to revover by performing a right-click (either
    with `Ctl+Click` or two-fingers+click on the menu bar (which brings a pop-up menu)

* On Windows, the version of the application using the QGV library does not work well

* There's a bug in the handling of layouts in the middle ("automaton") panel. It shows in two ways
  i) when the "local variables" sub-panel is too big (high) it overlaps the graphic canvas subpanel
  just above
  ii) when the graphic canvas is scrolled after a model has been loaded, a subsequent "new" or
  "duplicate" automaton action, spuriously affects the first input declared in the right panel (!). 
  To reproduce, for example: open a existing model and scroll using the vertical slider to show the upper part of
    the automaton view; then add a new automaton tab. An error message pops up telling
    that "the name xxx is already in used [...]". The name of the first global input is then erased. 
    Somehow, the `edited` slot associated to the corresponding QLineEdit has been called. How can
    this happen ?! Strangely
    i) this does not occur if the automaton view (QGraphicsScene) is _not_ scrolled before duplicating the automaton
    ii) a small, grayed, rectangle appears in the upper left part of the QGraphicSCene 
    iii) the bug does not occur if the first automaton is entered "by hand" (and not read from a file)
