LUALIB_CYGWIN=-I/usr/include -L/usr/local/bin -llua 
LUALIB=-I/usr/include -L/usr/local/bin -L/usr/lib -lm -llua -ldl -Wl,-E
SRC=\
src/main.c \
src/tluac-thread/tluac-thread.c \
src/tluac-lua/tluac-lua.c \
src/tluac-epoll/tluac-epoll.c \
src/util/produce-consume.c

all : clean win

win : tluac.dll tluac.exe

linux : tluac

tluac.dll : $(SRC)
	gcc -g -Wall --shared -o $@ $^ $(LUALIB_CYGWIN) -lpthread
tluac.exe : $(SRC)
	gcc -g -Wall -o $@ $^ $(LUALIB_CYGWIN) -lpthread
	
tluac : $(SRC)
	gcc -Wall -g -o $@ $^ $(LUALIB) -lpthread

clean :
	rm -rf tluac.dll tluac.exe tluac
