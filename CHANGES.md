## Changes

# 1.1.0 (unreleased)

* Stimuli are now either sporadic (for event inputs) or value changes (for `int` and `bool` inputs).
  Periodic stimuli found in files created by previous versions are converted to sporadic ones when
  read, and saved as such. Files written by this version can still be read by previous versions
* Stimuli editor: inputs with no stimulus can now be given one, dates are rounded to the nearest
  integer instead of being truncated, values of `bool` inputs are restricted to 0/1, and the model
  is marked as modified after editing
* Fixed crash when typing the name of a new input
* Fixed outputs and global variables being created as inputs
* Fixed IO names being lost when the model was saved before the name field lost focus
* Fixed the check for duplicate IO names, which never fired
* Fixed type lists not being displayed on macOS when a warning was issued at startup
* Stimuli editor: fixed double-clicks being handled twice (duplicate dialogs) and edit/remove dialogs
  acting on the wrong stimulus when the track was modified while they were opened
* The state, transition and diagram properties dialogs are now modal
* Deleting an input event, or changing its type, is refused when it triggers transitions. Renaming an
  input event renames it in the transitions it triggers
* State names are now unique in each diagram (new states could get the name of an existing state
  after a file was read, and one of them was then lost when the file was read back). Renaming a
  state or a diagram rejects empty, invalid or already used names. Diagram names are kept unique
* The initial pseudo-state can no longer be edited, nor used as the source or destination of a
  regular transition
* Unsaved changes: cancelling the save prompt now actually cancels New, Open and Quit, closing the
  window asks to save the changes, a failed save is reported, and moving states or renaming a
  diagram marks the model as modified
* Fixed general compiler options (such as `-synchronous_actions`) never being passed to the compiler,
  and compiler options with a value (such as `-stop_time`) making the compilation fail
* More robust communication with the compiler server (answers arriving in several parts or after
  their timeout, longer timeout for compilation and simulation, server not running). The reason why
  the server cannot be started (e.g. port already used by another instance) is now reported
* Compiler paths dialog: the Save button saves what has been typed, the default VCD viewer is
  `gtkwave`, and a leading `~` is expanded in paths
* Fixed several crashes: deleting the initial transition, deleting a state after changing the end
  state of one of its transitions, adding transitions in a new diagram, checking or duplicating the
  current diagram when all diagram tabs are closed, reading a malformed compiler options file
* Fixed several memory leaks (one diagram view was leaked per closed diagram and per New/Open model)
* An existing transition is no longer deleted when it is edited while no input event is defined
* Empty rows in the valuations, guards and actions panels are ignored (they produced invalid RFSM
  code when model checking was disabled)
* No `.fsm` file is written (and no compilation is started) when the RFSM code cannot be generated
  (invalid diagram, missing stimulus), instead of a partially written file
* Compiling a model which has never been saved now saves it first
* A name being typed (an input name, for example) is now taken into account when saving, checking,
  exporting or quitting without leaving the field first (renaming an input event while saving could
  write a file where the input and the transitions it triggers had different names)
* Clicking a state without moving it no longer marks the model as modified
* When the compiler server is not running, each check now reports it (only the first one did)
* Transitions are much easier to select (or delete): they can be clicked on their label, their arrow
  head or near their line (a right click next to a transition often opened the diagram properties)
* The editing cursors now act where they point: the delete cross deletes the item under its center
  (clicks were taken up-left of it), and new states are created under the state cursor
* Diagrams are now centered in their view when a model is opened and when the scale is changed. The
  scale list of the diagram properties shows the current scale and has a new "Fit" entry, showing
  the whole diagram
* The windows showing the compilation results (generated code, DOT graphs) are now sized after their
  content, within 80% of the screen, with scroll bars beyond (they had a fixed size)
* The stimuli editor window is now sized after the number of model inputs (one band per input) and
  the whole time range, within 80% of the screen, with scroll bars beyond (it had a fixed size)
* In the Inputs panel of the model properties, each input row has a "Stim" button ("-" when no stimulus
  is attached to the input), opening the stimuli editor with the track of this input highlighted. It
  replaces the global STIM button
* Changing the type of an input which has stimuli now deletes them, after confirmation (they were kept
  unchanged, a sequence of events attached to an int input for instance)
* The names of states, diagrams, IOs and variables, and the periodic fill parameters of the stimuli
  editor, are now checked as they are typed: an invalid field gets a red frame, the error is displayed
  under it and the dialog cannot be accepted (errors were reported by message boxes)
* A name already used by another IO or variable is no longer applied: the previous name is restored
* In the transition and state properties, all the guards, actions and valuations are checked when
  the dialog is accepted, the faulty rows being marked in red with their errors listed under the panel
  (only the first error was reported, by a message box). An action is now checked against the target
  state selected in the dialog
* A comma is no longer accepted in a guard, an action or a valuation (they are saved as comma-separated
  lists in .fsd files: an item containing a comma was split in two when the model was read again)
* The application version is now read from the `VERSION` file

# 1.0.3 (Mar 5, 2026)

* Fixed bug in the handling of the `rfsm` server responses (removed `\n` in phrases)
* Application now requires version `>= 2.3.1` of `rfsm`

# 1.0.2 (Jan 25, 2026)

* Fixed issue #1 (app crashes when typing backspace in text fields)

# 1.0.1 (Dec 20, 2025)

* Switch to TCP sockets for communicating with the rfsmc server in preparation of Windows port (the
  mininum version for the `rfsmc` compiler is now `2.3`)
* Fixed the build process for the Windows (10) version
* Renamed `grasp.ini` file (holding the paths to the auxilliary tools) to `grasp.conf` and updated
  the location of this file according to the target platform to ensure that it can both read and
  written by the user
* Removed the `DOTVIEWER` path from the `.conf` file and corresponding app menu. `.dot` files are
  now systematically displayed by converting them to `gif` using the `dot` program
* The `macOS` installer is now built using `macdeployqt`

# 1.0.0 (Nov 11, 2025)

* Initial version, forked from branch `2.0.0` of the `rfsm-light` project

