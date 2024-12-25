
-- Завантажуємо зображення
no = resources.load_image("no.bmp")
left = resources.load_image("left.bmp")
right = resources.load_image("right.bmp")
both = resources.load_image("both.bmp")

-- Створюємо змінну в яку буде записуватись стан кнопок
state = controller.get_state()

function lilka.update(delta)

    -- Оновлюємо змінну з станом кнопок    
    state = controller.get_state()

    -- Якщо була натиснута кнопка - відтворюємо звук
    if state.b.just_pressed or state.d.just_pressed then
        buzzer.play(40, 100)
    end

    -- Завершуємо програму при натисканні кнопки "A"
    if state.a.pressed then
        util.exit()
    end
end

function lilka.draw()

    -- Перевіряємо стан кнопок і відмальовуємо відповідне зображення 
    display.draw_image(no, 0, 0)
    if state.d.pressed then
        display.draw_image(left, 0, 0)
    end
    if state.b.pressed then
        display.draw_image(right, 0, 0)
    end
    if state.b.pressed and state.d.pressed then
        display.draw_image(both, 0, 0)
    end
    if state.c.pressed then
        util.sleep(0.1)
        display.draw_image(both, 0, 0)
        buzzer.play(40, 100)
        display.queue_draw()
        util.sleep(0.1)
        display.draw_image(no, 0, 0)
    end
end
