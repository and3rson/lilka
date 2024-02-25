-- Ця програма при кожному запуску збільшує лічильник на 1 та виводить його значення в консоль.

state = state or {} -- якщо це перший запуск, то state буде не визначений, тому ми ініціалізуємо його пустою таблицею
if state.counter == nil then
    state.counter = 0
end
state.counter = state.counter + 1
display.set_cursor(0, 32)
display.print('Лічильник запусків програми:')
display.print(state.counter)
