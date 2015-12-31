FILE=pong-pong

all: $(FILE).c 
	gcc $(FILE).c -o $(FILE) `sdl-config --libs --cflags`
clean:
	rm -f $(FILE)
