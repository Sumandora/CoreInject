# CoreInject

CoreInject is a utility for injecting shared library objects (.so files) into running Linux processes. It can be useful for debugging, instrumentation, hacking, and more.

## Features

- Inject multiple shared libraries into a target process
- Bypass sandbox restrictions by relocating libraries
- Delete injected libraries after use
- Easy to use CLI

## Usage

Basic usage:
```
coreinject -p 123 -m lib1.so -m lib2.so
```
This will inject lib1.so and lib2.so into the process with ID 123.

The following flags can be used to control injection behavior:

- `--allowDoubleInjection`: Allow injecting libraries already loaded by the target process
- `--workaroundSandboxes`: Relocate libraries to target process working directory when needed
- `--overwriteRelocationTarget`: Overwrite existing library files when relocating
- `--deleteAfterInjection`: Delete injected libraries after injection finishes

## Building

To build:
```bash
mkdir Build
cd Build
cmake ..
cmake --build . -j $(nproc)
```

## Installing

To install (run as root):
```bash
cmake --install .
```