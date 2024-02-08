#include <lilka.h>

#include "letris_splash.h"

#define BLOCK_SIZE 10
#define FIELD_COLS 10
#define FIELD_ROWS 28

#define X_OFFSET LILKA_DISPLAY_WIDTH / 2 - FIELD_COLS *BLOCK_SIZE / 2

const bool shapesData[7][4][4] = {
    {{0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}}, // I
    {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}}, // O
    {{0, 0, 0, 0}, {0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}}, // S
    {{0, 0, 0, 0}, {1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}}, // Z
    {{0, 0, 0, 0}, {1, 1, 1, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}}, // T
    {{0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 1, 0}}, // L
    {{0, 0, 1, 0}, {0, 0, 1, 0}, {0, 0, 1, 0}, {0, 1, 1, 0}}, // J
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
        // Генерує нову фігуру. Нові фігури відображаються в вікні попереднього перегляду
        x = FIELD_COLS + 2;
        y = 2;
        rotation = 0;
        uint8_t shapeIndex = random(0, 7);
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                this->shapeData[i][j] = shapesData[shapeIndex][i][j];
            }
        }
        color = colors[random(0, colorCount)];
    }

    void moveToField() {
        // Переміщує фігуру в поле
        x = FIELD_COLS / 2 - 2;
        y = 0;
    }

    void draw(bool drawEmptyBlocks = false) {
        for (int yy = 0; yy < 4; yy++) {
            for (int xx = 0; xx < 4; xx++) {
                if (this->shapeData[yy][xx]) {
                    canvas->fillRect(X_OFFSET + (this->x + xx) * BLOCK_SIZE, (this->y + yy) * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, color);
                    canvas->fillRect(X_OFFSET + (this->x + xx) * BLOCK_SIZE + 2, (this->y + yy) * BLOCK_SIZE + 2, BLOCK_SIZE - 4, BLOCK_SIZE - 4, lilka::display.color565(0, 0, 0));
                } else if (drawEmptyBlocks) {
                    canvas->fillRect(X_OFFSET + (this->x + xx) * BLOCK_SIZE, (this->y + yy) * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, lilka::display.color565(0, 0, 0));
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
        // Малює поле
        for (int y = 0; y < FIELD_ROWS; y++) {
            for (int x = 0; x < FIELD_COLS; x++) {
                canvas->fillRect(X_OFFSET + x * BLOCK_SIZE, y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, this->blocks[y][x]);
            }
        }
    }
    void addShape(Shape *shape) {
        // Додає фігуру як частину поля
        for (int yy = 0; yy < 4; yy++) {
            for (int xx = 0; xx < 4; xx++) {
                if (shape->shapeData[yy][xx]) {
                    this->blocks[shape->y + yy][shape->x + xx] = shape->color;
                }
            }
        }
        // Перевіряємо, чи є рядки, які можна видалити
        for (int y = 0; y < FIELD_ROWS; y++) {
            bool full = true;
            for (int x = 0; x < FIELD_COLS; x++) {
                if (!this->blocks[y][x]) {
                    full = false;
                    break;
                }
            }
            if (full) {
                // Видаляємо рядок
                for (int yy = y; yy > 0; yy--) {
                    for (int xx = 0; xx < FIELD_COLS; xx++) {
                        this->blocks[yy][xx] = this->blocks[yy - 1][xx];
                    }
                }
                for (int xx = 0; xx < FIELD_COLS; xx++) {
                    this->blocks[0][xx] = 0;
                }
            }
        }
    }
    bool willCollide(Shape *shape, int dx, int dy) {
        // Повертає true, якщо фігура зіткнеться з іншими блоками, якщо зміститься на (dx, dy)
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

void demo_letris() {
    // Створюємо Canvas для буферизації малюнку
    lilka::Canvas canvas;
    canvas.begin();
    // Створюємо поле та фігуру
    Field field(&canvas);
    Shape shape(&canvas);
    Shape nextShape(&canvas);
    nextShape.reset();

    // Вітання
    while (!lilka::controller.getState().start.justPressed) {
        float time = millis() / 1000.0;
        canvas.fillScreen(canvas.color565(0, 0, 0));
        float yShifts[letris_splash_width];
        for (uint16_t x = 0; x < letris_splash_width; x++) {
            yShifts[x] = cos(time + ((float)x) / 32.0) * 8;
        }
        for (uint16_t y = 0; y < letris_splash_height; y++) {
            int16_t xShift = sin(time * 4 + y / 8.0) * 4;
            for (uint16_t x = 0; x < letris_splash_width; x++) {
                canvas.drawPixel(x + xShift, LILKA_DISPLAY_HEIGHT / 2 - letris_splash_height / 2 + y + yShifts[x], letris_splash[y * letris_splash_width + x]);
            }
        }
        lilka::display.renderCanvas(canvas);
    }

    // Очищаємо екран
    canvas.fillScreen(canvas.color565(32, 32, 32));

    // Головний цикл гри
    while (1) {
        // Створюємо нову фігуру
        shape = nextShape;
        shape.moveToField();
        nextShape.reset();

        // Перевіряємо, чи фігура має місце для руху
        if (field.willCollide(&shape, 0, 0)) {
            // Ні, гра закінчилася - фігура вже зіткнулася з іншими блоками
            break;
        }

        bool fastDrop = false;
        while (1) {
            // Починаємо рух фігури
            int nextMove = millis() + (fastDrop ? 25 : 500);

            // Затримуємо рух фігури, поки не наступив час для наступного кроку
            while (millis() < nextMove) {
                // Обробляємо ввід
                lilka::State state = lilka::controller.getState();
                int8_t dx = 0, dy = 0;
                if (state.up.justPressed) { // TODO: Change to left
                    // Користувач натиснув вліво
                    dx = -1;
                } else if (state.down.justPressed) { // TODO: Change to right
                    // Користувач натиснув вправо
                    dx = 1;
                } else if (state.start.justPressed && !fastDrop) { // TODO: Change to down
                    // Користувач натиснув вниз
                    fastDrop = true;
                    nextMove = 0;
                }
                // Чи може фігура рухатися в горизонтально?
                if (field.willCollide(&shape, dx, 0)) {
                    // Ні, фігура зіткнеться зі стіною. Змінюємо напрямок руху на 0
                    dx = 0;
                }
                // Рухаємо фігуру горизонтально
                shape.move(dx, 0);
                // Малюємо поле та фігуру
                field.draw();
                shape.draw();
                nextShape.draw(true);
                // Відображаємо зміни на екрані
                lilka::display.renderCanvas(canvas);
            }

            // Перевіряємо, чи може фігура рухатися вниз
            if (field.willCollide(&shape, 0, 1)) {
                // Ні, фігура зіткнеться з іншими блоками. Додаємо фігуру до поля
                field.addShape(&shape);
                break;
            } else {
                // Так, фігура може рухатися вниз. Рухаємо фігуру вниз
                shape.move(0, 1);
            }
            // Фігура стала частиною поля. Починаємо новий цикл
        }
    }

    // Гра закінчилася. Виводимо повідомлення на екран
    lilka::ui_alert("Game over", "Гру завершено!\nТи намагався. :)");
}
