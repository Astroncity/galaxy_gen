#include "flecs.h"
#include "mathEx.h"
#include "render.h"
#include "state.h"
#include "transform.h"
#include "uiFramework.h"
#include "window.h"
#include <math.h>
#include <raylib.h>
#include <stdio.h>

ecs_world_t* world;

v2* mouse;
f32 time;
Font globalFont;

const u32 screenWidth = 640;
const u32 screenHeight = 360;

typedef bool(Equation(i32, i32));

typedef struct {
    Range domain;
    Range range;
    Equation* f;
    Color col;
} Grapher;

Grapher GrapherNew(Range domain, Range range, Equation* f, Color col) {
    return (Grapher){domain, range, f, col};
}

void GrapherDraw(Grapher* grapher) {
    for (i32 x = grapher->domain.min; x < grapher->domain.max; x++) {
        for (i32 y = grapher->range.min; y < grapher->range.max; y++) {
            if (grapher->f(x, y)) {
                DrawPixel(x, y, grapher->col);
            }
        }
    }
}

const f32 A = 190.0f;
const f32 B = 0.4f;
const f32 N = 7.4f;
const f32 C = 1.5f;

v2 polarToCartesian(f32 r, f32 theta) {
    v2 point;
    point.x = r * cosf(theta);
    point.y = r * sinf(theta);
    return point;
}

bool testEquation(i32 x, i32 y) {
    i32 centerX = 320;
    i32 centerY = 180;

    i32 res = (x - centerX) * (x - centerX) + (y - centerY) * (y - centerY);

    return LS_EQ(res, 100, 10);
}

void drawSpiral() {
    for (f32 theta = 0.0f; theta < 2.0f * PI * C; theta += 0.01f) {
        f32 r = A / logf(B * tanf(theta / (2.0f * N)));
        f32 r2 = -A / logf(B * tanf(theta / (2.0f * N)));

        v2 point = polarToCartesian(r, theta);
        v2 point2 = polarToCartesian(r2, theta);

        DrawCircle((i32)(point.x + screenWidth / 2),
                   (i32)(point.y + screenHeight / 2), 2, WHITE);

        DrawCircle((i32)(point2.x + screenWidth / 2),
                   (i32)(point2.y + screenHeight / 2), 2, WHITE);
    }
}

int main(void) {
    setWindowFlags();
    RenderTexture2D target = LoadRenderTexture(screenWidth, screenHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);

    globalFont = LoadFontEx("assets/fonts/spaceMono.ttf", 512, 0, 0);
    world = ecs_init();
    mouse = malloc(sizeof(v2));

    ECS_IMPORT(world, TransformModule);
    ECS_IMPORT(world, RendererModule);
    ECS_IMPORT(world, UIModule);

    Grapher test =
        GrapherNew(RangeNew(-500, 500), RangeNew(-500, 500), &testEquation, BLUE);

    while (!WindowShouldClose()) {
        f32 scale = getWindowScale();
        *mouse = getScreenMousePos(mouse, scale, screenWidth, screenHeight);

        BeginTextureMode(target);
        ClearBackground(BLACK);

        ecs_progress(world, GetFrameTime());
        GrapherDraw(&test);
        drawSpiral();

        EndTextureMode();
        BeginDrawing();
        ClearBackground(BLACK);
        drawScaledWindow(target, screenWidth, screenHeight, scale);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
