#include "include/raylib.h"
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define WIDTH 950
#define HEIGHT 600
#define RAYS WIDTH
#define SPEED 0.025
#define ANGLE_SPEED 0.05
#define MAP_SIZE 20
#define COLOR_MAX 180

struct player {
    double x, y;
    double angle, fov;
};

struct ray {
    double distance, wallHeight;
    uint8_t type;
};

struct player player = {
    .x = 1,
    .y = 1,
    .angle = 0,
    .fov = PI / 2,
};

int map[MAP_SIZE][MAP_SIZE] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 2, 2, 0, 0, 0, 3, 3, 0, 0, 4, 4, 0, 0, 0, 1, 1, 0, 1},
    {1, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 0, 0, 0, 0, 0, 4, 0, 0, 4, 0, 0, 0, 0, 0, 2, 0, 1},
    {1, 0, 1, 0, 0, 0, 0, 0, 4, 0, 0, 4, 0, 0, 0, 0, 0, 2, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 3, 0, 0, 0, 0, 0, 4, 0, 0, 4, 0, 0, 0, 0, 0, 1, 0, 1},
    {1, 0, 3, 0, 0, 0, 0, 0, 4, 0, 0, 4, 0, 0, 0, 0, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 1},
    {1, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
    {1, 0, 4, 4, 0, 0, 0, 2, 2, 0, 0, 3, 3, 0, 0, 0, 1, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
};

Color getColor(uint8_t type, uint8_t shade) {
    // 0 is empty space, so it doesn't enter here
    switch (type) {
        case 1: return (Color) { shade, shade, shade, 255 };
        case 2: return (Color) { shade, 0, shade, 255 };
        case 3: return (Color) { 0, shade, shade, 255 };
        case 4: return (Color) { shade, shade, 0, 255 };

        default:
            fprintf(stderr, "Invalid color type: %d", type);
            exit(EXIT_FAILURE);
    }
}

struct ray castRay(double angle) {
    double x = player.x;
    double y = player.y;

    double dx = cos(angle);
    double dy = sin(angle);

    while (map[(int32_t) y][(int32_t) x] == 0) {
        x += dx * 0.01;
        y += dy * 0.01;

        if (x < 0 || x >= MAP_SIZE || y < 0 || y >= MAP_SIZE)
            break;
    }

    double distance = sqrt(pow(x - player.x, 2) + pow(y - player.y, 2));
    double wallHeight = (HEIGHT / 2.) / distance;

    if (wallHeight > HEIGHT)
        wallHeight = HEIGHT;

    return (struct ray) {
        .distance = distance,
        .wallHeight = wallHeight,
        .type = map[(int32_t) y][(int32_t) x],
    };
}

void drawWallSlice(int32_t i, struct ray ray) {
    const int32_t sliceWidth = WIDTH / RAYS;
    int32_t shade = (int32_t) (180 / ray.distance) * 2;
    
    // this ensures it fits in a uint8_t. casting is safe
    if (shade < 0) shade = 0;
    if (shade > COLOR_MAX) shade = COLOR_MAX;
    
    int32_t height = ray.wallHeight * ray.type;
    DrawRectangle(i * sliceWidth, floor(HEIGHT / 2. - ray.wallHeight / 2.) - (height - ray.wallHeight), sliceWidth, height, getColor(ray.type, (uint8_t) shade));
}

void raycast(void) {
    const double angleStep = player.fov / RAYS;
    for (int32_t i = 0; i < RAYS; i++) {
        struct ray ray = castRay(player.angle - (player.fov / 2) + i * angleStep);
        drawWallSlice(i, ray);
    }
}
// ---

bool isColliding(void) {
    int32_t floorX = (int32_t) player.x;
    int32_t floorY = (int32_t) player.y;

    if (
        floorX < 0 ||
        floorX >= MAP_SIZE ||
        
        floorY < 0 ||
        floorY >= MAP_SIZE
    )
        return true;

    return map[floorY][floorX] != 0;
}

void input(void) {
    double oldX = player.x;
    double oldY = player.y;
    
    double dx = cos(player.angle);
    double dy = sin(player.angle);

    if (IsKeyDown(KEY_W)) {
        player.x += dx * SPEED;
        player.y += dy * SPEED;
    }

    else if (IsKeyDown(KEY_S)) {
        player.x -= dx * SPEED;
        player.y -= dy * SPEED;
    }

    if (IsKeyDown(KEY_A))
        player.angle -= ANGLE_SPEED;
    else if (IsKeyDown(KEY_D))
        player.angle += ANGLE_SPEED;

    if (player.angle < 0)
        player.angle += 2 * PI;
    else if (player.angle > 2 * PI)
        player.angle -= 2 * PI;

    if (isColliding()) {
        player.x = oldX;
        player.y = oldY;
    }
}

void environment(void) {
    Color c = (Color) { COLOR_MAX, COLOR_MAX, COLOR_MAX, 255 };

    DrawRectangleGradientV(0, 0, WIDTH, HEIGHT / 2, c, BLACK);
    DrawRectangleGradientV(0, HEIGHT / 2, WIDTH, HEIGHT / 2, BLACK, c);
}

void render(void) {
    // environment();
    raycast();
}
void clear(void) {
    DrawRectangle(0, 0, WIDTH, HEIGHT, BLACK);
}

int main(void) {
    InitWindow(WIDTH, HEIGHT, "Raycast!");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        input();
        
        BeginDrawing();
        clear();
        render();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
