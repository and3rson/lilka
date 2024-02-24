---@meta

---@class display
local display = {}

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

---@class controller
local controller = {}

---Повертає таблицю зі станом контролера.
---Ця таблиця містить наступні поля:
---
---  * ``up``: стан кнопки "вгору"
---  * ``down``: стан кнопки "вниз"
---  * ``left``: стан кнопки "вліво"
---  * ``right``: стан кнопки "вправо"
---  * ``a``: стан кнопки "A"
---  * ``b``: стан кнопки "B"
---  * ``c``: стан кнопки "C"
---  * ``d``: стан кнопки "D"
---  * ``select``: стан кнопки "SELECT"
---  * ``start``: стан кнопки "START"
---
---Кожна поле має наступні підполя:
---
---  * ``pressed``: true, якщо кнопка натиснута
---  * ``just_pressed``: true, якщо кнопка щойно натиснута вперше з моменту попереднього виклику ``controller.get_state``
---  * ``just_released``: true, якщо кнопка щойно відпущена вперше з моменту попереднього виклику ``controller.get_state``
---
---@return table
---@usage
--- display.set_cursor(0, 32)
---
--- while true do
---     local state = controller.get_state()
---
---     if state.a.just_pressed then
---         display.print("[A] щойно натиснуто!")
---     elseif state.a.just_released then
---         display.print("[A] щойно відпущено!")
---     end
--- end
function controller.get_state() end

---@class resources
local resources = {}

---Завантажує BMP-зображення і повертає таблицю з ідентифікатором зображення (а також з його розмірами), яку можна використовувати для малювання зображення на екрані.
---Ця таблиця містить наступні поля:
---
---  * ``width``: ширина зображення
---  * ``height``: висота зображення
---  * ``pointer``: внутрішній ідентифікатор зображення
---
---@param filename string шлях до файлу зображення .BMP (відносно місця знаходження скрипта, що виконується)
---@param transparent_color? integer колір, який буде використаний для прозорості (5-6-5). Якщо цей параметр не вказаний, зображення буде виводитись без прозорості
---@return table
---@usage
--- local face = resources.load_bitmap("face.bmp", display.color565(0, 0, 0))
--- print(face.width, face.height) -- Виведе розміри зображення
--- display.draw_bitmap(face, 50, 80) -- Виведе зображення на екран у позицію (50, 80)
function resources.load_bitmap(filename, transparent_color) end

---@class util
local util = {}

---Затримує виконання скрипта на вказану кількість секунд.
---@usage
---@param ms number кількість секунд, на яку потрібно затримати виконання програми
--- display.set_cursor(0, 32)
--- display.print("Зачекайте півсекунди...")
--- util.delay(0.5) -- Затримує виконання програми на півсекунди.
--- display.print("Готово!")
function util.delay(ms) end

---Завершує виконання програми.
---@return nil
---@usage
--- function _update()
---     state = controller.get_state()
---     if state.a.pressed then
---         util.exit() -- Завершує виконання програми.
---     end
---     print('Staying alive!')
--- end
function util.exit() end

---@class math
---@field pi number Число π
---@field e number Число e
---@field tau number Число τ (2π)
local math = {}

---Повертає випадкове число.
---
---Якщо заданий лише один аргумент, повертає випадкове число в діапазоні [0;a] (включно).
---Якщо задані обидва аргументи, повертає випадкове число в діапазоні [a;b] (включно).
---@param a integer початок діапазону (включно)
---@param b? integer кінець діапазону (включно)
---@return integer
---@usage
--- local r = math.random(10, 20)
--- print(r) -- Виведе випадкове число від 10 до 20 (включно).
function math.random(a, b) end

---Обмежує число x в діапазоні між min та max (включно).
---
---@param x number число, яке потрібно обмежити
---@param min number мінімальне значення
---@param max number максимальне значення
---@return number
---@usage
--- print(math.clamp(8.1, 10, 20) -- Виведе 10
--- print(math.clamp(15.2, 10, 20) -- Виведе 15.2
--- print(math.clamp(23.3, 10, 20) -- Виведе 20
function math.clamp(x, min, max) end

---Лінійна інтерполяція.
---
---Повертає значення, яке лінійно інтерполюється між min та max з коефіцієнтом t.
---@param min number мінімальне значення
---@param max number максимальне значення
---@param t number коефіцієнт інтерполяції (від 0 до 1)
---@return number
---@usage
--- print(math.lerp(0, 100, 0.5) -- Виведе 50
--- print(math.lerp(0, 100, 0.25) -- Виведе 25
--- print(math.lerp(0, 100, 0.7125) -- Виведе 71.25
function math.lerp(min, max, t) end

---Перетворення значення з одного діапазону в інший.
---
---Повертає число ``x``, перетворене з діапазону [in_min;in_max] в діапазон [out_min;out_max].
---@param x number число, яке потрібно перетворити
---@param in_min number початок вхідного діапазону
---@param in_max number кінець вхідного діапазону
---@param out_min number початок вихідного діапазону
---@param out_max number кінець вихідного діапазону
---@return number
---@usage
--- print(math.map(50, 0, 100, 0, 1) -- Виведе 0.5
--- print(math.map(25, 0, 100, 0, 1) -- Виведе 0.25
--- print(math.map(71.25, 0, 100, 0, 1) -- Виведе 0.7125
function math.map(x, in_min, in_max, out_min, out_max) end

---Повертає відстань між точками (x1;y1) та (x2;y2).
---@param x1 number координата x першої точки
---@param y1 number координата y першої точки
---@param x2 number координата x другої точки
---@param y2 number координата y другої точки
---@return number
---@usage
--- print(math.dist(0, 0, 3, 4) -- Виведе 5
--- print(math.dist(0, 0, 1, 1) -- Виведе 1.4142135623731
function math.dist(x1, y1, x2, y2) end

---Повертає модуль числа ``x``.
---@param x number число
---@return number
function math.abs(x) end

---Повертає знак числа ``x``: -1, якщо число від'ємне, 0, якщо число дорівнює 0, 1, якщо число додатнє.
---@param x number число
---@return number
---@usage
--- print(math.sign(-5) -- Виведе -1
--- print(math.sign(5) -- Виведе 1
--- print(math.sign(0) -- Виведе 0
function math.sign(x) end

---Повертає квадратний корінь числа ``x``.
---@param x number число
---@return number
function math.sqrt(x) end

---Повертає число ``x`` в степені ``exp``.
---@param base number число
---@param exp number степінь
---@return number
---@usage
--- print(math.pow(2, 3) -- Виведе 8
--- print(math.pow(64, 0.5) -- Виведе 8
function math.pow(base, exp) end

---Повертає мінімальне значення з таблиці.
---@param values table таблиця чисел
---@return number
---@usage
--- print(math.min({1.1, 2.2, 3.3, 4.4, 5.5}) -- Виведе 1.1
--- print(math.min({5, 4, 3, 2, 1}) -- Виведе 1
--- print(math.min({-5, -4, -3, -2, -1}) -- Виведе -5
function math.min(values) end

---Повертає максимальне значення з таблиці.
---@param values table таблиця чисел
---@return number
---@usage
--- print(math.max({1.1, 2.2, 3.3, 4.4, 5.5}) -- Виведе 5.5
--- print(math.max({5, 4, 3, 2, 1}) -- Виведе 5
--- print(math.max({-5, -4, -3, -2, -1}) -- Виведе -1
function math.max(values) end

---Повертає суму всіх чисел з таблиці.
---@param values table таблиця чисел
---@return number
---@usage
--- print(math.sum({1, 2, 3, 4, 5}) -- Виведе 15
--- print(math.sum({1.1, 2.2, 3.3, 4.4, 5.5}) -- Виведе 16.5
--- print(math.sum({-5, -4, -3, -2, -1}) -- Виведе -15
function math.sum(values) end

---Повертає середнє значення всіх чисел з таблиці.
---@param values table таблиця чисел
---@return number
---@usage
--- print(math.avg({1, 2, 3, 4, 5}) -- Виведе 3
--- print(math.avg({1.1, 2.2, 3.3, 4.4}) -- Виведе 2.75
--- print(math.avg({-5, -4, -3, -2}) -- Виведе -3.5
function math.avg(values) end

---Округлює число ``x`` вниз.
---
---Повертає найбільше ціле число, яке менше або дорівнює ``x``.
---@param x number число
---@return integer
---@usage
--- print(math.floor(1.1) -- Виведе 1
--- print(math.floor(1.9) -- Виведе 1
--- print(math.floor(-1.1) -- Виведе -2
--- print(math.floor(-1.9) -- Виведе -2
function math.floor(x) end

---Округлює число ``x`` вгору.
---
---Повертає найменше ціле число, яке більше або дорівнює ``x``.
---@param x number число
---@return integer
---@usage
--- print(math.ceil(1.1) -- Виведе 2
--- print(math.ceil(1.9) -- Виведе 2
--- print(math.ceil(-1.1) -- Виведе -1
--- print(math.ceil(-1.9) -- Виведе -1
function math.ceil(x) end

---Округлює число ``x`` до найближчого цілого.
---@param x number число
---@return integer
---@usage
--- print(math.round(1.1) -- Виведе 1
--- print(math.round(1.9) -- Виведе 2
--- print(math.round(-1.1) -- Виведе -1
--- print(math.round(-1.9) -- Виведе -2
function math.round(x) end

---Повертає значення синуса кута ``x`` (в радіанах).
---@param x number кут (в радіанах)
---@return number
---@usage
--- print(math.sin(0) -- Виведе 0
--- print(math.sin(math.pi / 2) -- Виведе 1
function math.sin(x) end

---Повертає значення косинуса кута ``x`` (в радіанах).
---@param x number кут (в радіанах)
---@return number
---@usage
--- print(math.cos(0) -- Виведе 1
--- print(math.cos(math.pi) -- Виведе -1
function math.cos(x) end

---Повертає значення тангенса кута ``x`` (в радіанах).
---@param x number кут (в радіанах)
---@return number
---@usage
--- print(math.tan(0) -- Виведе 0
--- print(math.tan(math.pi / 4) -- Виведе 1
--- print(math.tan(math.pi / 2) -- Виведе максимальне значення, яке підтримується
function math.tan(x) end

---Повертає значення арксинуса числа ``x``.
---@param x number число
---@return number
function math.asin(x) end

---Повертає значення арккосинуса числа ``x``.
---@param x number число
---@return number
function math.acos(x) end

---Повертає значення арктангенса числа ``x``.
---@param x number число
---@return number
function math.atan(x) end

---Повертає значення арктангенса числа ``y``/``x``.
---Цей метод дуже зручно використовувати для обчислення кута між віссю x та точкою (x, y).
---@param y number число y
---@param x number число x
function math.atan2(y, x) end

---Повертає значення логарифму числа ``x``.
---@param x number число
---@param base? number основа логарифму (за замовчуванням - число ``e``)
---@return number
function math.log(x, base) end

---Перетворює кут ``x`` з радіанів в градуси.
---@param x number кут (в радіанах)
---@return number
function math.deg(x) end

---Перетворює кут ``x`` з градусів в радіани.
---@param x number кут (в градусах)
---@return number
function math.rad(x) end

---@alias _update fun(delta: number)
