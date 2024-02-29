-- Ця програма при кожному запуску збільшує лічильник на 1 та виводить його значення в консоль.

state = state or {} -- якщо це перший запуск, то state буде не визначений, тому ми ініціалізуємо його пустою таблицею
if state.counter == nil then
    state.counter = 0
end
state.counter = state.counter + 1
display.fill_screen(display.color565(64, 0, 64))
display.set_cursor(0, 64)
display.print('Лічильник запусків\nпрограми: ', state.counter)
display.render()
util.sleep(0.5)
