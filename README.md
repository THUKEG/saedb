SAE
===

The Core Library of Social Analytic Engine.

Requirements
============

We do regular developments with Linux and Mac OS X environments. Here's a list of our prerequists:

	CMake 2.8+
	g++ 4.8 or clang 3.3
	protobuf 2.4+ (for RPC)
	zeromq 3.2+ (for PRC)

On Linux, it's easy to get those softwares and libraries with your distribution's package manager.

On Mac OS X, please install the lastest XCode Command Line Tools and homebrew. Then you can install protobuf and zeromq with brew. We use libc++ by default on Mac OS X.

Build
=====

We recommend to build the project outside the source tree. To do this, please go to the root of this project and execute the following commands:

	mkdir build
	cd build
	cmake ..
	make

Then you can do some testing with:

	ctest

If something goes wrong with your build system, it's easy to delete the `build` directory and restart.

Developer's Guide
=================

To develop and cotribute to this project, please follow the [Developer's Guide](https://github.com/THUKEG/saedb/wiki/Developer's-Guide).
