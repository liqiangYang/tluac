#include "tluac-lua.h"

void luanew(struct context * ctx){
	ctx->lua = luaL_newstate();
	luaopen_base(ctx->lua);
	luaopen_io(ctx->lua);
}

lua_State *lua_coroutine(struct context * ctx){
	return lua_newthread(ctx->lua);
}

void luareg(struct context * ctx){
	luaL_Reg reg[] = {
		{"on", _on},
		{"send", _send},
		{NULL, NULL},
	};
//	lua_newuserdata(ctx->lua, sizeof(struct context));
	lua_pushlightuserdata(ctx->lua, ctx);

	luaL_newmetatable(ctx->lua, "ctx_metatable");
	lua_pushstring(ctx->lua, "__index");
	lua_pushvalue(ctx->lua, -2);
	lua_settable(ctx->lua, -3);

	lua_pushstring(ctx->lua, "CONNECT");
	lua_pushinteger(ctx->lua, 1);
	lua_settable(ctx->lua, -3);

	lua_pushstring(ctx->lua, "MESSAGE");
	lua_pushinteger(ctx->lua, 2);
	lua_settable(ctx->lua, -3);

	lua_pushstring(ctx->lua, "CLOSE");
	lua_pushinteger(ctx->lua, 3);
	lua_settable(ctx->lua, -3);

	luaL_setfuncs(ctx->lua, reg, 0);
	lua_setmetatable(ctx->lua, -2);

	lua_setglobal(ctx->lua, "tluac");

	lua_settop(ctx->lua, 0);
}

int _on(lua_State *L){
	struct context *ctx = lua_touserdata(L, 1);
	int t = lua_tointeger(L, 2);
	char *func = lua_tostring(L, 3);

	printf("t = %d, func = %s\n", t,func);

	if (t == 1){
		ctx->on_connect = func;
	}else if(t == 2){
		ctx->on_message = func;
	}else if(t == 3){
		ctx->on_close = func;
	}

	return 0;
}

int _send(lua_State *L){
	struct context *ctx = lua_touserdata(L, 1);
	int fd = lua_tointeger(L, 2);
	int len = 0;
	char *msg = lua_tolstring(L, 3, &len);

	send(fd, msg, len, 0);

	return 0;
}

void luadofile(struct context * ctx, char *file){
	luaL_dofile(ctx->lua, file);
}

//int luaopen(lua_State *L) {
//	luaL_Reg l[] = {
//		{ "new", _new },
//		{ NULL, NULL },
//	};
//	luaL_Reg ml[] = {
//		{ "push", _push },
//		{ "shift", _shift },
//		{ NULL, NULL },
//	};
//	luaL_newmetatable(L, "ttt");
//	lua_pushstring(L, "__index");
//	lua_pushvalue(L, -2);
//	lua_settable(L, -3);
//
//	luaL_setfuncs(L, ml, 0);
//
//	luaL_newlib(L,l);
//
//	return 1;
//}


//int _start(lua_State *L){
//	size_t len = 0;
//	const char *s = lua_tolstring(L, -1, &len);
//	printf("xxxxxxxxx %s xxx", s);
//	return 1;
//}
//
//struct List {
//	struct List *next;
//	const char *var;
//} List;
//
//int _new(lua_State *L){
//	lua_newuserdata(L, sizeof(struct List));
//
//	luaL_getmetatable(L, "ttt");
//	lua_setmetatable(L, -2);
//	return 1;
//}
//
//int _push(lua_State *L){
//	const char *s = lua_tostring(L, -1);
//	struct List *list = lua_touserdata(L, 1);
//	struct List *l = malloc(sizeof(struct List));
////	l->var = malloc(strlen(s));
////	strcpy(l->var, s);
//	l->var = s;
//	l->next = list->next;
//	list->next = l;
//	printf("top %d\n", lua_gettop(L));
//	return 0;
//}
//
//int _shift(lua_State *L){
//	struct List *list = lua_touserdata(L, 1);
//	struct List *l = list->next;
//	lua_pushstring(L, l->var);
//	printf("top %d\n", lua_gettop(L));
//	list->next = list->next->next;
//
////	free(l->var);
//	free(l);
//	return 1;
//}
//
//int luaopen_t(lua_State *L) {
//	luaL_Reg l[] = {
//		{ "new", _new },
//		{ NULL, NULL },
//	};
//	luaL_Reg ml[] = {
//		{ "push", _push },
//		{ "shift", _shift },
//		{ NULL, NULL },
//	};
//	luaL_newmetatable(L, "ttt");
//	lua_pushstring(L, "__index");
//	lua_pushvalue(L, -2);
//	lua_settable(L, -3);
//
//	luaL_setfuncs(L, ml, 0);
//
//	luaL_newlib(L,l);
//
//	return 1;
//}
//
//char *revstr(char *str, size_t len)
//{
//
//    char    *start = str;
//    char    *end = str + len - 1;
//    char    ch;
//
//    if (str != NULL)
//    {
//        while (start < end)
//        {
//
//            ch = *start;
//            *start++ = *end;
//            *end-- = ch;
//        }
//    }
//    return str;
//}
