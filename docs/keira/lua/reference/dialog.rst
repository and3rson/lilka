``UIs``: Діалоги
--------------------

Функції для створення та управління діалогами.

Приклад:

.. code-block:: lua
    :linenos:
    -- Створення діалогу для введення тексту
    local keyboard_test = keyboardUI("Скажи моє ім'я") 
    keyboard_test:setValue("TEST")
    while true do      
        keyboard_test:draw()      
        keyboard_test:update()  
        if keyboard_test:isFinished() then
            break
        end
    end

    -- Створення сповіщення
    local alert_test = alertUI("Хто ти?", keyboard_test:getValue()) 
    alert_test:draw()      
    while true do      
        alert_test:update()     
        if alert_test:isFinished() then
            break
        end
    end

    alert_test:setTitle("НІ!!! Ти є:")
    alert_test:setMessage("НАЙКРАЩИЙ У СВІТІ")

    -- Створення діалогу з кнопками
    local alert_test = alertUI("Бажаєш пограти?", "[A]ТАК\n[B]НІ") 
    alert_test:addActivationButton("a")
    alert_test:addActivationButton("b")
    alert_test:draw()      
    while true do   
        alert_test:update()     
        if alert_test:isFinished() then
            break
        end
    end

    which_key = alert_test:getButton()

    if which_key == "a" then
        alert_test:setTitle("ГАРАЗД")
        alert_test:setMessage("ПОГРАЙМО")
    elseif which_key == "b" then
        alert_test:setTitle("Добре")
        alert_test:setMessage("...")
    end

    -- Створення діалогу прогресу
    local progress_test = progressUI("Ракета", "Запуск") 
    for i = 0, 100 do
        progress_test:setProgress(i)
        progress_test:draw()
    end
.. lua:autoclass:: keyboardUI
.. lua:autoclass:: alertUI
.. lua:autoclass:: progressUI
