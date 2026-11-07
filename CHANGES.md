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

