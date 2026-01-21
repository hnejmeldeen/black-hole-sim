

Last Updated - Jan. 20 2026

------------------------------------------------------------------------------------------------

Overview:

Polyrender is a 3D graphics rendering software written in C++ using OpenGL and GLFW. It takes a simple user-provided text file as input and produces a rendering of 3D shapes.


User provides a .txt file containing shape definitions and transformation parameters (see examples).

The program parses the file and builds vertex data.

Shapes are rendered using OpenGL with the specified rotations, translations, and scalings applied.

------------------------------------------------------------------------------------------------

Prerequisites:

 - C++17 compiler

 - At least CMake version 3.16

------------------------------------------------------------------------------------------------

Build Black Hole Sim. with CMake:

The following is an example using the command line version of CMake.

```bash
$ cd bh-sim
$ mkdir build
$ cd build
$ cmake ..
$ cmake --build .
```

To launch, use the following from the command line:

```bash
$ ./bh-sim
```

------------------------------------------------------------------------------------------------

Controls:

1. Click to select object/black hole (ESC to cancel selection).

2. Use arrow keys to move camera/objects (hold SHIFT to move faster).

3. Use mouse wheel to zoom in/out, adjust black hole mass, adjust object radius.

