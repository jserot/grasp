# Building from source

Building from sources is currently supported for (at least) the following platforms :
- MacOS 12.6 with Qt6 (platform name: `macos`)
- LinuxMint 6.8 with Qt6) (platform name: `linux`)
- Windows10 running MSYS2/ucrt64 development env and Qt6 (platform name: `windows`)

## Pre-requisites

* [Qt](http://www.qt.io) (version>=6.9)
* [ocaml](http://ocaml.org) (version>=5.0) 

The `graphviz` and `gtkwave` external tools are not required for building but, as for the binary
packages, will be required to view results when running the application.

#### How to build

* Get the source code: `git clone https://github.com/jserot/grasp`
* `cd grasp`
* `./configure -platform <platform_name> [other options]` (`./configure --help` for the list of options)
- `cd src`
- `qmake`
- `make`
- `make run` (for testing)
- `make dist`  (this will build the clickable app in `../dist/<platform>`)

**Note** If you can't or don't want to build the documentation from source, pass the `--no-doc` option to
`configure`. A pre-built version of the documentation is available
[here](https://github.com/jserot/grasp/blob/master/doc/using.md).
