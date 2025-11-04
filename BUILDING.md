# Building from source

Building from sources is currently supported for (at least) the following platforms :
- MacOS 14.7 (Sonoma) with Qt6 (platform name: `macos`)
- LinuxMint 6.8 with Qt6) (platform name: `linux`)
- Windows10 running MSYS2/ucrt64 development env and Qt6 (platform name: `windows`)

## Pre-requisites

* [Qt](http://www.qt.io) (version>=6.9)

The `graphviz` and `gtkwave` external tools are not required for building but, as for the binary
packages, will be required to view results when running the application.

#### How to build

* Get the source code: `git clone https://github.com/jserot/grasp`
* `cd grasp`

#### To build and test without installing 

* `./configure -draft -platform <platform_name> [other options]` (`./configure --help` for the list of options)
- `make`
- `make install` (for Linux - this will install binaries in `<install_dir>/bin` and config files and
  examples in `<install_dir>/share/grasp`)
- `make run` (for testing)

#### To build and create the self-installers (for MacOS and Windows)

* `./configure -platform <platform_name> [other options]` (`./configure --help` for the list of options)
- `make`
- `make install` 
- `make installer` (this will put in `./binaries`)

**Note** If you can't or don't want to build the documentation from source, pass the `--no-doc` option to
`configure`. A pre-built version of the documentation is available
[here](https://github.com/jserot/grasp/blob/master/doc/using.md).
