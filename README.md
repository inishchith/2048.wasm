# 2048.c
2048 written in C and compiled to WebAssembly


## Usage

- Compile C code
```sh
$ cd src
$ gcc 2048.c -o play -I ./include -L ./lib -l SDL2-2.0.0 -lSDL2 -lSDL2_ttf
```

- Compile C code to WASM
```sh
$ cd src
$emcc -s WASM=1 -s USE_SDL=2 -I src/include -L ./lib  src/*.c -o web/2048.wasm -o web/2048.js
```
