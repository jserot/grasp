## Changes

# 1.2.0 (Nov 24, 2026)

* Grasp no longer needs external tools to display diagrams and simulation results :
  * the current diagram can be exported as an image (PNG or PDF), with the `Export current diagram as
    image` action of the `Model` menu (or the right-click menu of the diagram). When the `dot` program
    (Graphviz) cannot be found, the DOT actions propose this export
  * the whole model can be exported with the `Export model as images` action of the `Model` menu,
    either in a file per diagram or in a single file showing all the diagrams side by side (each one
    framed, with its name and, optionally, its local variables; the global variables can be shown in a
    separate frame)
  * simulation results are displayed in a built-in viewer (chronograms with zoom and a cursor showing
    the values at a given time). The signals can be reordered by dragging their names, and this order
    saved in a `.gtkw` file (also read by gtkwave). gtkwave can still be used, by setting the new
    `-vcd_external_viewer` option (`Sim` tab of the compiler options)

# 1.1.1 (Nov 23, 2026)

* Keyboard shortcuts in diagrams: [Delete] deletes the selected items, [Escape] goes back to the
  select mode, [Return] edits the selected item
* Diagrams can be zoomed with [Cmd]+wheel ([Ctrl]+wheel) or the pinch gesture, and moved by dragging
  their background (in select mode)
* New File/Open recent menu
* Undo / redo of all the editing operations (new Edit menu, [Cmd]+Z / [Cmd]+[Shift]+Z)

# 1.1.0 (Nov 16, 2026)

* Added a graphical editor for attaching stimuli to inputs. The editor is invoked by clicking  the
  `Stim` button associatdto to each input (or `-` when no stimulus has been attached to the input)
* Properties of the current diagram (name, local variables) are now shown in a "Diagram" panel, next
  to the model panel (renamed from "Model IOs" to "Model"), instead of a dialog opened by right-clicking
  on the diagram. Right-clicking on the background of a diagram now shows this panel and opens a context
  menu (zoom, new, duplicate, delete and check diagram)
* State and transition properties dialogs are now modal
* Compiling a model which has never been saved now saves it first
* Transitions are much easier to select (or delete): they can be clicked on their label, their arrow
  head or near their line (a right click next to a transition often opened the diagram properties)
* The editing cursors now act where they point: the delete cross deletes the item under its center
  (clicks were taken up-left of it), and new states are created under the state cursor
* Diagrams are now centered in their view when a model is opened and when the zoom is changed. The
  zoom menu of a diagram shows the current scale and has a new "Fit" entry, showing the whole diagram
* The windows showing the compilation results (generated code, DOT graphs) are now sized after their
  content, within 80% of the screen, with scroll bars beyond (they had a fixed size)
* The names of states, diagrams, IOs and variables, and the periodic fill parameters of the stimuli
  editor, are now checked as they are typed: an invalid field gets a red frame, the error is displayed
  under it and the dialog cannot be accepted (errors were reported by message boxes)
* In the transition and state properties, all the guards, actions and valuations are checked when
  the dialog is accepted, the faulty rows being marked in red with their errors listed under the panel
  (only the first error was reported, by a message box). An action is now checked against the target
  state selected in the dialog
* Commas are no longer accepted in a guard, an action or a valuation
* The generated VHDL and SystemC files now have their own syntax highlighting (SystemC files got the
  CTask one, VHDL files had none). All the occurrences of a keyword in a line are now highlighted (only
  the first one was)
* Numerous bug fixes (too long to be listed here)

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

