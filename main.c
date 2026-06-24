#include "include/raylib.h"
#include <math.h>
#include <stdbool.h>
#include <stdint.h>

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
    {1, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1},
    {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1},
    {1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1},
    {1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1},
    {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
    {1, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
};

struct ray castRay(double angle) {
    double x = player.x;
    double y = player.y;

    double dx = cos(angle);
    double dy = sin(angle);

    while (map[(int32_t) y][(int32_t) x] == 0) {
        x += dx * 0.01;
        y += dy * 0.01;
    }

    double distance = sqrt(pow(x - player.x, 2) + pow(y - player.y, 2));
    double wallHeight = (HEIGHT / 2.) / distance;

    return (struct ray) {
        .distance = distance,
        .wallHeight = wallHeight,
    };
}

void drawWallSlice(int32_t i, struct ray ray) {
    const int32_t sliceWidth = WIDTH / RAYS;
    int32_t color = (int32_t) (180 / ray.distance);
    
    if (color < 0) color = 0;
    if (color > COLOR_MAX) color = COLOR_MAX;
    
    DrawRectangle(i * sliceWidth, floor(HEIGHT / 2. - ray.wallHeight / 2), sliceWidth, ray.wallHeight, (Color) { color, color, color, 255 });
}

void raycast(void) {
    const double angleStep = player.fov / RAYS;
    for (int32_t i = 0; i < RAYS; i++) {
        const double angle = player.angle - (player.fov / 2) + i * angleStep;
        struct ray ray = castRay(angle);

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

    if (IsKeyDown(KEY_D))
        player.angle += ANGLE_SPEED;

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

