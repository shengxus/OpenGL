# Makefile 

# assume the pic directory is the current one,
# change PIC_PATH if this is not the case

LIBRARIES = -framework OpenGL -framework GLUT -framework Foundation
COMPILER = g++

PROGRAM = sample
SOURCE = main.cpp
OBJECT = main.o libsoil.a libobj.a

.cpp.o: 
	$(COMPILER) -Wno-deprecated-declarations -c $<

all: $(PROGRAM)

$(PROGRAM): $(OBJECT)
	$(COMPILER) -o $(PROGRAM) $(OBJECT) $(LIBRARIES)

clean:
	-rm -rf *.o *~ .*~ $(PROGRAM)
