#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef TLUA_LUA

#define TLUA_LUA 1
#include "../tluac-epoll/tluac-epoll.h"

void luanew(struct context * ctx);
void luareg(struct context * ctx);
void luadofile(struct context * ctx, char *file);
int _on(lua_State *L);

#endif
