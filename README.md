# CoreInject

CoreInject is a utility for injecting shared library objects (.so files) into running Linux processes. It can be useful for debugging, instrumentation, hacking, and more.

## Features

- Inject multiple shared libraries into a target process
- Bypass sandbox restrictions by relocating libraries
- Delete injected libraries after use
- Easy to use CLI

## Usage

### This tool requires GDB to inject libraries
Basic usage:
```
CoreCLI -p 123 -m lib1.so -m lib2.so
```
This will inject lib1.so and lib2.so into the process with ID 123.

The following flags can be used to control injection behavior:

- ˋ--allowDoubleInjectionˋ: If enabled, then modules that are already loaded by the target process wont be filtered
- ˋ--workaroundSandboxesˋ: Before injection, copy the library to the current working directory of the target process
- ˋ--overwriteRelocationTargetˋ: If the target file already exists, then it will be overwritten (Requires ˋ--workaroundSandboxesˋ)
- ˋ--deleteAfterInjectionˋ: Deletes the module after injection. In case of a sandbox workaround, this affects the relocated module, not the original
- ˋ--ignoreELFHeaderˋ: Allows modules with invalid elf header to be injected (also allows mismatching architectures)

## Building

To build:
```bash
mkdir Build
cd Build
cmake ..
cmake --build . -j $(nproc)
```
To install (run as root):
```bash
cmake --install .
```
