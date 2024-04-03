#include "lualilka_imageTransform.h"
#include "lilka/display.h"

#define TRANSFORM_PTR(x) static_cast<lilka::Transform**>(x)

static int lualilka_create_object_imageTransform(lua_State* L) {
    lilka::Transform** userdata = TRANSFORM_PTR(lua_newuserdata(L, sizeof(lilka::Transform)));
    *userdata = new lilka::Transform();
    luaL_setmetatable(L, IMAGE_TRANSFORM);
    return 1;
}

static int lualilka_delete_object_imageTransform(lua_State* L) {
    lilka::Transform** userdata = TRANSFORM_PTR(luaL_checkudata(L, 1, IMAGE_TRANSFORM));
    if (*userdata) {
        delete *userdata;
        *userdata = nullptr;
    }
    return 0;
}

static int lualilka_imageTransform_rotate(lua_State* L) {
    lilka::Transform** userdata = TRANSFORM_PTR(luaL_checkudata(L, 1, IMAGE_TRANSFORM));
    lilka::Transform** newUserdata = TRANSFORM_PTR(lua_newuserdata(L, sizeof(lilka::Transform)));
    *newUserdata = new lilka::Transform((*userdata)->rotate(luaL_checknumber(L, 2)));
    luaL_setmetatable(L, IMAGE_TRANSFORM);
    return 1;
}

static int lualilka_imageTransform_scale(lua_State* L) {
    lilka::Transform** userdata = TRANSFORM_PTR(luaL_checkudata(L, 1, IMAGE_TRANSFORM));
    lilka::Transform** newUserdata = TRANSFORM_PTR(lua_newuserdata(L, sizeof(lilka::Transform)));
    *newUserdata = new lilka::Transform((*userdata)->scale(luaL_checknumber(L, 2), luaL_checknumber(L, 3)));
    luaL_setmetatable(L, IMAGE_TRANSFORM);
    return 1;
}

static int lualilka_imageTransform_multiply(lua_State* L) {
    lilka::Transform** userdata1 = TRANSFORM_PTR(luaL_checkudata(L, 1, IMAGE_TRANSFORM));
    lilka::Transform** userdata2 = TRANSFORM_PTR(luaL_checkudata(L, 2, IMAGE_TRANSFORM));
    lilka::Transform** newUserdata = TRANSFORM_PTR(lua_newuserdata(L, sizeof(lilka::Transform)));
    *newUserdata = new lilka::Transform((*userdata1)->multiply(**userdata2));
    luaL_setmetatable(L, IMAGE_TRANSFORM);
    return 1;
}

static int lualilka_imageTransform_inverse(lua_State* L) {
    lilka::Transform** userdata = TRANSFORM_PTR(luaL_checkudata(L, 1, IMAGE_TRANSFORM));
    lilka::Transform** newUserdata = TRANSFORM_PTR(lua_newuserdata(L, sizeof(lilka::Transform)));
    *newUserdata = new lilka::Transform((*userdata)->inverse());
    luaL_setmetatable(L, IMAGE_TRANSFORM);
    return 1;
}

static int lualilka_imageTransform_vtransform(lua_State* L) {
    lilka::Transform** userdata = TRANSFORM_PTR(luaL_checkudata(L, 1, IMAGE_TRANSFORM));
    lilka::int_vector_t result =
        (*userdata)->transform(lilka::int_vector_t{.x = (int)luaL_checknumber(L, 2), .y = (int)luaL_checknumber(L, 3)});
    lua_pushinteger(L, result.x);
    lua_pushinteger(L, result.y);
    return 2;
}

static int lualilka_imageTransform_get_matrix(lua_State* L) {
    lilka::Transform** userdata = TRANSFORM_PTR(luaL_checkudata(L, 1, IMAGE_TRANSFORM));
    lua_newtable(L);
    for (int i = 0; i < 2; ++i) {
        lua_newtable(L);
        for (int j = 0; j < 2; ++j) {
            lua_pushinteger(L, (*userdata)->matrix[i][j]);
            lua_rawseti(L, -2, j + 1);
        }
        lua_rawseti(L, -2, i + 1);
    }
    return 1;
}

static int lualilka_imageTransform_set_matrix(lua_State* L) {
    if (!lua_istable(L, 1)) {
        return luaL_error(L, "Expected a table as the first argument");

        for (int i = 0; i < 2; ++i) {
            lua_rawgeti(L, 1, i + 1);
            for (int j = 0; j < 2; ++j) {
                lua_rawgeti(L, -1, j + 1);

                if (!lua_isnumber(L, -1)) {
                    lua_pop(L, 2);
                    return luaL_error(L, "Element at (%d, %d) is not a number", i, j);
                }

                (*TRANSFORM_PTR(luaL_checkudata(L, 1, IMAGE_TRANSFORM)))->matrix[i][j] = lua_tonumber(L, -1);
                lua_pop(L, 1);
            }

            lua_pop(L, 1);
        }
    }
    return 0;
}

int lualilka_imageTransform_register(lua_State* L) {
    lua_register(L, IMAGE_TRANSFORM, lualilka_create_object_imageTransform);
    luaL_newmetatable(L, IMAGE_TRANSFORM);
    lua_pushcfunction(L, lualilka_delete_object_imageTransform);
    lua_setfield(L, -2, "__gc");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    lua_pushcfunction(L, lualilka_imageTransform_set_matrix);
    lua_setfield(L, -2, "set");
    lua_pushcfunction(L, lualilka_imageTransform_get_matrix);
    lua_setfield(L, -2, "get");

    lua_pushcfunction(L, lualilka_imageTransform_scale);
    lua_setfield(L, -2, "scale");
    lua_pushcfunction(L, lualilka_imageTransform_rotate);
    lua_setfield(L, -2, "rotate");
    lua_pushcfunction(L, lualilka_imageTransform_multiply);
    lua_setfield(L, -2, "multiply");
    lua_pushcfunction(L, lualilka_imageTransform_inverse);
    lua_setfield(L, -2, "inverse");
    lua_pushcfunction(L, lualilka_imageTransform_vtransform);
    lua_setfield(L, -2, "vtransform");

    lua_pop(L, 1);
    return 0;
}
