# starlight

Loosely packed Game "Sandbox" written in C

Font used: [Monocraft](https://github.com/IdreesInc/Monocraft)

## Building the Project
**If building on Windows, you need to have a working MSYS2 Installation**

Clone this repository recursively to pull in the submodules

**To Build the Project, You will need:**

* A C and C++ Compiler: Clang, GCC or Emscripten
* Premake5
* GLFW3
* make

You can Download them from your package manager

After cloning the repo recursively, `cd` into it.

Then, use `sh generate-source-files.sh` to generate some source files

Use `premake5 gmake2` to generate the project build files.

Use `make help` to get a list of the available configurations.

Use `make config=CONFIG` to build the project for the required Platform.

The executable is in `bin/CONFIG/starlight/starlight`,

## Android Support
### Using Gradle
Use `sh generate-source-files.sh && premake5 androidndk` to generate the build files

Then `cd` into `android-project` and use `gradle build`

### Build it directly (Doesn't work!)
Use `premake5 androidndk` to generate the build files

Then, use `ndk-build NDK_PROJECT_PATH=./ APP_BUILD_SCRIPT=starlight.wks.Android.mk PREMAKE_ANDROIDNDK_CONFIGURATIONS=Debug PREMAKE_ANDROIDNDK_PLATFORMS:=Android APP_PLATFORM=android-30` to build the .so file
