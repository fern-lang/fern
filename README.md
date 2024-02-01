<!-- markdownlint-disable MD014 -->

# Fern

[![Ko-Fi Badge](https://img.shields.io/badge/Ko--fi-F16061?style=for-the-badge&logo=ko-fi&logoColor=white)](https://ko-fi.com/julesnieves)

## Setup

Before you build, you need to install LLVM as well as basic development packages. On Ubuntu, you can do this with the following command:

```bash
sudo apt install llvm-dev build-essential git cmake ninja-build
```

## Building and Running

```bash
$ git clone https://github.com/fern-lang/fern.git
$ cd fern
$ mkdir build
$ cd build
$ cmake -G Ninja ..
$ ninja
```

## TODO

- [ ] A usable Fern compiler
- [ ] C to Fern bindings
- [ ] Fern to C bindings
- [ ] Fern stdlib
