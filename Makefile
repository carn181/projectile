all: build

# Too Lazy rn to write a proper makefile
build: main.c
	gcc main.c -lm -lSDL2 -lSDL2_ttf -o projectile

clean:
	rm projectile
