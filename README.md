# 2048.wasm

2048.WASM is purely written in C and compiled to WebAssembly.

## Player Controls

- Use the arrow keys <kbd>ᐊ</kbd> <kbd>ᐅ</kbd> <kbd>ᐃ</kbd> <kbd> ᐁ</kbd> to
  slide the tiles.
- Press <kbd>n</kbd> to play over.

## Installation Notes

Before beginning to play the game, installation is necessary. Run the following
command in your terminal:

```sh
$ cd src
$ make game
$ ./play
```

**Note:** Make sure you add the appropriate path to SDL2 headers and libs under
Makefile.


### Compile C code to WASM via emscripten

```sh
$ emcc -s WASM=1 -s USE_SDL=2 -s USE_SDL_TTF=2 --preload-file src/Nunito-ExtraBold.ttf -O3 -I include -L lib src/main.c -o docs/2048.js
```

---

<center>Copyright &copy; Nishchith Shetty 2021</center>