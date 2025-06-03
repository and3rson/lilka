#include "lualilka_resources.h"

int lualilka_resources_loadImage(lua_State* L) {
    const char* path = luaL_checkstring(L, 1);
    // Get dir from registry
    lua_getfield(L, LUA_REGISTRYINDEX, "dir");
    const char* dir = lua_tostring(L, -1);
    lua_pop(L, 1);
    String fullPath = String(dir) + "/" + path;
    // 2nd argument is optional transparency color (16-bit 5-6-5)
    int32_t transparencyColor = -1;
    if (lua_gettop(L) > 1) {
        if (lua_isnumber(L, 2)) {
            transparencyColor = lua_tointeger(L, 2);
        }
    }
    int32_t pivotX = 0;
    int32_t pivotY = 0;
    if (lua_gettop(L) > 3) {
        // Pivot X/Y
        pivotX = luaL_checkinteger(L, 3);
        pivotY = luaL_checkinteger(L, 4);
    }

    lilka::Image* image = lilka::resources.loadImage(fullPath, transparencyColor, pivotX, pivotY);

    if (!image) {
        return luaL_error(L, "Не вдалося завантажити зображення %s", fullPath.c_str());
    }

    lilka::serial.log(
        "lua: loaded image %s, size: %d x %d, pivot: %d,%d", path, image->width, image->height, pivotX, pivotY
    );

    // Append image to images table in registry
    lua_getfield(L, LUA_REGISTRYINDEX, "images");
    lua_pushlightuserdata(L, image);
    lua_setfield(L, -2, path);
    lua_pop(L, 1);

    // Create and return table that contains image width, height and pointer
    lua_newtable(L);
    lua_pushinteger(L, image->width);
    lua_setfield(L, -2, "width");
    lua_pushinteger(L, image->height);
    lua_setfield(L, -2, "height");
    lua_pushlightuserdata(L, image);
    lua_setfield(L, -2, "pointer");

    return 1;
}

int lualilka_resources_rotateImage(lua_State* L) {
    // Args are image table and angle in degrees
    // First argument is table that contains image width, height and pointer. We need all of them.
    // Second argument is angle in degrees.
    // Third argument is blank color for pixels that are not covered by the image after rotation.
    lua_getfield(L, 1, "pointer");
    // Check if value is a valid pointer
    if (!lua_islightuserdata(L, -1)) {
        return luaL_error(L, "Невірне зображення");
    }
    lilka::Image* image = (lilka::Image*)lua_touserdata(L, -1);
    lua_pop(L, 1);

    int16_t angle = luaL_checknumber(L, 2);
    int32_t blankColor = luaL_checkinteger(L, 3);

    // Instantiate a new image
    lilka::Image* rotatedImage = new lilka::Image(image->width, image->height, image->transparentColor);
    // Rotate the image
    image->rotate(angle, rotatedImage, blankColor);

    // Append rotatedImage to images table in registry
    lua_getfield(L, LUA_REGISTRYINDEX, "images");
    lua_pushlightuserdata(L, rotatedImage);
    lua_setfield(L, -2, (String("rotatedImage-") + random(100000)).c_str());
    lua_pop(L, 1);

    // Create and return table that contains image width, height and pointer
    lua_newtable(L);
    lua_pushinteger(L, rotatedImage->width);
    lua_setfield(L, -2, "width");
    lua_pushinteger(L, rotatedImage->height);
    lua_setfield(L, -2, "height");
    lua_pushlightuserdata(L, rotatedImage);
    lua_setfield(L, -2, "pointer");

    return 1;
}

int lualilka_resources_flipImageX(lua_State* L) {
    // Arg is image table
    // First argument is table that contains image width, height and pointer. We need all of them.
    lua_getfield(L, 1, "pointer");
    // Check if value is a valid pointer
    if (!lua_islightuserdata(L, -1)) {
        return luaL_error(L, "Невірне зображення");
    }
    lilka::Image* image = (lilka::Image*)lua_touserdata(L, -1);
    lua_pop(L, 1);

    // Instantiate a new image
    lilka::Image* flippedImage = new lilka::Image(image->width, image->height, image->transparentColor);
    // Rotate the image
    image->flipX(flippedImage);

    // Append rotatedImage to images table in registry
    lua_getfield(L, LUA_REGISTRYINDEX, "images");
    lua_pushlightuserdata(L, flippedImage);
    lua_setfield(L, -2, (String("xFlippedImage-") + random(100000)).c_str());
    lua_pop(L, 1);

    // Create and return table that contains image width, height and pointer
    lua_newtable(L);
    lua_pushinteger(L, flippedImage->width);
    lua_setfield(L, -2, "width");
    lua_pushinteger(L, flippedImage->height);
    lua_setfield(L, -2, "height");
    lua_pushlightuserdata(L, flippedImage);
    lua_setfield(L, -2, "pointer");

    return 1;
}

int lualilka_resources_flipImageY(lua_State* L) {
    // Arg is image table
    // First argument is table that contains image width, height and pointer. We need all of them.
    lua_getfield(L, 1, "pointer");
    // Check if value is a valid pointer
    if (!lua_islightuserdata(L, -1)) {
        return luaL_error(L, "Невірне зображення");
    }
    lilka::Image* image = (lilka::Image*)lua_touserdata(L, -1);
    lua_pop(L, 1);

    // Instantiate a new image
    lilka::Image* flippedImage = new lilka::Image(image->width, image->height, image->transparentColor);
    // Rotate the image
    image->flipY(flippedImage);

    // Append rotatedImage to images table in registry
    lua_getfield(L, LUA_REGISTRYINDEX, "images");
    lua_pushlightuserdata(L, flippedImage);
    lua_setfield(L, -2, (String("yFlippedImage-") + random(100000)).c_str());
    lua_pop(L, 1);

    // Create and return table that contains image width, height and pointer
    lua_newtable(L);
    lua_pushinteger(L, flippedImage->width);
    lua_setfield(L, -2, "width");
    lua_pushinteger(L, flippedImage->height);
    lua_setfield(L, -2, "height");
    lua_pushlightuserdata(L, flippedImage);
    lua_setfield(L, -2, "pointer");

    return 1;
}

int lualilka_resources_readFile(lua_State* L) {
    const char* path = luaL_checkstring(L, 1);
    // Get dir from registry
    lua_getfield(L, LUA_REGISTRYINDEX, "dir");
    const char* dir = lua_tostring(L, -1);
    lua_pop(L, 1);
    String fullPath = String(dir) + "/" + path;

    String fileContent;
    int result = lilka::resources.readFile(fullPath, fileContent);
    if (result) {
        return luaL_error(L, "Не вдалося прочитати файл %s", fullPath.c_str());
    }

    lua_pushstring(L, fileContent.c_str());
    return 1;
}

int lualilka_resources_writeFile(lua_State* L) {
    const char* path = luaL_checkstring(L, 1);
    const char* content = luaL_checkstring(L, 2);
    // Get dir from registry
    lua_getfield(L, LUA_REGISTRYINDEX, "dir");
    const char* dir = lua_tostring(L, -1);
    lua_pop(L, 1);
    String fullPath = String(dir) + "/" + path;

    int result = lilka::resources.writeFile(fullPath, content);
    if (result) {
        return luaL_error(L, "Не вдалося записати файл %s", fullPath.c_str());
    }

    return 0;
}

static const luaL_Reg lualilka_resources[] = {
    {"load_image", lualilka_resources_loadImage},
    {"rotate_image", lualilka_resources_rotateImage},
    {"flip_image_x", lualilka_resources_flipImageX},
    {"flip_image_y", lualilka_resources_flipImageY},
    {"read_file", lualilka_resources_readFile},
    {"write_file", lualilka_resources_writeFile},
    {NULL, NULL},
};

int lualilka_resources_register(lua_State* L) {
    // Create global "resources" table that contains all resources functions
    luaL_newlib(L, lualilka_resources);
    lua_setglobal(L, "resources");
    return 0;
}
