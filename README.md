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
$ emcc -s WASM=1 -s ALLOW_MEMORY_GROWTH=1 -s TOTAL_MEMORY=1024mb -s TOTAL_STACK=512mb -s USE_SDL=2 -s USE_SDL_TTF=2 --preload-file src/OpenSans-Bold.ttf -O3 -I include -L lib src/main.c -o docs/2048.js
$ emcc -s WASM=1 -s USE_SDL=2 -s USE_SDL_TTF=2 --preload-file src/OpenSans-Bold.ttf -O3 -I include -L lib src/main.c -o docs/2048.js
```
