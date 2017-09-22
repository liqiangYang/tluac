#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void luanew(struct context * ctx);
void luareg(struct context * ctx);
void luadofile(struct context * ctx, char *file);
