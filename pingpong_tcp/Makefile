CC = gcc
LIB = -lpthread

all: server client

server: gameserver.c game.c
	${CC} -o gameserver gameserver.c game.c ${LIB}

client: gameclient.c game.c
	${CC} -o gameclient gameclient.c game.c ${LIB}

clean:
	rm gameserver
	rm gameclient	
