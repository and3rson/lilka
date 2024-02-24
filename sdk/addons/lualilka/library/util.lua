---@meta

---@class util
util = {}

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

return util
