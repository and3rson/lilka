#include "lualilka_imageTransform.h"
#include "lilka/display.h"

#define IMAGE_TRANSFORM "imageTransform"

static int lualilka_imageTransform_rotate(lua_State* L){
	(*reinterpret_cast<lilka::Transform**>(luaL_checkudata(L, 1, IMAGE_TRANSFORM)))->rotate(luaL_checknumber(L, 2));
	return 0;
}

static int lualilka_imageTransform_scale(lua_State* L){
	(*reinterpret_cast<lilka::Transform**>(luaL_checkudata(L, 1, IMAGE_TRANSFORM)))->scale(luaL_checknumber(L, 2), luaL_checknumber(L, 3));
	return 0;
}

static int lualilka_imageTransform_multiply(lua_State* L){
	// (*reinterpret_cast<lilka::Transform**>(luaL_checkudata(L, 1, IMAGE_TRANSFORM)))->multiply((*reinterpret_cast<lilka::Transform**>(luaL_checkudata(L, 2, IMAGE_TRANSFORM))));
    (*reinterpret_cast<lilka::Transform*>(luaL_checkudata(L, 1, IMAGE_TRANSFORM))).multiply(*reinterpret_cast<lilka::Transform*>(luaL_checkudata(L, 2, IMAGE_TRANSFORM)));

	return 0;
}

// static int myobject_get(lua_State* L){
// 	lua_pushnumber(L, (*reinterpret_cast<lilka::Transform**>(luaL_checkudata(L, 1, IMAGE_TRANSFORM)))->get());
// 	return 1;
// }

static int lualilka_create_object_imageTransform(lua_State* L){
	*reinterpret_cast<lilka::Transform**>(lua_newuserdata(L, sizeof(lilka::Transform*))) = new lilka::Transform();
	luaL_setmetatable(L, IMAGE_TRANSFORM);
	return 1;
}

static int lualilka_delete_object_imageTransform(lua_State* L){
	delete *reinterpret_cast<lilka::Transform**>(lua_touserdata(L, 1));
	return 0;
}

// static const luaL_Reg lualilka_resources[] = {
//     // {"load_image", lualilka_resources_loadImage},
//     // {"rotate_image", lualilka_resources_rotateImage},
//     // {"flip_image_x", lualilka_resources_flipImageX},
//     // {"flip_image_y", lualilka_resources_flipImageY},
//     // {"read_file", lualilka_resources_readFile},
//     // {"write_file", lualilka_resources_writeFile},
//     {NULL, NULL},
// };

int lualilka_imageTransform_register(lua_State* L) {
	lua_register(L, IMAGE_TRANSFORM, lualilka_create_object_imageTransform);
	luaL_newmetatable(L, IMAGE_TRANSFORM);
    lua_pushcfunction(L, lualilka_delete_object_imageTransform); lua_setfield(L, -2, "__gc");
    lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");
	// lua_pushcfunction(L, myobject_set); lua_setfield(L, -2, "set");
	// lua_pushcfunction(L, myobject_get); lua_setfield(L, -2, "get");
	lua_pop(L, 1);
    
    // Create global "resources" table that contains all resources functions
    // luaL_newlib(L, lualilka_resources);
    // lua_setglobal(L, "imageTransform");
    return 0;
}
