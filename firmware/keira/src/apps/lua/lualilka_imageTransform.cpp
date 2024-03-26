#include "lualilka_imageTransform.h"
#include "lilka/display.h"

static int lualilka_create_object_imageTransform(lua_State* L) {
    *reinterpret_cast<lilka::Transform**>(lua_newuserdata(L, sizeof(lilka::Transform*))) = new lilka::Transform();
    luaL_setmetatable(L, IMAGE_TRANSFORM);
    return 1;
}

static int lualilka_delete_object_imageTransform(lua_State* L) {
    lilka::Transform** transformPtr = reinterpret_cast<lilka::Transform**>(luaL_checkudata(L, 1, IMAGE_TRANSFORM));
    if (*transformPtr) {
        delete *transformPtr;
        *transformPtr = nullptr;
    } else {
        lilka::serial_err("double free of transform %p, should never happen", transformPtr);
    }
    return 0;
}

static int lualilka_imageTransform_rotate(lua_State* L) {
    lilka::Transform* transformPtr = reinterpret_cast<lilka::Transform*>(luaL_checkudata(L, 1, IMAGE_TRANSFORM));
    lilka::Transform* rotatedTransform = new lilka::Transform(transformPtr->rotate(luaL_checknumber(L, 2)));
    *reinterpret_cast<lilka::Transform**>(lua_newuserdata(L, sizeof(lilka::Transform))) = rotatedTransform;
    luaL_setmetatable(L, IMAGE_TRANSFORM);
    return 1;
}

static int lualilka_imageTransform_scale(lua_State* L) {
    lilka::Transform* transformPtr = reinterpret_cast<lilka::Transform*>(luaL_checkudata(L, 1, IMAGE_TRANSFORM));
    lilka::Transform* scaledTransform =
        new lilka::Transform(transformPtr->scale(luaL_checknumber(L, 2), luaL_checknumber(L, 3)));
    *reinterpret_cast<lilka::Transform**>(lua_newuserdata(L, sizeof(lilka::Transform))) = scaledTransform;
    luaL_setmetatable(L, IMAGE_TRANSFORM);
    return 1;
}

static int lualilka_imageTransform_multiply(lua_State* L) {
    lilka::Transform* transformPtr = reinterpret_cast<lilka::Transform*>(luaL_checkudata(L, 1, IMAGE_TRANSFORM));
    lilka::Transform* multipliedTransfform = new lilka::Transform(
        transformPtr->multiply(*reinterpret_cast<lilka::Transform*>(luaL_checkudata(L, 2, IMAGE_TRANSFORM)))
    );
    *reinterpret_cast<lilka::Transform**>(lua_newuserdata(L, sizeof(lilka::Transform))) = multipliedTransfform;
    luaL_setmetatable(L, IMAGE_TRANSFORM);
    return 1;
}

static int lualilka_imageTransform_inverse(lua_State* L) {
    lilka::Transform* transformPtr = reinterpret_cast<lilka::Transform*>(luaL_checkudata(L, 1, IMAGE_TRANSFORM));
    lilka::Transform* inversedTransform = new lilka::Transform(transformPtr->inverse());
    *reinterpret_cast<lilka::Transform**>(lua_newuserdata(L, sizeof(lilka::Transform))) = inversedTransform;
    luaL_setmetatable(L, IMAGE_TRANSFORM);
    return 1;
}

static int lualilka_imageTransform_vtransform(lua_State* L) {
    lilka::int_vector_t result =
        (*reinterpret_cast<lilka::Transform**>(luaL_checkudata(L, 1, IMAGE_TRANSFORM)))
            ->transform(lilka::int_vector_t{.x = (int)luaL_checknumber(L, 2), .y = (int)luaL_checknumber(L, 3)});
    lua_pushinteger(L, result.x);
    lua_pushinteger(L, result.y);
    return 2;
}

static int lualilka_imageTransform_get_matrix(lua_State* L) {
    lua_newtable(L);
    for (int i = 0; i < 2; ++i) {
        lua_newtable(L);
        for (int j = 0; j < 2; ++j) {
            lua_pushinteger(
                L, (*reinterpret_cast<lilka::Transform**>(luaL_checkudata(L, 1, IMAGE_TRANSFORM)))->matrix[i][j]
            );
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

                (*reinterpret_cast<lilka::Transform**>(luaL_checkudata(L, 1, IMAGE_TRANSFORM)))->matrix[i][j] =
                    lua_tonumber(L, -1);
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
