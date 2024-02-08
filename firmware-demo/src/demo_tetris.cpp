#include <lilka.h>

#define BLOCK_SIZE 10
#define FIELD_COLS 10
#define FIELD_ROWS 28

#define X_OFFSET LILKA_DISPLAY_WIDTH / 2 - FIELD_COLS *BLOCK_SIZE / 2

const bool shapesData[7][4][4] = {
    {{0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}}, // I
    {{1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, // O
    {{0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, // S
    {{1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, // Z
    {{1, 1, 1, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, // T
    {{1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, // L
    {{1, 1, 1, 0}, {1, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, // J
};

const uint16_t colors[7] = {
    lilka::display.color565(255, 0, 0), lilka::display.color565(0, 255, 0), lilka::display.color565(0, 0, 255), lilka::display.color565(255, 255, 0), lilka::display.color565(255, 0, 255), lilka::display.color565(0, 255, 255), lilka::display.color565(200, 200, 200),
};
const uint8_t colorCount = sizeof(colors) / sizeof(colors[0]);

class Shape {
public:
    int x, y;
    int shapeData[4][4];
    uint16_t color;
    int rotation;

    Shape(Arduino_Canvas *canvas) : canvas(canvas) {}

    void reset() {
        x = FIELD_COLS / 2 - 2;
        y = 0;
        rotation = 0;
        uint8_t shapeIndex = random(0, 7);
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                this->shapeData[i][j] = shapesData[shapeIndex][i][j];
            }
        }
        color = colors[random(0, colorCount)];
    }

    void draw() {
        for (int yy = 0; yy < 4; yy++) {
            for (int xx = 0; xx < 4; xx++) {
                if (this->shapeData[yy][xx]) {
                    canvas->fillRect(X_OFFSET + (this->x + xx) * BLOCK_SIZE, (this->y + yy) * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, color);
                    canvas->fillRect(X_OFFSET + (this->x + xx) * BLOCK_SIZE + 2, (this->y + yy) * BLOCK_SIZE + 2, BLOCK_SIZE - 4, BLOCK_SIZE - 4, lilka::display.color565(0, 0, 0));
                }
            }
        }
    }

    bool canMove(int dx, int dy) {
        // TODO
        return true;
    }

    void move(int dx, int dy) {
        if (this->canMove(dx, dy)) {
            this->x += dx;
            this->y += dy;
        }
    }

private:
    Arduino_Canvas *canvas;
};

class Field {
public:
    Field(Arduino_Canvas *canvas) : canvas(canvas) {
        for (int y = 0; y < FIELD_ROWS; y++) {
            for (int x = 0; x < FIELD_COLS; x++) {
                this->blocks[y][x] = 0;
            }
        }
    }
    void draw() {
        for (int y = 0; y < FIELD_ROWS; y++) {
            for (int x = 0; x < FIELD_COLS; x++) {
                canvas->fillRect(X_OFFSET + x * BLOCK_SIZE, y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, this->blocks[y][x]);
            }
        }
    }
    void addShape(Shape *shape) {
        for (int yy = 0; yy < 4; yy++) {
            for (int xx = 0; xx < 4; xx++) {
                if (shape->shapeData[yy][xx]) {
                    this->blocks[shape->y + yy][shape->x + xx] = shape->color;
                }
            }
        }
    }
    bool willCollide(Shape *shape, int dx, int dy) {
        for (int yy = 0; yy < 4; yy++) {
            for (int xx = 0; xx < 4; xx++) {
                if (shape->shapeData[yy][xx]) {
                    if (shape->x + xx + dx < 0 || shape->x + xx + dx >= FIELD_COLS || shape->y + yy + dy >= FIELD_ROWS || this->blocks[shape->y + yy + dy][shape->x + xx + dx]) {
                        return true;
                    }
                }
            }
        }
        return false;
    }

private:
    uint16_t blocks[FIELD_ROWS][FIELD_COLS]; // Black color means no block
    Arduino_Canvas *canvas;
};

void demo_tetris() {
    while (lilka::controller.state().start) {
    };
    lilka::Canvas canvas;
    canvas.begin();
    canvas.fillScreen(canvas.color565(32, 32, 32));
    Field field(&canvas);
    Shape shape(&canvas);
    lilka::State prevState = lilka::controller.state();
    while (1) {
        shape.reset();

        bool fastDrop = false;
        while (1) {
            int nextMove = millis() + (fastDrop ? 50 : 500);

            while (millis() < nextMove) {
                lilka::State state = lilka::controller.state();
                int8_t dx = 0, dy = 0;
                if (state.up && !prevState.up) { // TODO: Change to left
                    dx = -1;
                } else if (state.down && !prevState.down) { // TODO: Change to right
                    dx = 1;
                } else if (state.start && !prevState.start) { // TODO: Change to down
                    fastDrop = true;
                    nextMove = 0;
                }
                if (field.willCollide(&shape, dx, 0)) {
                    // Cannot move horizontally
                    dx = 0;
                }
                shape.move(dx, 0);
                prevState = state;
                field.draw();
                shape.draw();
                lilka::display.renderCanvas(canvas);
            }
            if (field.willCollide(&shape, 0, 1)) {
                // Cannot move down, add to field
                field.addShape(&shape);
                break;
            } else {
                shape.move(0, 1);
            }
        }
    }
}
