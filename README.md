saedb
=====

Engine

Directory Structure
===================

Directories are structured like follows:

	src
	|-- app
	|   |-- basic
	|   `-- demo
	|       `-- pagerank
	`-- saedb
		`-- sae_include.hpp

Please add your applications to the `app` directory with corresponding `CMakeLists.txt`.

Build
=====

This project requires a C++11 capable compilation environment, such as g++ 4.7, clang 3.2 or VS 2012.

Please install `cmake` to generate your project files. The default is `Makefile`.

To build the project, go to the root of this project, execute the following commands:

	mkdir build
	cd build
	cmake ..
	make

And sample applications should appear in the `build` directory.

Developer's Guide
=================

To develop and cotribute to this project, please follow the [Developer's Guide](https://github.com/THUKEG/saedb/wiki/Developer's-Guide).
