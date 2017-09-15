LUALIB_CYGWIN=-I/usr/include -L/usr/local/bin -llua 
LUALIB=-I/usr/include -L/usr/local/bin -llua
SRC=\
src/main.c \
src/thread/thread.c \
src/lua/lua.c

all : clean win

win : tluac.dll tluac.exe

linux : tluac

tluac.dll : $(SRC)
	gcc -g -Wall --shared -o $@ $^ $(LUALIB_CYGWIN) -lpthread
tluac.exe : $(SRC)
	gcc -g -Wall -o $@ $^ $(LUALIB_CYGWIN) -lpthread
	
tluac : $(SRC)
	gcc -o $@ $^ $(LUALIB) -lpthread

clean :
	rm -rf tluac.dll tluac.exe tluac