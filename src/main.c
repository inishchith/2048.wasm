#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>

#include<SDL2/SDL.h>
#include<SDL2/SDL_ttf.h>

#ifdef __EMSCRIPTEN__

#include<emscripten.h>

#endif

#define GRID_SIZE 4
#define CELL_SIZE 120

bool init();
void begin();
bool loop();
void display_board();
void kill_resources();

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Surface* surface;
SDL_Texture* texture;

int window_side = (GRID_SIZE * CELL_SIZE) + 1;
TTF_Font* font;
int INIT_DISPLAY = true;


SDL_Color grid_background = {30, 100, 119, 255};
SDL_Color grid_line_color = {15, 50, 60, 255};
SDL_Color grid_cursor_ghost_color = {15, 50, 60, 255};
SDL_Color grid_cursor_color = {255, 255, 255, 255};

int game[GRID_SIZE][GRID_SIZE];


#ifdef __EMSCRIPTEN__

void mainloop() {
    loop();
}

#endif


int main() {
    if (!init()) return 1;

    begin();
    font = TTF_OpenFont("Nunito-ExtraBold.ttf", 108);

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(mainloop, 0, 1);
#else
    while (loop (game)) {
        SDL_Delay(100);
    }
#endif

    kill_resources();
    return 0;
}

bool init() {
    /* Initialize resources */

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Initialize SDL: %s", SDL_GetError());
        return EXIT_FAILURE;
    }

    if (SDL_CreateWindowAndRenderer(window_side, window_side, 0, &window, &renderer) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Create window and renderer: %s", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_SetWindowTitle(window, "2048");

    TTF_Init();

    return true;
}

void display_board() {
    if (font == NULL) {
        printf("Failed to load font!\n");
    }

    // find Max Index
    int _max_i = 0, _max_j = 0, _max = game[0][0], i, j;

    for(i = 0; i < GRID_SIZE; ++i) {
        for(j = 0; j < GRID_SIZE; ++j) {
            if (_max < game[i][j]) {
                _max = game[i][j];
                _max_i = i;
                _max_j = j;
            }
        }
    }

    for(i = 0; i < GRID_SIZE; ++i) {
        for(j = 0; j < GRID_SIZE; ++j) {
            if(!game[i][j]) continue;

            char *snum = calloc(6, sizeof(char));
            sprintf(snum, "%d", game[i][j]);

            SDL_Color Font_Color;

            if (i == _max_i && j == _max_j) {
                SDL_Rect grid_cursor_ghost = {
                    j * CELL_SIZE,
                    i * CELL_SIZE,
                    CELL_SIZE,
                    CELL_SIZE
                };
                Font_Color.r = 0;
                Font_Color.g = 255;
                Font_Color.b = 255;
                SDL_SetRenderDrawColor(renderer, grid_cursor_ghost_color.r,
                                   grid_cursor_ghost_color.g,
                                   grid_cursor_ghost_color.b,
                                   grid_cursor_ghost_color.a);
                SDL_RenderFillRect(renderer, &grid_cursor_ghost);
            }
            else {
                Font_Color.r = 255;
                Font_Color.g = 255;
                Font_Color.b = 255;
            }

            surface = TTF_RenderText_Blended(font, snum, Font_Color);
            texture = SDL_CreateTextureFromSurface(renderer, surface);

            SDL_Rect Message_rect = {
                .x = j * CELL_SIZE + 36,
                .y = i * CELL_SIZE + 30,
                .w = 42 + game[i][j] / 16,
                .h = 72,
            };

            SDL_RenderCopy(renderer, texture, NULL, &Message_rect);

            SDL_DestroyTexture(texture);
            SDL_FreeSurface(surface);
            free(snum);
        }
    }
}

int random_cord() {
    return rand() % GRID_SIZE;
}

int random_value() {
    return 2;
}

void begin() {
    memset(game, 0, sizeof(int) * GRID_SIZE * GRID_SIZE);

    game[random_cord()][random_cord()] = random_value();
    game[random_cord()][random_cord()] = random_value();
}

void add_value() {
    int i, j, c=15;
    while (c) {
        i = random_cord();
        j = random_cord();

        if(!game[i][j]) {
            game[i][j] = random_value();
            return;
        }
        c--;
    }

    // lol
    begin();
}

void fall() {
    int i, j, k, temp;

    // migrate zeros to backward
    for (i = 0; i < GRID_SIZE; ++i) {
        j = GRID_SIZE - 1;
        k = GRID_SIZE - 2;
        while (k > -1) {
            if (game[j][i] == 0 && game[k][i] != 0) {
                temp = game[k][i];
                game[k][i] = game[j][i];
                game[j][i] = temp;
                j--;
            }
            else if (game[j][i]) {
                j--;
            }
            k--;
        }
    }

    // resolve
    for (i = GRID_SIZE - 2; i >= 0; --i) {
        for (j = 0; j < GRID_SIZE; ++j) {
            if (game[i][j] == game[i+1][j]) {
                game[i+1][j] = game[i][j] * 2;
                game[i][j] = 0;
            }
            else if (game[i+1][j] == 0) {
                game[i+1][j] = game[i][j];
                game[i][j] = 0;
            } else {
                game[i][j] = game[i][j];
            }
        }
    }
}

void upside_down() {
    int temp;
    for(int i = 0; i < GRID_SIZE / 2; ++i) {
        for(int j = 0; j < GRID_SIZE; ++j) {
            temp = game[i][j];
            game[i][j] = game[GRID_SIZE-i-1][j];
            game[GRID_SIZE-i-1][j] = temp;
        }
    }
}

void rotate_right() {
    int temp;
    int (*g2)[GRID_SIZE] = calloc(GRID_SIZE, GRID_SIZE * sizeof(int));

    for(int i = 0; i < GRID_SIZE; ++i) {
        for(int j = 0; j < GRID_SIZE; ++j) {
            g2[GRID_SIZE-1-j][i] = game[i][j];
        }
    }

    memcpy(game, g2, sizeof(int) * GRID_SIZE * GRID_SIZE);
    free(g2);
}

void rotate_left() {
    int (*g2)[GRID_SIZE] = calloc(GRID_SIZE, GRID_SIZE * sizeof(int));

    for(int i = 0; i < GRID_SIZE; ++i) {
        for(int j = 0; j < GRID_SIZE; ++j) {
            g2[i][j] = game[GRID_SIZE-1-j][i];
        }
    }

    memcpy(game, g2, sizeof(int) * GRID_SIZE * GRID_SIZE);
    free(g2);
}


bool loop() {
    SDL_Event event;

    if (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_w:
                    case SDLK_UP:
                        upside_down();
                        fall();
                        upside_down();
                        break;
                    case SDLK_s:
                    case SDLK_DOWN:
                        fall();
                        break;
                    case SDLK_a:
                    case SDLK_LEFT:
                        rotate_right();
                        fall();
                        rotate_left();
                        break;
                    case SDLK_d:
                    case SDLK_RIGHT:
                        rotate_left();
                        fall();
                        rotate_right();
                        break;
                    case SDLK_n:
                        begin();
                        break;
                }
                add_value();
                break;
            case SDL_QUIT:
                return false;
                break;
        }
    }

    SDL_SetRenderDrawColor(renderer, grid_background.r, grid_background.g, grid_background.b, grid_background.a);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, grid_line_color.r, grid_line_color.g, grid_line_color.b, grid_line_color.a);

    // I know this is bad - but how otherwise
    for (int x = 0; x < window_side; x += CELL_SIZE) {
        SDL_RenderDrawLine(renderer, x, 0, x, window_side);
        SDL_RenderDrawLine(renderer, x + 1, 0, x + 1, window_side);
        SDL_RenderDrawLine(renderer, x + 2, 0, x + 2, window_side);
        SDL_RenderDrawLine(renderer, x + 3, 0, x + 3, window_side);
    }

    for (int y = 0; y < window_side; y += CELL_SIZE) {
        SDL_RenderDrawLine(renderer, 0, y, window_side, y);
        SDL_RenderDrawLine(renderer, 0, y + 1, window_side, y + 1);
        SDL_RenderDrawLine(renderer, 0, y + 2, window_side, y + 2);
        SDL_RenderDrawLine(renderer, 0, y + 3, window_side, y + 3);
    }

    display_board();
    SDL_RenderPresent(renderer);

    return true;
}

void kill_resources() {
    /* Free resources and Quit */

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}