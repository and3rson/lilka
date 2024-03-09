#include "lualilka_state.h"

int lualilka_state_load(lua_State* L, const char* path) {
    FILE* file = fopen(path, "r");
    if (!file) {
        return 1;
    }

    int count = 0;

    // Create state table
    lua_newtable(L);
    // Read state table
    char key[256];
    char type[32];
    while (fscanf(file, "%s", key) != EOF) {
        // Read value type
        fscanf(file, "%s", type);
        if (strcmp(type, "number") == 0) {
            // Read number
            double value;
            fscanf(file, "%lf", &value);
            lilka::serial_log("lua: state: load number %s = %lf", key, value);
            lua_pushnumber(L, value);
            lua_setfield(L, -2, key);
            count++;
        } else if (strcmp(type, "string") == 0) {
            // Read string
            char value[256];
            fscanf(file, "%s", value);
            lilka::serial_log("lua: state: load string %s = %s", key, value);
            lua_pushstring(L, value);
            lua_setfield(L, -2, key);
            count++;
        } else if (strcmp(type, "boolean") == 0) {
            // Read boolean
            int value;
            fscanf(file, "%d", &value);
            lilka::serial_log("lua: state: load boolean %s = %d", key, value);
            lua_pushboolean(L, value);
            lua_setfield(L, -2, key);
            count++;
        } else if (strcmp(type, "nil") == 0) {
            // Read nil
            lilka::serial_log("lua: state: load nil %s", key);
            lua_pushnil(L);
            lua_setfield(L, -2, key);
            count++;
        } else {
            // Skip unsupported types
        }
    }

    lilka::serial_log("lua: state: loaded %d values", count);

    // Set state table to global
    lua_setglobal(L, "state");

    fclose(file);
    return 0;
}

int lualilka_state_save(lua_State* L, const char* path) {
    FILE* file = fopen(path, "w");
    if (!file) {
        return 1;
    }

    int count = 0;

    // Get state global
    lua_getglobal(L, "state");
    // Iterate over state table
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        // Get key
        const char* key = lua_tostring(L, -2);
        fprintf(file, "%s\n", key);
        // Get value type
        int type = lua_type(L, -1);
        if (type == LUA_TNUMBER) {
            // Write number
            double value = lua_tonumber(L, -1);
            lilka::serial_log("lua: state: save number %s = %lf", key, value);
            fprintf(file, "number\n%lf\n", value);
            count++;
        } else if (type == LUA_TSTRING) {
            // Write string
            const char* value = lua_tostring(L, -1);
            String valueStr = value;
            valueStr.replace("\n", "\\n");
            valueStr.replace("\r", "\\r");
            lilka::serial_log("lua: state: save string %s = %s", key, valueStr.c_str());
            fprintf(file, "string\n%s\n", value);
            count++;
        } else if (type == LUA_TBOOLEAN) {
            // Write boolean
            int value = lua_toboolean(L, -1);
            lilka::serial_log("lua: state: save boolean %s = %d", key, value);
            fprintf(file, "boolean\n%d\n", value);
            count++;
        } else if (type == LUA_TNIL) {
            // Write nil
            lilka::serial_log("lua: state: save nil %s", key);
            fprintf(file, "nil\n");
            count++;
        } else {
            // Skip unsupported types
            lilka::serial_log("lua: state: skip %s (cannot serialize)", key);
        }
        // Remove value from stack
        lua_pop(L, 1);
    }

    lilka::serial_log("lua: state: saved %d values", count);

    fclose(file);
    return 0;
}
