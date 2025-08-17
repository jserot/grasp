Known bugs and limitations
--------------------------

* Multi-events FSM models are not supported by the VHDL backend

* Synchronous interpretation of actions is not (yet) supported by the CTask and SystemC backends

* Types are limited to `event`, `int` and `bool` (sized ints available in the "full" RFSM language
  are not supported in particular)

* The concept of transition priority used in RFSM is not supported; as a result, simulation may fail due to
  non-deterministic situations.

* A portable way of building on Linux distros is still missing

* When running on a Mac M1 under Mac OS 12 - but this may also occur on other versions - when the
  Grasp app is launched by double-clicking, it seems unable to launch the gtkwave application. This does _not_ happen 
  when the app is launched from the terminal (by invoking
  `/Applications/Grasp.app/Contents/MacOS/Grasp` or `open /Applications/Grasp.app`).

* Under Linux, the `-dot-external-viewer` relies on an working DOT viewer application. None seems to be working / supported currently for LinuxMint (`dotty`, `xdot`, ...) 
