#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>

#include<SDL2/SDL.h>
#include<SDL2/SDL_ttf.h>

#define GRID_SIZE 4
#define CELL_SIZE 80

bool init();
void begin();
bool loop();
void kill();

SDL_Window* window;
SDL_Renderer* renderer;

int window_side = (GRID_SIZE * CELL_SIZE) + 1;

// Dark Theme
SDL_Color grid_background = {22, 22, 22, 255};
SDL_Color grid_line_color = {44, 44, 44, 255};
SDL_Color grid_cursor_ghost_color = {44, 44, 44, 255};
SDL_Color grid_cursor_color = {255, 255, 255, 255};

struct game {
    int tiles[GRID_SIZE][GRID_SIZE];
};


// Light Theme
// SDL_Color grid_background = {233, 233, 233, 255}; // Barely white
// SDL_Color grid_line_color = {200, 200, 200, 255}; // Very light grey
// SDL_Color grid_cursor_ghost_color = {200, 200, 200, 255};
// SDL_Color grid_cursor_color = {160, 160, 160, 255}; // Grey


int main() {
    if (!init()) return 1;

    struct game g;
    begin(&g);

    while (loop (&g)) {
        SDL_Delay(10);
    }

    kill();
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

void display_board(struct game* g) {
    for(int i = 0; i < GRID_SIZE; ++i) {
        for(int j = 0; j < GRID_SIZE; ++j) {
            char snum[6];
            sprintf(snum, "%d", g->tiles[i][j]);
            // itoa(, snum, 10)

            TTF_Font* font = TTF_OpenFont("/Users/Nishchith/Projects/GitHub/2048.c/src/OpenSans-Bold.ttf", 24);
            SDL_Color White = {255, 255, 255};
            SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, snum, White);
            SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

            SDL_Rect Message_rect = {
                .x = j*CELL_SIZE + 20,
                .y = i*CELL_SIZE + 20,
                .w = CELL_SIZE / 2,
                .h = CELL_SIZE / 2,
            };

            SDL_RenderCopy(renderer, Message, NULL, &Message_rect);

            SDL_FreeSurface(surfaceMessage);
        }
    }
}

int random_cord() {
    return rand() % GRID_SIZE;
}

int random_value() {
    return 2;
}

void begin(struct game* g) {
    struct game g2 = {};

    *g = g2;
    g->tiles[random_cord()][random_cord()] = random_value();
    g->tiles[random_cord()][random_cord()] = random_value();
}

void add_value(struct game *g) {
    int i, j;
    while (1) {
        i = random_cord();
        j = random_cord();

        if(!g->tiles[i][j]) {
            g->tiles[i][j] = random_value();
            return;
        }
    }
}

void fall(struct game* g) {

    struct game g2 = {};
    int i, j, k, temp;

    // migrate zeros from front
    for (i = 0; i < GRID_SIZE; i++) {
        for (j = GRID_SIZE-1; j >=0; j--){
            for (k = j-1; k >=0; k--){
                if (g->tiles[j][i] == 0 && g->tiles[k][i] != 0) {
                    temp = g->tiles[k][i];
                    g->tiles[k][i] = g->tiles[j][i];
                    g->tiles[j][i] = temp;
                    break;
                }
            }
        }
    }

    for (i = GRID_SIZE - 2; i >= 0; i--) {
        for (j = 0; j < GRID_SIZE; j++) {
            if (i==GRID_SIZE-2)
                g2.tiles[i+1][j] = g->tiles[i+1][j];

            if (g->tiles[i][j] == g->tiles[i+1][j]) {
                g2.tiles[i+1][j] = g->tiles[i][j] * 2;
                g->tiles[i][j] = 0;
            }
            else if (g->tiles[i+1][j] == 0) {
                g2.tiles[i+1][j] = g->tiles[i][j];
                g->tiles[i][j] = 0;
            } else {
                g2.tiles[i][j] = g->tiles[i][j];
            }
        }
    }

    *g = g2;

    // migrate zeros from front
    for (i = 0; i < GRID_SIZE; i++) {
        for (j = GRID_SIZE-1; j >=0; j--){
            for (k = j-1; k >=0; k--){
                if (g->tiles[j][i] == 0 && g->tiles[k][i] != 0) {
                    temp = g->tiles[k][i];
                    g->tiles[k][i] = g->tiles[j][i];
                    g->tiles[j][i] = temp;
                    break;
                }
            }
        }
    }

    add_value(g);

}

void upside_down(struct game* g) {
    int temp;
    for(int i = 0; i < GRID_SIZE / 2; i++) {
        for(int j = 0; j < GRID_SIZE; j++) {
            temp = g->tiles[i][j];
            g->tiles[i][j] = g->tiles[GRID_SIZE-i-1][j];
            g->tiles[GRID_SIZE-i-1][j] = temp;
        }
    }
}

void rotate_right(struct game* g) {
    int temp;
    struct game g2 = {};
    for(int i = 0; i < GRID_SIZE; i++) {
        for(int j = 0; j < GRID_SIZE; j++) {
            g2.tiles[GRID_SIZE-1-j][i] = g->tiles[i][j];
        }
    }
    *g = g2;
}

void rotate_left(struct game *g) {
    struct game g2 = {};
    for(int i = 0; i < GRID_SIZE; i++) { 
        for(int j = 0; j < GRID_SIZE; j++) { 
            g2.tiles[i][j] = g->tiles[GRID_SIZE-1-j][i];
        }
    }

    *g = g2;
}


bool loop(struct game* g) {
    SDL_Event event;
    struct game g2;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_w:
                    case SDLK_UP:
                        upside_down(g);
                        fall(g);
                        upside_down(g);
                        break;
                    case SDLK_s:
                    case SDLK_DOWN:
                        fall(g);
                        break;
                    case SDLK_a:
                    case SDLK_LEFT:
                        rotate_right(g);
                        fall(g);
                        rotate_left(g);
                        break;
                    case SDLK_d:
                    case SDLK_RIGHT:
                        rotate_left(g);
                        fall(g);
                        rotate_right(g);
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

    // Add random 2

    SDL_SetRenderDrawColor(renderer, grid_background.r, grid_background.g, grid_background.b, grid_background.a);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, grid_line_color.r, grid_line_color.g, grid_line_color.b, grid_line_color.a);

    for (int x = 0; x < window_side; x += CELL_SIZE) {
        SDL_RenderDrawLine(renderer, x, 0, x, window_side);
    }

    for (int y = 0; y < window_side; y += CELL_SIZE) {
        SDL_RenderDrawLine(renderer, 0, y, window_side, y);
    }

    display_board(g);

    SDL_RenderPresent(renderer);

    return true;
}

void kill() {
    /* Free resources and Quit */

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}