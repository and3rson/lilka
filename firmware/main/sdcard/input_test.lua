COORDS = {
    up = { 2, 1, 2, 2 },
    down = { 2, 5, 2, 2 },
    left = { 0, 3, 2, 2 },
    right = { 4, 3, 2, 2 },

    a = { 12, 3, 2, 2 },
    b = { 10, 5, 2, 2 },
    c = { 10, 1, 2, 2 },
    d = { 8, 3, 2, 2 },

    select = { 4, 8, 2, 1 },
    start = { 8, 8, 2, 1 },
}

BLACK = display.color565(0, 0, 0)
GRAY = display.color565(128, 128, 128)
GREEN = display.color565(0, 255, 0)

local state

function lilka.update()
    state = controller.get_state()

    if state.up.pressed and state.down.pressed and state.left.pressed and state.right.pressed and state.start.pressed then
        util.exit()
    end
end

function lilka.draw()
    display.fill_screen(BLACK)
    for k, v in pairs(COORDS) do
        fill_cells(v[1], v[2], v[3], v[4], state[k].pressed and GREEN or GRAY)
    end
    display.set_cursor(16, display.height - 32);
    display.print("Натисніть всі стрілки");
    display.set_cursor(16, display.height - 16);
    display.print("і START для виходу");
end

function fill_cells(x, y, w, h, color)
    -- Хитрий код, що заповнює клітинки, розділивши дисплей на поле 14х12 клітинок і працює незалежно від розміру дисплею
    local cw = display.width / 14
    local ch = display.height / 12

    display.fill_rect(x * cw, y * ch, w * cw, h * ch, color)
end
