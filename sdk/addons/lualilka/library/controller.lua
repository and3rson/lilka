---@meta

---@class controller
controller = {}

---Повертає таблицю зі станом контролера.
---
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

return controller
