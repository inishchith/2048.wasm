# 2048.c
2048 written in C and compiled to WebAssembly


## Usage

- Compile C code
```sh
$ cd src
$ gcc 2048.c -o play -I ./include -L ./lib -l SDL2-2.0.0 -lSDL2 -lSDL2_ttf
```

- Compile C code to WASM via emscripten
```sh
$ emcc -s WASM=1 -s USE_SDL=2 -s USE_SDL_TTF=2 --preload-file src/Nunito-ExtraBold.ttf -O3 -I include -L lib src/main.c -o docs/2048.js
```
