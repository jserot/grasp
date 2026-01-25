## Changes

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

