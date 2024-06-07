#include <SDL/SDL.h>

#include "./game.h"

#define WIDTH 1000
#define HEIGHT 1000

void draw(SDL_Surface *surface);

void drawCubes(SDL_Surface *surface, Cube cubes[], int cubesLength);

void drawCube(SDL_Surface *surface, Cube cube);

void drawSpeedText(SDL_Surface *surface);

void drawGameOverText(SDL_Surface *surface);
