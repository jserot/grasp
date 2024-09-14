# Building from source

Building from sources is currently supported for (at least) the following platforms :
- Qt5 on MacOS 10.12 running on an Intel (x86) Mac (platform name: `qt5-macx86`)
- Qt6 on Windows10 running MSYS2/ucrt64 development env (platform name: `qt6-ucrt64`)
- Qt6 on MacOS 12.6 running on an M1 Mac (platform name: `qt6-macm1`)

## Pre-requisites

* [ocaml](http://ocaml.org) (version>=5.0) with latest version of the following [opam](http://opam.ocaml.org) packages installed:
  - [dune](http://opam.ocaml.org/packages/dune) (version>=3.11)
  - [rfsm](http://opam.ocaml.org/packages/rfsm) (version>=2.1)
* [Qt](http://www.qt.io) (version>=5.8)

The `graphviz` and `gtkwave` external tools are not required for building but, as for the binary
packages, will be required to view results when running the application.

#### How to build

* Get the source code: `git clone https://github.com/jserot/grasp`
* `cd grasp`
* `./configure -platform <platform name> [other options]` (`./configure --help` for the list of options)
- `cd src`
- `make qmake`
- `make`
- `make run` (for testing)
- `make dist`  (this will build the clickable app in `../dist/<platform>`)

**Note** To build using the `QGV` library (instead of relying on the external `graphviz`) pass the
`-qgvlibdir <qgvlibsir>` option to `configure` (where `qgvlibdir` is the directory where the library has been installed)

**Note** If you can't or don't want to build the documentation from source, pass the `--no-doc` option to
`configure`. A pre-built version of the documentation is available
[here](https://github.com/jserot/grasp/blob/master/doc/using.md).
