# Union API

Union API is the core of Union Framework that provides an API to create native plugins for Gothic games based on ZenGin. Native plugins can use Union API to access the game objects in memory and hook any function using the [Decorator pattern](https://refactoring.guru/design-patterns/decorator) to extend and modify ZenGin behavior to a large extent.

The complementary [Gothic API](https://gitlab.com/union-framework/gothic-api) repository contains ZenGin headers with a mapping to the memory address for each supported Gothic release.

## Project status

Union API is **under active development** and is not released yet. You are welcome to try and play with it, but please keep in mind that it's not finished, may contain bugs and the API may change before the final release. 

## Documentation

Documentation is available on [Union API Wiki](https://gitlab.com/union-framework/union-api/-/wikis/home).

## Usage

### Build

Union API is a Visual Studio solution and requires Visual Studio 2022 (v143) platform toolset to compile. To build the Union API, clone the repository to your computer, open `union-api.sln` using Visual Studio and select the configuration:

* **DLL** - dynamically linked library (recommended)
* **LIB** - statically linked library
* **EXE** - executable with an entry point from `union-api.cpp` for testing the code without running it as DLL attached to the game

Configurations with `d` suffix build the project for debugging.

### Create a plugin

To create a plugin using Union API, you have to create a C++ project, add `union-api` to include directories and link Union API to it. Then you can compile the plugin with preprocessor definitions `_UNION_API_DLL` if you linked Union API dynamically or `_UNION_API_LIB` if statically. The resulting plugin DLL can be loaded by Gothic with Union runtime installed.

For the step-by-step instructions follow [Create a new project](https://gitlab.com/union-framework/union-api/-/wikis/Create-a-new-project) Wiki page.

## Support

Community-driven support is provided on [Gothic Modding Community Discord](https://discord.gg/mCpS5b5SUY).

If you have found a bug, please [create an issue](https://gitlab.com/union-framework/union-api/-/issues) with a detailed description and your environment.

## License

Union API is licensed under [GNU GENERAL PUBLIC LICENSE Version 3](https://gitlab.com/union-framework/union-api/-/blob/main/LICENSE).

### Third-party Licenses

* Detours [MIT License (c) Microsoft Corporation](union-api/Libs/licenses/Detours_LICENSE) - https://github.com/microsoft/Detours
* libogg [BSD 3-Clause License (c) 2002, Xiph.org Foundation](union-api/Libs/licenses/libogg_LICENSE) - https://github.com/xiph/ogg
* Vorbis [BSD 3-Clause License (c) 2002, Xiph.org Foundation](union-api/Libs/licenses/Vorbis_LICENSE) - https://github.com/xiph/vorbis
* zlib [zlib License (C) 1995-2023 Jean-loup Gailly and Mark Adler](union-api/Libs/licenses/zlib_LICENSE) - https://zlib.net/