#include "cube.h"

CubeApp::CubeApp() : App("Cube") {
}

typedef struct vec2 {
    float x, y;
} vec2;

typedef struct vec3 {
    float x, y, z;
} vec3;

vec2 add(vec2 a, vec2 b) {
    return {a.x + b.x, a.y + b.y};
}

vec2 neg(vec2 a) {
    return {-a.x, -a.y};
}

vec3 add(vec3 a, vec3 b) {
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}

vec3 mul(vec3 a, float f) {
    return {a.x * f, a.y * f, a.z * f};
}

float sqr(float a) {
    return a * a;
}

vec3 rotate(vec3 v, vec3 axis, float angle) {
    float s = sin(angle);
    float c = cos(angle);
    return {
        v.x * (c + sqr(axis.x) * (1 - c)) + v.y * (axis.x * axis.y * (1 - c) - axis.z * s) +
            v.z * (axis.x * axis.z * (1 - c) + axis.y * s),
        v.x * (axis.y * axis.x * (1 - c) + axis.z * s) + v.y * (c + sqr(axis.y) * (1 - c)) +
            v.z * (axis.y * axis.z * (1 - c) - axis.x * s),
        v.x * (axis.z * axis.x * (1 - c) - axis.y * s) + v.y * (axis.z * axis.y * (1 - c) + axis.x * s) +
            v.z * (c + sqr(axis.z) * (1 - c)),
    };
}

typedef struct line {
    vec3 a, b;
} line;

vec2 project(vec3 v, lilka::Canvas* canvas) {
    return {
        v.x / (v.z * 0.25f) * canvas->height() / 4.0f + canvas->width() / 2.0f,
        v.y / (v.z * 0.25f) * canvas->height() / 4.0f + canvas->height() / 2.0f,
    };
}

void drawLines(
    lilka::Canvas* canvas, const line* lines, int count, int thickness, float angle, float angle2, int32_t color
) {
    for (int i = 0; i < count; i++) {
        vec3 axis = rotate({0, 1, 0}, {0, 0, 1}, angle);
        vec3 a = rotate(lines[i].a, axis, angle2);
        vec3 b = rotate(lines[i].b, axis, angle2);
        a.z += 4;
        b.z += 4;

        vec2 aa = project(a, canvas);
        vec2 bb = project(b, canvas);

        if (color < 0) {
            color = canvas->color565(random(128, 255), random(128, 255), random(128, 255));
        }
        for (int y = 0; y < thickness; y++) {
            for (int x = 0; x < thickness; x++) {
                canvas->drawLine(aa.x + x, aa.y + y, bb.x + x, bb.y + y, color);
            }
        }
    }
}

void CubeApp::run() {
    line cube[] = {
        {{-1, -1, -1}, {-1, 1, -1}},
        {{-1, 1, -1}, {1, 1, -1}},
        {{1, 1, -1}, {1, -1, -1}},
        {{1, -1, -1}, {-1, -1, -1}},
        {{-1, -1, 1}, {-1, 1, 1}},
        {{-1, 1, 1}, {1, 1, 1}},
        {{1, 1, 1}, {1, -1, 1}},
        {{1, -1, 1}, {-1, -1, 1}},
        {{-1, -1, -1}, {-1, -1, 1}},
        {{-1, 1, -1}, {-1, 1, 1}},
        {{1, 1, -1}, {1, 1, 1}},
        {{1, -1, -1}, {1, -1, 1}},
    };

    constexpr uint8_t size = sizeof(cube) / sizeof(cube[0]);

    line cube2[size];
    for (int i = 0; i < size; i++) {
        cube2[i].a = mul(cube[i].a, 0.5);
        cube2[i].b = mul(cube[i].b, 0.5);
    }

    while (1) {
        lilka::State state = lilka::controller.getState();
        if (state.a.justPressed) {
            return;
        }

        float time = millis() / 1000.0f;

        canvas->fillScreen(0);
        drawLines(canvas, cube2, size, 1, (-time - 1) * 3.1415, (-time - 1) * 3.1415 * 1.3, 0xFFFF);
        drawLines(canvas, cube, size, 3, time * 3.1415, time * 3.1415 * 1.3, -1);
        queueDraw();
    }
}
