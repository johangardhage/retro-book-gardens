# retro-book-gardens

The Gardens of Imagination raycasting engine, brought back from the 90's.

![Screenshot](/screenshots/raycast.png "raycast")
![Screenshot](/screenshots/raycastlight.png "raycastlight")

## Prerequisites

To build the demo programs, you must first install the following tools:

- [Meson](https://mesonbuild.com/)
- [Ninja](https://ninja-build.org/)
- [GCC](https://gcc.gnu.org/) or [Clang](https://clang.llvm.org/)
- [SDL2](https://www.libsdl.org/)

### Install dependencies

#### openSUSE

`$ sudo zypper install meson ninja gcc-c++ libSDL2-devel`

#### Ubuntu

`$ sudo apt install meson ninja-build g++ libsdl2-dev`

#### macOS

`$ brew install meson ninja sdl2`

## Build instructions

To build the demo programs, run:

```
$ meson setup build
$ ninja -C build
```

The `build` directory will contain the demo programs.

## Usage

```
Usage: raycast [OPTION]...

Options:
 -h, --help           Display this text and exit
 -w, --window         Render in a window
     --fullwindow     Render in a fullscreen window
 -f, --fullscreen     Render in fullscreen
 -v, --vsync          Enable sync to vertical refresh
     --novsync        Disable sync to vertical refresh
 -l, --linear         Render using linear filtering
     --nolinear       Render using nearest pixel sampling
 -c, --showcursor     Show mouse cursor
     --nocursor       Hide mouse cursor
     --showfps        Show frame rate in window title
     --nofps          Hide frame rate
     --capfps=VALUE   Limit frame rate to the specified VALUE
```

## License

Licensed under MIT license. See [LICENSE](LICENSE) for more information.

## Authors

Original code written by Christopher Lampton for [Gardens of Imagination](https://www.amazon.com/Gardens-Imagination-Programming-Maze-Games/dp/187873959X) (Waite Group Press)

## Screenshots

![Screenshot](/screenshots/wiremaze.png "wiremaze")
![Screenshot](/screenshots/polymaze.png "polymaze")
![Screenshot](/screenshots/bitmapmaze.png "bitmapmaze")
![Screenshot](/screenshots/bitmapmaze2.png "bitmapmaze2")
![Screenshot](/screenshots/wallspoly.png "wallspoly")
![Screenshot](/screenshots/walls.png "walls")
![Screenshot](/screenshots/heightmap.png "heightmap")
![Screenshot](/screenshots/heightmap2.png "heightmap2")
![Screenshot](/screenshots/raycast.png "raycast")
![Screenshot](/screenshots/raycastlight.png "raycastlight")
![Screenshot](/screenshots/lightmap.png "lightmap")
![Screenshot](/screenshots/lightmap2.png "lightmap2")
![Screenshot](/screenshots/gardens.png "gardens")
![Screenshot](/screenshots/raytrace.png "raytrace")
