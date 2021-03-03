# 2048.wasm
2048 written in C and compiled to WebAssembly


### Play

- Use the arrow keys ( <kbd> ᐊ </kbd> <kbd> ᐅ </kbd> <kbd> ᐃ </kbd> <kbd> ᐁ </kbd> ) to slide the tiles.
- press <kbd> n </kbd> to play over.

### Usage

- Compile C code
```sh
$ cd src
$ make game
$ ./play
```

**Note:** Make sure you add the appropriate path to SDL2 headers and libs under Makefile.


- Compile C code to WASM via emscripten
```sh
$ emcc -s WASM=1 -s USE_SDL=2 -s USE_SDL_TTF=2 --preload-file src/Nunito-ExtraBold.ttf -O3 -I include -L lib src/main.c -o docs/2048.js
```

### But Why?

why not? Took this up as an experiment; This is just 300 lines of really poor C code which took forever to compile to wasm (rip sdl font).
