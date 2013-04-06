CC = g++
CFLAGS = -g 
LIBS = -lm -lX11 -lXi -lXmu -lglut -lGL -lGLU -lGLEW
DEPS = ply.h plyread.h normalsply.h shaderSetup.h shader_test.h bitmap.h main.h globals.h 
OBJ = ply.o plyread.o normalsply.o shaderSetup.o shader_test.o bitmap.o main.o globals.o 
OUT = shader

%.o: %.cpp $(DEPS) 
	$(CC) -c -o $@ $< $(CFLAGS)


$(OUT): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS) 

clean:
	rm *.o $(OUT)
