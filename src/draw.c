#include "./draw.h"

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_gfxPrimitives.h>
#include <math.h>
#include <stdbool.h>

#include "./game.h"
#include "./math.h"

const int UP = 0;
const int DOWN = 1;
const int LEFT = 2;
const int RIGHT = 3;
const int FRONT = 4;

const int BACKGROUND_R = 15;
const int BACKGROUND_G = 255;
const int BACKGROUND_B = 155;

int TRANSFORMED_FRONT_I = FRONT * 5;

Point2 transformedCube[CUBE_FACE_N * 5];

typedef struct
{
    Uint8 r, g, b, a;
} Colora;

Colora darkBackgroundTriangle = {.r = 0, .b = 0, .g = 0, .a = 250 / 3};
Colora emptyBackgroundTriangle = {.r = 255, .b = 255, .g = 255, .a = 0};

TTF_Font *Sans = NULL;
SDL_Color TEXT_COLOR = {255, 0, 0};

typedef struct
{
    Point2 position;
    Colora color;
} Vertex;

void renderGeometry(SDL_Surface *surface, const Vertex* vertices, int length)
{
    if (length < 3) return;

    Sint16* vx = (Sint16*)malloc(length * sizeof(Sint16));
    Sint16* vy = (Sint16*)malloc(length * sizeof(Sint16));
    Uint8* vc = (Uint8*)malloc(4 * length * sizeof(Uint8));

    for (int i = 0; i < length; ++i)
    {
        vx[i] = (Sint16)vertices[i].position.x;
        vy[i] = (Sint16)vertices[i].position.y;
        vc[4 * i] = vertices[i].color.r;
        vc[4 * i + 1] = vertices[i].color.g;
        vc[4 * i + 2] = vertices[i].color.b;
        vc[4 * i + 3] = vertices[i].color.a;
    }

    filledPolygonRGBA(surface, vx, vy, length, vc[0], vc[1], vc[2], vc[3]);

    free(vx);
    free(vy);
    free(vc);
}

void drawBackgroundTriangle(SDL_Surface *screen, Point2 trianglePoints[])
{
    Vertex triangle[3];
    triangle[0].position = trianglePoints[0];
    triangle[1].position = trianglePoints[1];
    triangle[2].position = trianglePoints[2];
    triangle[0].color = darkBackgroundTriangle;
    triangle[1].color = emptyBackgroundTriangle;
    triangle[2].color = darkBackgroundTriangle;
    renderGeometry(screen, triangle, 3);
}

void draw(SDL_Surface *surface)
{
    Point2 triangle1Points[] =
    {
        {.x = -WIDTH, .y = HEIGHT / 2},
        {.x = WIDTH / 2, .y = -HEIGHT},
        {.x = WIDTH * 2, .y = HEIGHT / 2},
    };
    Point2 triangle2Points[] =
    {
        {.x = -WIDTH, .y = HEIGHT / 2},
        {.x = WIDTH / 2, .y = HEIGHT * 2},
        {.x = WIDTH * 2, .y = HEIGHT / 2},
    };

    drawBackgroundTriangle(surface, triangle1Points);
    drawBackgroundTriangle(surface, triangle2Points);
}

float screenX(float x)
{
    return x * WIDTH + WIDTH / 2;
}

float screenY(float y)
{
    return y * HEIGHT + HEIGHT / 2;
}

bool isPointOutsideFront(int f, int frontI)
{
    float x = transformedCube[f].x;
    float y = transformedCube[f].y;
    float frontStartX = transformedCube[frontI].x;
    float frontEndX = transformedCube[frontI + 2].x;
    float frontStartY = transformedCube[frontI].y;
    float frontEndY = transformedCube[frontI + 2].y;
    bool outWithX = x < frontStartX || x > frontEndX;
    bool outWithY = y < frontStartY || y > frontEndY;
    return outWithX || outWithY;
}

void drawCubes(SDL_Surface *surface, Cube cubes[], int cubesLength)
{
    for (int i = 0; i < cubesLength; i++)
    {
        drawCube(surface, cubes[i]);
    }
}

float fadeTowards(float current, float target, float amount)
{
    if (current == target)
    {
        return current;
    }
    float toDiff = fabs((current - target) * amount);
    if (current > target)
    {
        return current - toDiff;
    }
    else
    {
        return current + toDiff;
    }
}

void drawCube(SDL_Surface *surface, Cube cube)
{
    for (int f = 0; f < 5; f++)
    {
        int orgCubeI = f * 4;    // The way our cube is defined, a face has four cornes
        int transCubeI = f * 5;  // The way our transformed cube is defined, a face has 5 corners (last one connects back to the firt one)
        for (int p = 0; p < 4; p++)
        {
            Point point = cube[orgCubeI + p];
            Point2 sPoint = {screenX(transform3Dto2D(point.x, point.z)), screenY(transform3Dto2D(point.y, point.z))};
            transformedCube[transCubeI + p] = sPoint;
        }
        transformedCube[transCubeI + 4] = transformedCube[transCubeI + 0];
    }

    // If a have has at least two points outside of front, it gets to be drawn last
    int faceOrder[5];

    int lastI = 4;
    int firstI = 0;

    faceOrder[lastI--] = FRONT;  // Front always gets to be last

    for (int f = 0; f < 4; f++)
    {
        int cubeI = f * 5;
        bool sideOutsideFront = isPointOutsideFront(cubeI, FRONT * 5) && isPointOutsideFront(cubeI + 1, FRONT * 5);
        // If we are outside, we should draw this as last as possible
        if (sideOutsideFront)
        {
            faceOrder[lastI--] = f;
        }
        else
        {
            faceOrder[firstI++] = f;
        }
    }

    // No need to draw the first 2 faces. They are hidden behind the front
    for (int f = 2; f < 5; f++)
    {
        int cubeI = faceOrder[f] * 5;

        Point2 triable1Points[] =
        {
            {.x = transformedCube[cubeI + 0].x, .y = transformedCube[cubeI + 0].y},
            {.x = transformedCube[cubeI + 1].x, .y = transformedCube[cubeI + 1].y},
            {.x = transformedCube[cubeI + 2].x, .y = transformedCube[cubeI + 2].y},
        };

        Point2 triable2Points[] =
        {
            {.x = transformedCube[cubeI + 2].x, .y = transformedCube[cubeI + 2].y},
            {.x = transformedCube[cubeI + 3].x, .y = transformedCube[cubeI + 3].y},
            {.x = transformedCube[cubeI + 4].x, .y = transformedCube[cubeI + 4].y},
        };

        Colora color;

        if (f == FRONT)
        {
            Colora c = {.r = 200, .b = 120, .g = 250};
            color = c;
        }
        else
        {
            Colora c = {.r = 100, .b = 200, .g = 100};
            color = c;
        }

        float min = 150;
        float max = 190;

        float z = (cube[(cubeI / 5) * 4].z) + fabs(cube[(cubeI / 5) * 4].x) * 7 + fabs(cube[(cubeI / 5) * 4].y) * 7;
        float fadeAmount = z < min ? 0 : fmin((z - min) / (max - min), 1);

        color.r = color.r;
        color.g = color.g;
        color.b = color.b;
        color.a = fadeTowards(255, 0, fadeAmount);

        Vertex triangle1[3];
        Vertex triangle2[3];

        triangle1[0].color = color;
        triangle1[1].color = color;
        triangle1[2].color = color;
        triangle2[0].color = color;
        triangle2[1].color = color;
        triangle2[2].color = color;

        triangle1[0].position = triable1Points[0];
        triangle1[1].position = triable1Points[1];
        triangle1[2].position = triable1Points[2];

        triangle2[0].position = triable2Points[0];
        triangle2[1].position = triable2Points[1];
        triangle2[2].position = triable2Points[2];

        Point2 linePoints[] = {
            transformedCube[cubeI + 0],
            transformedCube[cubeI + 1],
            transformedCube[cubeI + 2],
            transformedCube[cubeI + 3],
            transformedCube[cubeI + 4],
        };

        renderGeometry(surface, triangle1, 3);
        renderGeometry(surface, triangle2, 3);
        fadeAmount = fmin(fadeAmount * 1.5, 1);
        //SDL_SetRenderDrawColor(renderer, 0, 0, 0, fadeTowards(255, 0, fadeAmount));
        //SDL_RenderDrawLines(renderer, linePoints, 5);
    }
}

void drawSpeedText(SDL_Surface *surface)
{
    if (Sans == NULL)
    {
        Sans = TTF_OpenFont("Mono.ttf", 42);
    }
    char score[10];
    sprintf(score, "%d", (int)playerSpeed);
    SDL_Surface *surfaceMessage = TTF_RenderText_Solid(Sans, score, TEXT_COLOR);
    SDL_Rect Message_rect;
    Message_rect.x = 0;
    Message_rect.y = -5;
    SDL_BlitSurface(surfaceMessage, NULL, surface, &Message_rect);
    SDL_FreeSurface(surfaceMessage);
}

void drawGameOverText(SDL_Surface *surface)
{
    SDL_Surface *surfaceMessage = TTF_RenderText_Solid(Sans, "GAME OVER", TEXT_COLOR);
    SDL_Rect Message_rect;
    Message_rect.x = WIDTH / 2 - 120;
    Message_rect.y = HEIGHT / 2 - 30;
    SDL_BlitSurface(surfaceMessage, NULL, surface, &Message_rect);
    SDL_FreeSurface(surfaceMessage);
}
