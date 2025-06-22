# Union API

Union API is the core of Union Framework that provides an API to create native plugins for Gothic games based on ZenGin. Native plugins can use Union API to access the game objects in memory and hook any function using the [Decorator pattern](https://refactoring.guru/design-patterns/decorator) to extend and modify ZenGin behavior to a large extent.

The complementary [Gothic API](https://gitlab.com/union-framework/gothic-api) repository contains ZenGin headers with a mapping to the memory address for each supported Gothic release.

## Project status

Union API is **under active development** and is not released yet. You are welcome to try and play with it, but please keep in mind that it's not finished, may contain bugs and the API may change before the final release. 

## Documentation

Documentation is available on [Union Framework Docs](https://union-framework.gitlab.io/docs/union-api/).

## Usage

### Build
Union API is built using [CMake](https://cmake.org/) and requires C++17 or later to compile. The recommended way to build is to use [Visual Studio 2022](https://visualstudio.microsoft.com/vs/) with CMake support enabled.

Project could be built in several configurations, depending on how you want to use it:

* **DLL** - dynamically linked library
* **LIB** - statically linked library
* **EXE** - executable with an entry point from `union-api.cpp` for testing the code without running it as DLL attached to the game

You can change the configuration directly in `CMakeLists.txt`  or by using command line. Default configuration is `LIB`.

### Including

To include Union API in your project, you need to get its source using git submodule or CMake's `FetchContent` module.

If you are using git submodule, run the following command in your project root:

```bash
git submodule add https://gitlab.com/union-framework/union-api.git
```
This will add Union API as a submodule in your project. You can then include it in your CMake project by adding the following lines to your `CMakeLists.txt`:

```cmake
add_subdirectory(union-api)
```

If you are using CMake's `FetchContent`, add the following lines to your `CMakeLists.txt`:

```cmake
include(FetchContent)
FetchContent_Declare(
    union-api
    GIT_REPOSITORY https://gitlab.com/union-framework/union-api.git
    GIT_TAG main
)
FetchContent_MakeAvailable(union-api)
```

### Linking

After including Union API, you can link it to your target. For example, if your target is `your_target`, you can add the following line to your `CMakeLists.txt`:

```cmake
target_link_libraries(your_target PRIVATE union_api_lib)
```

## Support

Community-driven support is provided on [Gothic Modding Community Discord](https://discord.gg/mCpS5b5SUY).

If you have found a bug, please [create an issue](https://gitlab.com/union-framework/union-api/-/issues) with a detailed description and your environment.

## License

Union API is licensed under [BSD 3-Clause](https://gitlab.com/union-framework/union-api/-/blob/main/LICENSE).

### Third-party Licenses

* Detours [MIT License (c) Microsoft Corporation](union-api/Libs/licenses/Detours_LICENSE) - https://github.com/microsoft/Detours
* libogg [BSD 3-Clause License (c) 2002, Xiph.org Foundation](union-api/Libs/licenses/libogg_LICENSE) - https://github.com/xiph/ogg
* Vorbis [BSD 3-Clause License (c) 2002, Xiph.org Foundation](union-api/Libs/licenses/Vorbis_LICENSE) - https://github.com/xiph/vorbis
* zlib [zlib License (C) 1995-2023 Jean-loup Gailly and Mark Adler](union-api/Libs/licenses/zlib_LICENSE) - https://zlib.net/