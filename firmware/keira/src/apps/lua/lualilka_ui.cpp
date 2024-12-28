
#include "lualilka_ui.h"
#include "lilka.h"
#include "app.h"
#include "lualilka_display.h"

// <KEYBOARD UI ------------------------------------------------->

int lualilka_create_object_keyboardUI(lua_State* L) {
    lilka::InputDialog** userdata =
        reinterpret_cast<lilka::InputDialog**>(lua_newuserdata(L, sizeof(lilka::InputDialog)));
    *userdata = new lilka::InputDialog(luaL_checkstring(L, 1));

    luaL_setmetatable(L, OBJECT_UI_KEYBOARD);
    return 1;
}

int lualilka_delete_object_keyboardUI(lua_State* L) {
    lilka::InputDialog** userdata = reinterpret_cast<lilka::InputDialog**>(lua_touserdata(L, 1));

    if (*userdata) {
        delete *userdata;
        *userdata = nullptr;
    }

    return 0;
}

int lualilka_update_object_keyboardUI(lua_State* L) {
    lilka::InputDialog** userdata = reinterpret_cast<lilka::InputDialog**>(lua_touserdata(L, 1));
    (*userdata)->update();

    return 0;
}

int lualilka_draw_object_keyboardUI(lua_State* L) {
    lilka::InputDialog** userdata = reinterpret_cast<lilka::InputDialog**>(lua_touserdata(L, 1));

    lua_getfield(L, LUA_REGISTRYINDEX, "app");
    App* app = static_cast<App*>(lua_touserdata(L, -1));
    (*userdata)->draw(getDrawable(L));
    lua_pop(L, 1);
    // Queue draw
    app->queueDraw();

    return 0;
}

int lualilka_isFinished_object_keyboardUI(lua_State* L) {
    lilka::InputDialog** userdata = reinterpret_cast<lilka::InputDialog**>(lua_touserdata(L, 1));
    bool isFinished = (*userdata)->isFinished();

    lua_pushboolean(L, isFinished);
    return 1;
}

int lualilka_setMasked_object_keyboardUI(lua_State* L) {
    lilka::InputDialog** userdata = reinterpret_cast<lilka::InputDialog**>(lua_touserdata(L, 1));
    (*userdata)->setMasked(lua_toboolean(L, 2));

    return 0;
}

int lualilka_setValue_object_keyboardUI(lua_State* L) {
    lilka::InputDialog** userdata = reinterpret_cast<lilka::InputDialog**>(lua_touserdata(L, 1));
    (*userdata)->setValue(lua_tostring(L, 2));

    return 0;
}

int lualilka_getValue_object_keyboardUI(lua_State* L) {
    lilka::InputDialog** userdata = reinterpret_cast<lilka::InputDialog**>(lua_touserdata(L, 1));
    String value = (*userdata)->getValue();

    lua_pushstring(L, value.c_str());

    return 1;
}

// <ALERT UI ------------------------------------------------->

int lualilka_create_object_alertUI(lua_State* L) {
    lilka::Alert** userdata = reinterpret_cast<lilka::Alert**>(lua_newuserdata(L, sizeof(lilka::Alert)));
    *userdata = new lilka::Alert(luaL_checkstring(L, 1), luaL_checkstring(L, 2));

    luaL_setmetatable(L, OBJECT_UI_ALERT);
    return 1;
}

int lualilka_delete_object_alertUI(lua_State* L) {
    lilka::Alert** userdata = reinterpret_cast<lilka::Alert**>(lua_touserdata(L, 1));

    if (*userdata) {
        delete *userdata;
        *userdata = nullptr;
    }

    return 0;
}

int lualilka_update_object_alertUI(lua_State* L) {
    lilka::Alert** userdata = reinterpret_cast<lilka::Alert**>(lua_touserdata(L, 1));
    (*userdata)->update();

    return 0;
}

int lualilka_draw_object_alertUI(lua_State* L) {
    lilka::Alert** userdata = reinterpret_cast<lilka::Alert**>(lua_touserdata(L, 1));

    lua_getfield(L, LUA_REGISTRYINDEX, "app");
    App* app = static_cast<App*>(lua_touserdata(L, -1));
    (*userdata)->draw(getDrawable(L));
    lua_pop(L, 1);
    // Queue draw
    app->queueDraw();

    return 0;
}

int lualilka_isFinished_object_alertUI(lua_State* L) {
    lilka::Alert** userdata = reinterpret_cast<lilka::Alert**>(lua_touserdata(L, 1));
    bool isFinished = (*userdata)->isFinished();

    lua_pushboolean(L, isFinished);
    return 1;
}

int lualilka_setTitle_object_alertUI(lua_State* L) {
    lilka::Alert** userdata = reinterpret_cast<lilka::Alert**>(lua_touserdata(L, 1));
    (*userdata)->setTitle(lua_tostring(L, 2));

    return 0;
}

int lualilka_setMessage_object_alertUI(lua_State* L) {
    lilka::Alert** userdata = reinterpret_cast<lilka::Alert**>(lua_touserdata(L, 1));
    (*userdata)->setMessage(lua_tostring(L, 2));

    return 0;
}

int lualilka_addActivationButton_object_alertUI(lua_State* L) {
    lilka::Alert** userdata = reinterpret_cast<lilka::Alert**>(lua_touserdata(L, 1));

    const char* keys[] = {"up", "down", "left", "right", "a", "b", "c", "d", "select", "start"};
    const int keysCount = sizeof(keys) / sizeof(keys[0]);

    String key = luaL_checkstring(L, 2);
    lilka::Button which_key;
    for (int i = 0; i < keysCount; ++i) {
        if (key == keys[i]) {
            which_key = (lilka::Button)i;
        }
    }

    (*userdata)->addActivationButton(which_key);

    return 0;
}

int lualilka_getButton_object_alertUI(lua_State* L) {
    lilka::Alert** userdata = reinterpret_cast<lilka::Alert**>(lua_touserdata(L, 1));

    const char* keys[] = {"up", "down", "left", "right", "a", "b", "c", "d", "select", "start"};

    lilka::Button button = (*userdata)->getButton();
    lua_pushstring(L, keys[button]);

    return 1;
}

// <PROGRESS UI ------------------------------------------------->

int lualilka_create_object_ProgressDialog(lua_State* L) {
    lilka::ProgressDialog** userdata =
        reinterpret_cast<lilka::ProgressDialog**>(lua_newuserdata(L, sizeof(lilka::ProgressDialog)));
    *userdata = new lilka::ProgressDialog(luaL_checkstring(L, 1), luaL_checkstring(L, 2));

    luaL_setmetatable(L, OBJECT_UI_PROGRESS);
    return 1;
}

int lualilka_delete_object_ProgressDialog(lua_State* L) {
    lilka::ProgressDialog** userdata = reinterpret_cast<lilka::ProgressDialog**>(lua_touserdata(L, 1));

    if (*userdata) {
        delete *userdata;
        *userdata = nullptr;
    }

    return 0;
}

int lualilka_draw_object_ProgressDialog(lua_State* L) {
    lilka::ProgressDialog** userdata = reinterpret_cast<lilka::ProgressDialog**>(lua_touserdata(L, 1));

    lua_getfield(L, LUA_REGISTRYINDEX, "app");
    App* app = static_cast<App*>(lua_touserdata(L, -1));
    (*userdata)->draw(getDrawable(L));
    lua_pop(L, 1);
    // Queue draw
    app->queueDraw();

    return 0;
}

int lualilka_setMessage_object_ProgressDialog(lua_State* L) {
    lilka::ProgressDialog** userdata = reinterpret_cast<lilka::ProgressDialog**>(lua_touserdata(L, 1));
    (*userdata)->setMessage(lua_tostring(L, 2));

    return 0;
}

int lualilka_setProgress_object_ProgressDialog(lua_State* L) {
    lilka::ProgressDialog** userdata = reinterpret_cast<lilka::ProgressDialog**>(lua_touserdata(L, 1));
    (*userdata)->setProgress(lua_tointeger(L, 2));

    return 0;
}

int lualilka_UI_register_keyboard(lua_State* L) {
    lua_register(L, OBJECT_UI_KEYBOARD, lualilka_create_object_keyboardUI);
    luaL_newmetatable(L, OBJECT_UI_KEYBOARD);
    lua_pushcfunction(L, lualilka_delete_object_keyboardUI);
    lua_setfield(L, -2, "__gc");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    lua_pushcfunction(L, lualilka_update_object_keyboardUI);
    lua_setfield(L, -2, "update");
    lua_pushcfunction(L, lualilka_draw_object_keyboardUI);
    lua_setfield(L, -2, "draw");
    lua_pushcfunction(L, lualilka_isFinished_object_keyboardUI);
    lua_setfield(L, -2, "isFinished");
    lua_pushcfunction(L, lualilka_setMasked_object_keyboardUI);
    lua_setfield(L, -2, "setMasked");
    lua_pushcfunction(L, lualilka_setValue_object_keyboardUI);
    lua_setfield(L, -2, "setValue");
    lua_pushcfunction(L, lualilka_getValue_object_keyboardUI);
    lua_setfield(L, -2, "getValue");
    return 0;
}

int lualilka_UI_register_alert(lua_State* L) {
    lua_register(L, OBJECT_UI_ALERT, lualilka_create_object_alertUI);
    luaL_newmetatable(L, OBJECT_UI_ALERT);
    lua_pushcfunction(L, lualilka_delete_object_alertUI);
    lua_setfield(L, -2, "__gc");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    lua_pushcfunction(L, lualilka_update_object_alertUI);
    lua_setfield(L, -2, "update");
    lua_pushcfunction(L, lualilka_draw_object_alertUI);
    lua_setfield(L, -2, "draw");
    lua_pushcfunction(L, lualilka_isFinished_object_alertUI);
    lua_setfield(L, -2, "isFinished");
    lua_pushcfunction(L, lualilka_setTitle_object_alertUI);
    lua_setfield(L, -2, "setTitle");
    lua_pushcfunction(L, lualilka_setMessage_object_alertUI);
    lua_setfield(L, -2, "setMessage");

    lua_pushcfunction(L, lualilka_addActivationButton_object_alertUI);
    lua_setfield(L, -2, "addActivationButton");
    lua_pushcfunction(L, lualilka_getButton_object_alertUI);
    lua_setfield(L, -2, "getButton");
    return 0;
}

int lualilka_UI_register_progress(lua_State* L) {
    lua_register(L, OBJECT_UI_PROGRESS, lualilka_create_object_ProgressDialog);
    luaL_newmetatable(L, OBJECT_UI_PROGRESS);
    lua_pushcfunction(L, lualilka_delete_object_ProgressDialog);
    lua_setfield(L, -2, "__gc");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    lua_pushcfunction(L, lualilka_draw_object_ProgressDialog);
    lua_setfield(L, -2, "draw");
    lua_pushcfunction(L, lualilka_setMessage_object_ProgressDialog);
    lua_setfield(L, -2, "setMessage");
    lua_pushcfunction(L, lualilka_setProgress_object_ProgressDialog);
    lua_setfield(L, -2, "setProgress");

    lua_pop(L, 1);

    return 0;
}
