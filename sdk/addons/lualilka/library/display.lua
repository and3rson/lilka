---@meta

---@class display
---@field width integer ширина дисплею в пікселях
---@field height integer висота дисплею в пікселях
display = {}

---Повертає 16-бітне значення кольору
---
---Наприклад, ``display.color565(255, 0, 0)`` поверне 63488, що відповідає червоному кольору.
---Це потрібно, оскільки дисплей Лілки працює з 16-бітними кольорами (5-6-5), а не з 24-бітними (8-8-8).
---
---Всі функції, що приймають аргумент ``color``, очікують, що він буде саме 16-бітним числом.
---
---@param r integer червоний (0-255)
---@param g integer зелений (0-255)
---@param b integer синій (0-255)
---@return integer 16-бітне значення кольору (5-6-5)
---@usage
--- local color = display.color565(255, 0, 0)
--- display.draw_line(0, 0, 100, 100, color)
function display.color565(r, g, b) end

---Встановлює позицію курсора.
---Позиція курсора використовується для виведення тексту на екран.
---@param x integer координата x
---@param y integer координата y
function display.set_cursor(x, y) end

---Встановлює шрифт для виведення тексту.
---Шрифт, який буде використовуватися для виведення тексту.
---Доступні шрифти:
---
---* ``"4x6"``: https://github.com/olikraus/u8g2/wiki/fntgrpx11#4x6
---* ``"5x7"``: https://github.com/olikraus/u8g2/wiki/fntgrpx11#5x7
---* ``"5x8"``: https://github.com/olikraus/u8g2/wiki/fntgrpx11#5x8
---* ``"6x12"``: https://github.com/olikraus/u8g2/wiki/fntgrpx11#6x12
---* ``"6x13"``: https://github.com/olikraus/u8g2/wiki/fntgrpx11#6x13
---* ``"7x13"``: https://github.com/olikraus/u8g2/wiki/fntgrpx11#7x13
---* ``"8x13"``: https://github.com/olikraus/u8g2/wiki/fntgrpx11#8x13
---* ``"9x15"``: https://github.com/olikraus/u8g2/wiki/fntgrpx11#9x15
---* ``"10x20"``: https://github.com/olikraus/u8g2/wiki/fntgrpx11#10x20
---
---@param font string
---@usage
--- display.set_font("6x13")
--- display.set_cursor(8, 32)
--- display.print("Привіт,")
--- display.set_cursor(8, 64)
--- display.set_font("10x20")
--- display.print("Лілка!")
function display.set_font(font) end

---Встановлює масштаб тексту.
---Якщо цей параметр дорівнює 1, текст виводиться в масштабі 1:1. Якщо 2, то кожен піксель тексту буде займати 2x2 пікселі на екрані, і так далі.
---@param size integer масштаб тексту
function display.set_text_size(size) end

---Виводить текст на екран.
---@param ... any значення для виведення
---@usage
--- display.set_cursor(8, 32)
--- display.print("Hello,", "world!", 69, 420, "nice")
function display.print(...) end

---Заповнює екран вказаним кольором.
---@param color integer Колір (5-6-5)
---@usage
--- display.fill_screen(display.color565(255, 0, 0)) -- заповнює екран червоним кольором
function display.fill_screen(color) end

---Малює піксель на екрані.
---@param x integer координата x
---@param y integer координата y
---@param color integer Колір (5-6-5)
function display.draw_pixel(x, y, color) end

---Малює лінію на екрані.
---@param x1 integer координата x початку лінії
---@param y1 integer координата y початку лінії
---@param x2 integer координата x кінця лінії
---@param y2 integer координата y кінця лінії
---@param color integer Колір (5-6-5)
---@usage
--- display.draw_line(0, 0, 100, 100, display.color565(255, 0, 0)) -- малює червону лінію від (0, 0) до (100, 100)
function display.draw_line(x1, y1, x2, y2, color) end

---Малює контур прямокутника з координатами (x, y), розмірами (w, h) та кольором color.
---@param x integer координата x початку прямокутника
---@param y integer координата y початку прямокутника
---@param w integer ширина прямокутника
---@param h integer висота прямокутника
---@param color integer Колір (5-6-5)
function display.draw_rect(x, y, w, h, color) end

---Малює заповнений прямокутник з координатами (x, y), розмірами (w, h) та кольором color.
---@param x integer координата x початку прямокутника
---@param y integer координата y початку прямокутника
---@param w integer ширина прямокутника
---@param h integer висота прямокутника
---@param color integer Колір (5-6-5)
function display.fill_rect(x, y, w, h, color) end

---Малює контур кола з центром в точці (x, y), радіусом r та кольором color.
---@param x integer координата x центру кола
---@param y integer координата y центру кола
---@param r integer радіус кола
---@param color integer Колір (5-6-5)
function display.draw_circle(x, y, r, color) end

---Малює заповнене коло з центром в точці (x, y), радіусом r та кольором color.
---@param x integer координата x центру кола
---@param y integer координата y центру кола
---@param r integer радіус кола
---@param color integer Колір (5-6-5)
function display.fill_circle(x, y, r, color) end

---Малює контур трикутника з вершинами в точках (x1, y1), (x2, y2), (x3, y3) та кольором color.
---@param x1 integer координата x першої точки
---@param y1 integer координата y першої точки
---@param x2 integer координата x другої точки
---@param y2 integer координата y другої точки
---@param x3 integer координата x третьої точки
---@param y3 integer координата y третьої точки
---@param color integer Колір (5-6-5)
function display.draw_triangle(x1, y1, x2, y2, x3, y3, color) end

---Малює заповнений трикутник з вершинами в точках (x1, y1), (x2, y2), (x3, y3) та кольором color.
---@param x1 integer координата x першої точки
---@param y1 integer координата y першої точки
---@param x2 integer координата x другої точки
---@param y2 integer координата y другої точки
---@param x3 integer координата x третьої точки
---@param y3 integer координата y третьої точки
---@param color integer Колір (5-6-5)
function display.fill_triangle(x1, y1, x2, y2, x3, y3, color) end

---Малює контур еліпса з центром в точці (x, y), радіусами rx та ry та кольором color.
---@param x integer координата x центру еліпса
---@param y integer координата y центру еліпса
---@param rx integer радіус еліпса по осі x
---@param ry integer радіус еліпса по осі y
---@param color integer Колір (5-6-5)
function display.draw_elipse(x, y, rx, ry, color) end

---Малює заповнений еліпс з центром в точці (x, y), радіусами rx та ry та кольором color.
---@param x integer координата x центру еліпса
---@param y integer координата y центру еліпса
---@param rx integer радіус еліпса по осі x
---@param ry integer радіус еліпса по осі y
---@param color integer Колір (5-6-5)
function display.fill_elipse(x, y, rx, ry, color) end

---Малює контур дуги з центром в точці (x, y), зовнішнім радіусом r1, внутрішнім радіусом r2, початковим кутом start_angle, кінцевим кутом end_angle та кольором color.
---@param x integer координата x центру дуги
---@param y integer координата y центру дуги
---@param r1 integer зовнішній радіус дуги
---@param r2 integer внутрішній радіус дуги
---@param start_angle integer початковий кут дуги (в градусах)
---@param end_angle integer кінцевий кут дуги (в градусах)
---@param color integer Колір (5-6-5)
function display.draw_arc(x, y, r1, r2, start_angle, end_angle, color) end

---Малює заповнену дугу з центром в точці (x, y), зовнішнім радіусом r1, внутрішнім радіусом r2, початковим кутом start_angle, кінцевим кутом end_angle та кольором color.
---@param x integer координата x центру дуги
---@param y integer координата y центру дуги
---@param r1 integer зовнішній радіус дуги
---@param r2 integer внутрішній радіус дуги
---@param start_angle integer початковий кут дуги (в градусах)
---@param end_angle integer кінцевий кут дуги (в градусах)
---@param color integer Колір (5-6-5)
function display.fill_arc(x, y, r1, r2, start_angle, end_angle, color) end

---Малює зображення на екрані.
---@param bitmap table ідентифікатор зображення, отриманий з :lua:func:`resources.load_bitmap`
---@param x integer координата x лівого верхнього кута зображення
---@param y integer координата y лівого верхнього кута зображення
---@usage
--- local bitmap = resource.load_bitmap("face.bmp", display.color565(0, 0, 0))
--- display.draw_bitmap(bitmap, 50, 80) -- малює зображення в точці (50, 80)
function display.draw_bitmap(bitmap, x, y) end

---Буферизація дисплея дозволяє малювати всю графіку на полотні-буфері в пам'яті, а потім виводити цей буфер на екран. Це дозволяє уникнути мерехтіння зображення на екрані.
---Щоб вивести буфер на екран, використовуйте функцію :lua:func:`display.render`.
---За замовчуванням, буферизація екрану **увімкнена**, щоб запобігти мерехтінню зображення на екрані. Це трохи збільшує затримку між малюванням графіки та виведенням її на екран, але відсутність мерехтіння зображення на екрані зазвичай в пріоритеті.
---Приклад коду, при якому буде дуже помітне мерехтіння зображення на екрані:
---@param value boolean якщо цей параметр дорівнює true, буферизація дисплея вмикається. Якщо false, вимикається.
---@usage
--- -- Приклад коду з (за замовчуванням) увімкненою буферизацією екрану:
--- while true do
---     -- Заповнюємо екран чорним кольором:
---     display.fill_screen(display.color565(0, 0, 0))
---     -- Малюємо червоний прямокутник на екрані:
---     display.fill_rect(0, 0, 100, 100, display.color565(255, 0, 0))
---     -- Виводимо зображення з буфера на екран:
---     display.render()
---     -- Жодного мерехтіння не буде!
--- end
---
--- -- Вимикаємо буферизацію екрану:
--- display.set_buffered(false)
---
--- while true do
---     -- Заповнюємо екран чорним кольором:
---     display.fill_screen(display.color565(0, 0, 0))
---     -- Малюємо червоний прямокутник на екрані:
---     display.fill_rect(0, 0, 100, 100, display.color565(255, 0, 0))
--- end
--- -- Якщо ви виконаєте цей код, ви побачите, що червоний прямокутник мерехтить на екрані.
function display.set_buffered(value) end

---Виводить зображення з буфера на екран.
---Якщо буферизація екрану вимкнена, ця функція повертає помилку.
---@return nil
function display.render() end

return display
