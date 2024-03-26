
#include "lualilka_dialog.h"
#include "lilka.h"
#include "app.h"
#include "lualilka_display.h"
// <INPUT DIALOG ------------------------------------------------->
int lualilka_dialog_input(lua_State* L) {
    lilka::InputDialog dialog(luaL_checkstring(L, 2));

    while (true) {
        dialog.update();
        dialog.draw(getDrawable(L));
        lua_getfield(L, LUA_REGISTRYINDEX, "app");
        App* app = static_cast<App*>(lua_touserdata(L, -1));
        lua_pop(L, 1);
        app->queueDraw();
        if (dialog.isFinished()) {
            break;
        }
    }
    lua_pushstring(L, dialog.getValue().c_str());
    return 1;
}
// <ALERT DIALOG ------------------------------------------------->
static int lualilka_create_object_alertDialog(lua_State* L) {
    *reinterpret_cast<lilka::Alert**>(lua_newuserdata(L, sizeof(lilka::Alert*))) = new lilka::Alert(luaL_checkstring(L, 1), luaL_checkstring(L, 2));
    luaL_setmetatable(L, OBJECT_DIALOG_ALERT);
    return 1;
}

static int lualilka_delete_object_alertDialog(lua_State* L) {
    delete *reinterpret_cast<lilka::Alert**>(lua_touserdata(L, 1));
    return 0;
}

static int lualilka_dialog_alert_setTitile(lua_State* L) {
    lilka::Alert* alertPtr = reinterpret_cast<lilka::Alert*>(luaL_checkudata(L, 1, OBJECT_DIALOG_ALERT));
    alertPtr->setTitle(luaL_checkstring(L, 2));
    return 0;
}

static int lualilka_dialog_alert_setMessage(lua_State* L) {
    lilka::Alert* alertPtr = reinterpret_cast<lilka::Alert*>(luaL_checkudata(L, 1, OBJECT_DIALOG_ALERT));
    alertPtr->setMessage(luaL_checkstring(L, 2));
    return 0;
}

static int lualilka_dialog_alert_update(lua_State* L) {
    lilka::Alert* alertPtr = reinterpret_cast<lilka::Alert*>(luaL_checkudata(L, 1, OBJECT_DIALOG_ALERT));
    alertPtr->update();
    alertPtr->draw(getDrawable(L));
    return 0;
}

static int lualilka_dialog_alert_isfinished(lua_State* L) {
    lilka::Alert* alertPtr = reinterpret_cast<lilka::Alert*>(luaL_checkudata(L, 1, OBJECT_DIALOG_ALERT));
    bool ret = alertPtr->isFinished();
    lua_pushboolean(L, ret);
    return 1;
}
// <PROGRESS DIALOG ------------------------------------------------->
static int lualilka_create_object_ProgressDialog(lua_State* L) {
    *reinterpret_cast<lilka::ProgressDialog**>(lua_newuserdata(L, sizeof(lilka::ProgressDialog*))) = new lilka::ProgressDialog(luaL_checkstring(L, 1), luaL_checkstring(L, 2));
    luaL_setmetatable(L, OBJECT_DIALOG_ALERT);
    return 1;
}

static int lualilka_delete_object_ProgressDialog(lua_State* L) {
    delete *reinterpret_cast<lilka::ProgressDialog**>(lua_touserdata(L, 1));
    return 0;
}

static int lualilka_dialog_progress_setMessage(lua_State* L) {
    lilka::ProgressDialog* ProgressDialogPtr = reinterpret_cast<lilka::ProgressDialog*>(luaL_checkudata(L, 1, OBJECT_DIALOG_PROGRESS_ALERT));
    ProgressDialogPtr->setMessage(luaL_checkstring(L, 2));
    return 0;
}

static int lualilka_dialog_progress_setProgress(lua_State* L) {
    lilka::ProgressDialog* ProgressDialogPtr = reinterpret_cast<lilka::ProgressDialog*>(luaL_checkudata(L, 1, OBJECT_DIALOG_PROGRESS_ALERT));
    ProgressDialogPtr->setProgress(luaL_checkinteger(L, 2));
    return 0;
}

static int lualilka_dialog_progress_update(lua_State* L) {
    lilka::ProgressDialog* ProgressDialogPtr = reinterpret_cast<lilka::ProgressDialog*>(luaL_checkudata(L, 1, OBJECT_DIALOG_PROGRESS_ALERT));
    ProgressDialogPtr->draw(getDrawable(L));
    return 0;
}

static const luaL_Reg lualilka_dialog[] = {
    {"keyboard", lualilka_dialog_input},
    {NULL, NULL},
};

int lualilka_dialog_register_inputDialog(lua_State* L){
    luaL_newlib(L, lualilka_dialog);
    lua_setglobal(L, "dialog");
    
    return 0;
}

int lualilka_dialog_register_alertDialog(lua_State* L){
    lua_register(L, OBJECT_DIALOG_ALERT, lualilka_create_object_alertDialog);
    luaL_newmetatable(L, OBJECT_DIALOG_ALERT);
    lua_pushcfunction(L, lualilka_delete_object_alertDialog);
    lua_setfield(L, -2, "__gc");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    lua_pushcfunction(L, lualilka_dialog_alert_setTitile);
    lua_setfield(L, -2, "setTitle");
    lua_pushcfunction(L, lualilka_dialog_alert_setMessage);
    lua_setfield(L, -2, "setMessage");
    lua_pushcfunction(L, lualilka_dialog_alert_update);
    lua_setfield(L, -2, "update");
    lua_pushcfunction(L, lualilka_dialog_alert_isfinished);
    lua_setfield(L, -2, "isFinished");

    lua_pop(L, 1);

    return 0;
}

int lualilka_dialog_register_progressDialog(lua_State* L){
    lua_register(L, OBJECT_DIALOG_PROGRESS_ALERT, lualilka_create_object_alertDialog);
    luaL_newmetatable(L, OBJECT_DIALOG_PROGRESS_ALERT);
    lua_pushcfunction(L, lualilka_delete_object_alertDialog);
    lua_setfield(L, -2, "__gc");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    lua_pushcfunction(L, lualilka_dialog_progress_setMessage);
    lua_setfield(L, -2, "setMessage");
    lua_pushcfunction(L, lualilka_dialog_progress_setProgress);
    lua_setfield(L, -2, "setProgress");
    lua_pushcfunction(L, lualilka_dialog_alert_update);
    lua_setfield(L, -2, "update");

    lua_pop(L, 1);
    
    return 0;
}
