# Viewer

This is a 3D model viewer.

## Prerequisites

To build and run the project, you need CMake and a proper build environment
(e.g. GCC, Visual Studio, Xcode, ...).

For Unix type systems (Linux, Mac OS X, FreeBSD, ...), run the script:

```bash
build/install-deps.sh
```

It should install the necessary tools and libraries (or tell you what you need
to install).

For Windows, you should be able to use [Visual Studio](https://www.visualstudio.com/)
or  [mingw-w64](http://mingw-w64.org/) (or similar), together with
[CMake](https://cmake.org/).

## Building

Using CMake, you should make an out-of-tree build. Here are some examples:

### Linux

To make a release build using [Ninja](https://ninja-build.org/):

```bash
mkdir out_release
cd out_release
cmake -GNinja -DCMAKE_BUILD_TYPE=Release ../src
ninja
```

...or a Debug build using make:

```bash
mkdir out_debug
cd out_debug
cmake -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug ../src
make -j10
```

### Windows

To make a Visual Studio 2015 solution for 64-bit targets:

```bash
mkdir out_vs2015
cd out_vs2015
cmake -G"Visual Studio 14 2015 Win64" ../src
```

You can open the solution in Visual Studio, and select the desired build type
(e.g. Debug or Release).

If you are using MinGW (or some other GCC variant for Windows), you can create
a debug or release build similar to how you would do it on Linux.

### Mac OS X

To make an Xcode project:

```bash
mkdir out_xcode
cd out_xcode
cmake -GXcode ../src
```

You can open the project in Xcode, and select the desired build type (e.g.
Debug or Release) by editing the scheme.
