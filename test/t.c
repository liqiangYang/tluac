#include "lauxlib.h"
#include "lualib.h"
//#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/tluac-lua/tluac-lua.h"

int _start(lua_State *L){
	size_t len = 0;
	const char *s = lua_tolstring(L, -1, &len);
	printf("xxxxxxxxx %s xxx", s);
	return 1;
}

struct List {
	struct List *next;
	const char *var;
} List;

int _new(lua_State *L){
	lua_newuserdata(L, sizeof(struct List));

	luaL_getmetatable(L, "ttt");
	lua_setmetatable(L, -2);
	return 1;
}

int _push(lua_State *L){
	const char *s = lua_tostring(L, -1);
	struct List *list = lua_touserdata(L, 1);
	struct List *l = malloc(sizeof(struct List));
//	l->var = malloc(strlen(s));
//	strcpy(l->var, s);
	l->var = s;
	l->next = list->next;
	list->next = l;
	printf("top %d\n", lua_gettop(L));
	return 0;
}

int _shift(lua_State *L){
	struct List *list = lua_touserdata(L, 1);
	struct List *l = list->next;
	lua_pushstring(L, l->var);
	printf("top %d\n", lua_gettop(L));
	list->next = list->next->next;

//	free(l->var);
	free(l);
	return 1;
}

int luaopen_t(lua_State *L) {
	luaL_Reg l[] = {
		{ "new", _new },
		{ NULL, NULL },
	};
	luaL_Reg ml[] = {
		{ "push", _push },
		{ "shift", _shift },
		{ NULL, NULL },
	};
	luaL_newmetatable(L, "ttt");
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);
	lua_settable(L, -3);

	luaL_setfuncs(L, ml, 0);

	luaL_newlib(L,l);

	return 1;
}

char *revstr(char *str, size_t len)
{

    char    *start = str;
    char    *end = str + len - 1;
    char    ch;

    if (str != NULL)
    {
        while (start < end)
        {

            ch = *start;
            *start++ = *end;
            *end-- = ch;
        }
    }
    return str;
}

int main() {
	char *s = malloc(sizeof(char) * 10);
	for (int i = 0; i < 10; i ++)
	{
		s[i] = (char)(i + 98);
	}
	s[10] = '\0';
	char a[10] = "12345678\0";
	char b[10] = "1234567890";

	printf("strlen = %d %d\n", (int)strlen(a), (int)strlen(b));

	printf("strlen = %d\n", (int)strlen(s));
	printf("%s\n", s);

	s = revstr(s, strlen(s));
	printf("%s", s);

	free(s);
	return 0;
}
