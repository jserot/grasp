Problems can be diagnosed by launching the application from a command line interpreter
("console", "terminal", ...) and inspecting the trace/debug messages.
- under Windows
  - open a terminal (console, PowerShell, Cygwin, MSYS, ...)
  - cd to the directory where the `Grasp` application has been installed
  - launch it from the command line
- under MacOS
  - open a terminal (`Applications/Tools/Terminal.app`)
  - cd to the directory where the `Grasp` application has been installed
  - launch it from the command line

The most common problems are due to wrong locations of the `dot` and `gtkwave` external tools
(used, respectively, to display to FSM diagrams and the simulation results). Check the paths
by choosing the `Compiler and Tools` in the `Configuration` menu. 
