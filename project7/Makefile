GCC_OPTIONS = -Wall -pedantic -I ../includes
LIBS = -L/usr/lib -lGLEW -lglut -lGL -lGLU -lX11 -lm -lXmu -pthread
#INCS = -l/usr/include

.cpp:
	g++ $@.cpp ../Common/GLShader.cpp $(GCC_OPTIONS) $(LIBS) -o $@

clean:
	rm *.o
	rm *.out
	rm project6
