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

