## Grasp 

![](./doc/snapshot.png)
![](./doc/wave.png)

**Grasp** is a simplified version of the [RFSM](https://github.com/jserot/rfsm) State Diagram
simulator and compiler. 

The main difference is the ability to describe state diagrams graphically using an
interface similar to that used in the [SSDE](https://github.com/jserot/ssde) editor.

On the other hand, and contrary to *RFSM*, **Grasp** only handles systems composed of a *single
FSM* and hence does not support the abstraction facilities offered by the full RFSM language.
 
Apart from these differences, the underlying concepts and capabilities of the tools are similar.  In
particular, starting from a model of a system as a *reactive finite state machine* &mdash; *i.e.* a
set of *states* connected by *transitions* triggered by an *event* and a set boolean *guards* and
triggering a sequence of *actions* &mdash; *Grasp* can

- generate a DOT representation of the model (to be viewed with [Graphviz](http://www.graphviz.org) for example)

- simulate the behavior of the model in response to stimuli attached to inputs (producing `.vcd`
files to be viewed using the [gtkwave](http://gtkwave.sourceforge.net) software

- generate description of the system (including *testbench* code when stimuli are attached to
  inputs) in

    - `CTask` (a C dialect with primitives for describing event-based synchronisation)

    - `SystemC`

    - `VHDL` 

## DOCUMENTATION

A minimalistic "Howto" is available
[here](https://github.com/jserot/grasp/blob/master/doc/using.md)

This sequence of short videos can be used as a tutorial
1. [Tutorial part 1](https://github.com/jserot/grasp/releases/download/1.3.1/Light-v1.mov)
2. [Tutorial part 2](https://github.com/jserot/grasp/releases/download/1.3.1/Light-v2.mov)
3. [Tutorial part 3](https://github.com/jserot/grasp/releases/download/1.3.1/Light-v3.mov)
4. [Tutorial part 4](https://github.com/jserot/grasp/releases/download/1.3.1/Light-v4.mov)

## INSTALLATION

### Using binary versions

Prebuilt Windows and MacOS (Intel and M1) versions can be downloaded
[here](https://github.com/jserot/grasp/releases) (check the `Assets` tab).

For Windows, a quick Install Guide is available
[here](https://github.com/jserot/grasp/blob/master/dist/windows/windows-install-guide.pdf)

**Note** the [Graphviz](http://www.graphviz.org) package and the [gtkwave](http://gtkwave.sourceforge.net)
application (used, respectively, to display diagrams and viewing simulation results) are _not_
included in the distribution and must be installed separately. If you cannot (or don't want) to install
the `Graphviz` package, versions of **Grasp** using the [QGV](https://github.com/nbergont/qgv)
library instead are provided (the corresponding files have the `_qgv` suffix in the [corresponding
directory](https://github.com/jserot/grasp/releases). These versions are provided only as
workarounds because the rendering is poorer than when using the external `Graphviz` package. 

### Building from source 

See [this file](https://github.com/jserot/grasp/blob/master/BUILDING.md)
