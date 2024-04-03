---@meta

---@class resources
resources = {}

---Завантажує BMP-зображення і повертає таблицю з ідентифікатором зображення (а також з його розмірами), яку можна використовувати для малювання зображення на екрані.
---
---Ця таблиця містить наступні поля:
---
---  * ``width``: ширина зображення
---  * ``height``: висота зображення
---  * ``pointer``: внутрішній ідентифікатор зображення
---
---@param filename string шлях до файлу зображення .BMP (відносно місця знаходження скрипта, що виконується)
---@param transparent_color? integer колір, який буде використаний для прозорості (5-6-5). Якщо цей параметр не вказаний, зображення буде виводитись без прозорості
---@param pivotX? integer X-координата центру зображення (за замовчуванням це середина зображення)
---@param pivotY? integer Y-координата центру зображення (за замовчуванням це середина зображення)
---@return table
---@usage
--- local face = resources.load_image("face.bmp", display.color565(0, 0, 0))
--- print(face.width, face.height) -- Виведе розміри зображення
--- display.draw_image(face, 50, 80) -- Виведе зображення на екран у позицію (50, 80)
function resources.load_image(filename, transparent_color, pivotX, pivotY) end

---Повертає зображення на певну кількість градусів за годинниковою стрілкою навколо його центру.
---
---@param image table ідентифікатор зображення
---@param angle integer кут (в градусах)
---@param blank_color integer колір для пікселів, які залишаться незаповненими
function resources.rotate_image(image, angle, blank_color) end

---Відображає зображення горизонтально. Зручно використовувати для платформерів, де герой може рухатись вліво та вправо.
---
---@param image table ідентифікатор зображення
function resources.flip_image_x(image) end

---Відображає зображення вертикально.
---
---@param image table ідентифікатор зображення
function resources.flip_image_y(image) end

---Читає вміст файлу і повертає його як текст.
---@param filename string шлях до файлу (відносно місця знаходження скрипта, що виконується)
---@return string
---@usage
--- local content = resources.read_file("file.txt")
--- print(content) -- Виведе вміст файлу
function resources.read_file(filename) end

---Записує текст у файл.
---@param filename string шлях до файлу (відносно місця знаходження скрипта, що виконується)
---@param content string текст, який буде записаний у файл
---@usage
--- resources.write_file("file.txt", "Hello, world!")
function resources.write_file(filename, content) end

return resources
