

Last Updated - Jan. 20 2026

------------------------------------------------------------------------------------------------

Overview:

An interactive real-time black-hole simulator. Light rays are traced by integrating null geodesics in Schwarzschild spacetime (non-rotating, spherically symmetric mass). Visualizes basic relativistic lensing and time-dilation effects on interactive objects and the central mass.

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

