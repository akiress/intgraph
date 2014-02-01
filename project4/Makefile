GCC_OPTIONS = -Wall -pedantic -I ../include
LIBS = -L/usr/lib -lGLEW -lglut -lGL -lGLU -lX11 -lm -lXmu
#INCS = -l/usr/include

.cpp:
	g++ $@.cpp $(GCC_OPTIONS) $(LIBS) -o $@

clean:
	rm *.o
	rm *.out
