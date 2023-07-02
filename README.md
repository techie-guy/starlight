# mist-lib

Loosely packed Game "Library" written in C

## Build it
**If building on Windows, you need to have a working MSYS2 Installation**

Clone this repository recursively to pull in the submodules

**To Build the Project, You will need:**

* A C Compiler: Clang, GCC or Emscripten
* make
* GLFW3

You can Download them from your package manager

After cloning the repo recursively, `cd` into it. Then use `make` to build the project.

To run the executable use `make run`.

### Customizing the Build Options
Use `make PLATFORM=platform`, where 'platform' is `native` or `web` to change the Build Platform.

Use `make BUILDTYPE=type`, where 'type' is `debug` or `release` to change the Build Type.
