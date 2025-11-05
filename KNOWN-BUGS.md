Known bugs and limitations
--------------------------

* Multi-events FSM models are not supported by the VHDL backend

* Synchronous interpretation of actions is not (yet) supported by the CTask and SystemC backends

* Types are limited to `event`, `int` and `bool` (sized ints available in the "full" RFSM language
  are not supported in particular)

* The concept of transition priority used in RFSM is not supported; as a result, simulation may fail due to
  non-deterministic situations.

* A portable way of building on Linux distros is still missing

* Under Linux, the `-dot-external-viewer` relies on an working DOT viewer application. None seems to be working / supported currently for LinuxMint (`dotty`, `xdot`, ...) 
