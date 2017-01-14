xyz: main.c
	gcc main.c -o xyz -lGL -lGLU -lglut -lm -g -Wall
	
clean:
	rm xyz
	rm *.o
	rm *~
