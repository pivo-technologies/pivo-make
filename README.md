
# pivo-make

Cross-platform build system to be used with [LLVM](https://llvm.org/)

`pivo` comes from Russian `пиво` that is translated to `beer` in English. Project is named after beer because it was consumed a lot during development

##
- [Requirements](#Requirements)
- [Features](#Features)
- [Install](#Install)
- [Configuration](#Configuration)
- [Usage](#Usage)
- [How to install LLVM](#How-to-install-LLVM)
- [TODO](#TODO)

## Requirements
- LLVM installation
- Python to build and install
- GLFW if using Linux
- [DirectX End-User Runtime](https://www.microsoft.com/en-us/download/details.aspx?id=35) if using Windows
## Features

- [ImGui](https://github.com/ocornut/imgui) based interface to configure compiler and linker behaviour (as in Visual Studio)
- OS supported: Windows & Linux
- Cross-platform compilation (compile Windows binaries from Linux and vice-versa)
- Custom locations of compiler and linker
- Multithreaded compilation
- CLI build support
- Only 64-bit compilation is supported at this moment


## Install

1. Clone repository
```
git clone https://github.com/pivo-technologies/pivo-make
```

### Windows
2. Launch install.py. It will ask you for Administator rights to proceed installation
pivo-make will be installed in C:\Program Files\pivo-make and this directory will be added to PATH environment variable

### Linux
2. Enter directory and start install.py as root
```
cd pivo-make
sudo python install.py
```
pivo-make will be installed in /usr/local/bin
## Configuration
You can change default settings for pivo-make. Configuration file `pivo.config` is located at `C:\Program Files\pivo-make\` or `/etc/pivo-make/` if you are on Windows or Linux respectively
```JSON
{
  "cxx_windows": "clang++",
  "cxx_linux": "clang++",
  "linker_windows" : "lld-link",
  "linker_linux" : "clang++",
  "include_path_windows" : [
    "C:\\Program Files\\Microsoft Visual Studio\\2022\\Enterprise\\VC\\Tools\\MSVC\\14.38.33130\\include",
    "C:\\Program Files\\Microsoft Visual Studio\\2022\\Enterprise\\VC\\Tools\\MSVC\\14.38.33130\\atlmfc\\include",
    "C:\\Program Files\\Microsoft Visual Studio\\2022\\Enterprise\\VC\\Auxiliary\\VS\\include",
    "C:\\Program Files (x86)\\Windows Kits\\10\\Include\\10.0.22621.0\\ucrt",
    "C:\\Program Files (x86)\\Windows Kits\\10\\Include\\10.0.22621.0\\um",
    "C:\\Program Files (x86)\\Windows Kits\\10\\Include\\10.0.22621.0\\shared",
    "C:\\Program Files (x86)\\Windows Kits\\10\\Include\\10.0.22621.0\\winrt",
    "C:\\Program Files (x86)\\Windows Kits\\10\\Include\\10.0.22621.0\\cppwinrt"
  ],
  "lib_path_windows" : [
    "C:\\Program Files\\Microsoft Visual Studio\\2022\\Enterprise\\VC\\Tools\\MSVC\\14.38.33130\\lib\\x64",
    "C:\\Program Files\\Microsoft Visual Studio\\2022\\Enterprise\\VC\\Tools\\MSVC\\14.38.33130\\atlmfc\\lib\\x64",
    "C:\\Program Files\\Microsoft Visual Studio\\2022\\Enterprise\\VC\\Auxiliary\\VS\\lib\\x64",
    "C:\\Program Files (x86)\\Windows Kits\\10\\lib\\10.0.22621.0\\ucrt\\x64",
    "C:\\Program Files (x86)\\Windows Kits\\10\\lib\\10.0.22621.0\\um\\x64"
  ],
  "include_path_linux" : [

  ],
  "lib_path_linux" : [
    
  ]
}
```

- `cxx_windows`, `cxx_linux`: default compilers which are used when compiling for Windows and Linux respectively
- `linker_windows`, `linker_linux`: default linkers which are used when compiling for Windows and Linux respectively
- `include_path_windows`, `lib_path_windows`, `include_path_linux`, `lib_path_linux`: directories added to compiler/linker when compiling for chosen OS (includes and libraries for Windows are already added to list, only need to change paths if yours differ)

If you are on Linux and do not have Windows SDK you can [download](https://github.com/pivo-technologies/visual-cpp) and change paths in config file
## Usage
`pivo-make <args>`

- `-c` or `--config`: config file to build from
- `-i` or `--intermediate`: directory where to store object files
- `-o` or `--output`: output file name
## How to install LLVM

1. Find the latest release at [LLVM Releases Page](https://github.com/llvm/llvm-project/releases)

### Windows
2. Download and install LLVM-X.X.X-win64.exe

### Linux
2. Install CMake from your package manager
3. Download source code of the latest release and unpack it
4. Enter LLVM directory
5. `cmake -S llvm -B build -G "Unix Makefiles" -DLLVM_ENABLE_PROJECTS="clang;lld;lldb" -DCMAKE_BUILD_TYPE=Release -DLLVM_PARALLEL_COMPILE_JOBS=N -DLLVM_PARALLEL_LINK_JOBS=N`. Where `N` is CPU threads to use for compilation. If you don't need LLDB then erase it from LLVM_ENABLE_PROJECTS
6. `cd build`
7. `make -jN`. Where `N` is CPU threads to use for compilation
8. `sudo make install`
## TODO

- [ ] Add caching to prevent pivo-make from recompiling object files that don't need to be compiled again
- [ ] Add 32-bit compilation support
- [ ] Add MacOS support
