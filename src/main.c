#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "./draw.h"
#include "./game.h"
#include "./math.h"

//SDL_Window *window = NULL;
SDL_Surface *screen = NULL;
SDL_Surface *sgame = NULL;
//SDL_Renderer *renderer;
SDL_Event e;
bool quit = false;
bool gameOver = false;

Uint64 now = 0;
Uint64 last = 0;
double deltaTime = 0;

int cubesLength = 0;
Cube cubes[1000];

void init() {
  SDL_Init(SDL_INIT_EVERYTHING);
  SDL_ShowCursor(SDL_DISABLE);
  //window = SDL_CreateWindow("Blockamok", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
  //renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  screen = SDL_SetVideoMode(WIDTH, HEIGHT, 16,  SDL_HWSURFACE | SDL_DOUBLEBUF| SDL_FULLSCREEN);
  sgame = SDL_CreateRGBSurface(SDL_HWSURFACE, 320, 240, screen->format->BitsPerPixel,
			screen->format->Rmask,
			screen->format->Gmask,
			screen->format->Bmask, 0);
  srand(time(NULL));
  TTF_Init();
  gameInit(cubes, &cubesLength);
}

void gameLoop() {
  SDL_PollEvent(&e);
  if (!gameOver) {
    gameOver = gameFrame(deltaTime, cubes, &cubesLength);
  }
  if (e.type == SDL_QUIT || e.key.keysym.sym == SDLK_ESCAPE) {
    quit = true;
  }
}

int main(int arg, char *argv[]) {
  init();

  Uint32 bg = SDL_MapRGB(screen->format, 15, 255, 155);

  while (!quit) {
    last = now;
    now = SDL_GetTicks();

    gameLoop();
    SDL_FillRect(sgame, NULL, bg);
    draw(sgame);
    drawCubes(sgame, cubes, cubesLength);
    SDL_BlitSurface(sgame, NULL, screen, NULL);

    drawSpeedText(screen);
    if (gameOver) {
      drawGameOverText(screen);
    }

    SDL_Flip(screen);

    deltaTime = (double)((now - last)) / 12000;
  }

  SDL_FreeSurface(screen);
  SDL_Quit();
  return 0;
}
