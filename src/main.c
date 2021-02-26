#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>

#include<SDL2/SDL.h>
#include<SDL2/SDL_ttf.h>

#ifdef __EMSCRIPTEN__

#include<emscripten.h>

#endif

#define GRID_SIZE 4
#define CELL_SIZE 80

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

// Dark Theme
// SDL_Color grid_background = {119, 110, 101, 0};
SDL_Color grid_background = {22, 22, 22, 255};
SDL_Color grid_line_color = {44, 44, 44, 255};
SDL_Color grid_cursor_ghost_color = {44, 44, 44, 255};
SDL_Color grid_cursor_color = {255, 255, 255, 255};

int game[GRID_SIZE][GRID_SIZE];


// Light Theme
// SDL_Color grid_background = {233, 233, 233, 255}; // Barely white
// SDL_Color grid_line_color = {200, 200, 200, 255}; // Very light grey
// SDL_Color grid_cursor_ghost_color = {200, 200, 200, 255};
// SDL_Color grid_cursor_color = {160, 160, 160, 255}; // Grey


#ifdef __EMSCRIPTEN__

void mainloop() {
    loop();
}

#endif


int main() {
    if (!init()) return 1;

    memset(game, 0, sizeof(int) * GRID_SIZE * GRID_SIZE);

    begin();
    font = TTF_OpenFont("OpenSans-Bold.ttf", 20);

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(mainloop, 0, 1);
#else
    while (loop (game)) {
        // SDL_Delay(10);
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

    for(int i = 0; i < GRID_SIZE; ++i) {
        for(int j = 0; j < GRID_SIZE; ++j) {
            char *snum = calloc(6, sizeof(char));
            sprintf(snum, "%d", game[i][j]);

            SDL_Color Font_Color = {255, 255, 255};
            surface = TTF_RenderText_Blended(font, snum, Font_Color);
            texture = SDL_CreateTextureFromSurface(renderer, surface);

            SDL_Rect Message_rect = {
                .x = j*CELL_SIZE + 20,
                .y = i*CELL_SIZE + 20,
                .w = CELL_SIZE / 2,
                .h = CELL_SIZE / 2,
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
    exit(0);
}

void fall() {
    int i, j, k, temp;
    int (*g2)[GRID_SIZE] = calloc(GRID_SIZE, GRID_SIZE * sizeof(int));

    // migrate zeros from front
    for (i = 0; i < GRID_SIZE; i++) {
        for (j = GRID_SIZE-1; j >=0; j--){
            for (k = j-1; k >=0; k--){
                if (game[j][i] == 0 && game[k][i] != 0) {
                    temp = game[k][i];
                    game[k][i] = game[j][i];
                    game[j][i] = temp;
                    break;
                }
            }
        }
    }

    for (i = GRID_SIZE - 2; i >= 0; i--) {
        for (j = 0; j < GRID_SIZE; j++) {
            if (i==GRID_SIZE-2)
                g2[i+1][j] = game[i+1][j];

            if (game[i][j] == game[i+1][j]) {
                g2[i+1][j] = game[i][j] * 2;
                game[i][j] = 0;
            }
            else if (game[i+1][j] == 0) {
                g2[i+1][j] = game[i][j];
                game[i][j] = 0;
            } else {
                g2[i][j] = game[i][j];
            }
        }
    }

    memcpy(game, g2, sizeof(int) * GRID_SIZE * GRID_SIZE);
    free(g2);

    // migrate zeros from front
    for (i = 0; i < GRID_SIZE; i++) {
        for (j = GRID_SIZE-1; j >=0; j--){
            for (k = j-1; k >=0; k--){
                if (game[j][i] == 0 && game[k][i] != 0) {
                    temp = game[k][i];
                    game[k][i] = game[j][i];
                    game[j][i] = temp;
                    break;
                }
            }
        }
    }

    add_value();
}

void upside_down() {
    int temp;
    for(int i = 0; i < GRID_SIZE / 2; i++) {
        for(int j = 0; j < GRID_SIZE; j++) {
            temp = game[i][j];
            game[i][j] = game[GRID_SIZE-i-1][j];
            game[GRID_SIZE-i-1][j] = temp;
        }
    }
}

void rotate_right() {
    int temp;
    int (*g2)[GRID_SIZE] = calloc(GRID_SIZE, GRID_SIZE * sizeof(int));

    for(int i = 0; i < GRID_SIZE; i++) {
        for(int j = 0; j < GRID_SIZE; j++) {
            g2[GRID_SIZE-1-j][i] = game[i][j];
        }
    }

    memcpy(game, g2, sizeof(int) * GRID_SIZE * GRID_SIZE);
    free(g2);
}

void rotate_left() {
    int (*g2)[GRID_SIZE] = calloc(GRID_SIZE, GRID_SIZE * sizeof(int));

    for(int i = 0; i < GRID_SIZE; i++) { 
        for(int j = 0; j < GRID_SIZE; j++) { 
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
                        printf("UP \n");
                        fall();
                        upside_down();
                        break;
                    case SDLK_s:
                    case SDLK_DOWN:
                        printf("DOWN \n");
                        fall();
                        break;
                    case SDLK_a:
                    case SDLK_LEFT:
                        printf("LEFT \n");
                        rotate_right();
                        fall();
                        rotate_left();
                        break;
                    case SDLK_d:
                    case SDLK_RIGHT:
                        printf("RIGHT \n");
                        rotate_left();
                        fall();
                        rotate_right();
                        break;
                }
                break;
            // ADD NEW GAME
            // begin(&g);
            case SDL_QUIT:
                return false;
                break;
        }
    }

    SDL_SetRenderDrawColor(renderer, grid_background.r, grid_background.g, grid_background.b, grid_background.a);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, grid_line_color.r, grid_line_color.g, grid_line_color.b, grid_line_color.a);

    for (int x = 0; x < window_side; x += CELL_SIZE) {
        SDL_RenderDrawLine(renderer, x, 0, x, window_side);
    }

    for (int y = 0; y < window_side; y += CELL_SIZE) {
        SDL_RenderDrawLine(renderer, 0, y, window_side, y);
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