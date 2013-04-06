#ifndef GLOBALS
#define GLOBALS
typedef struct Vertex {
  float x, y, z;
  float nx, ny, nz;
} Vertex;

typedef struct Face {
  unsigned int count;
  unsigned int *vertices;
  float nx, ny, nz;
} Face;
extern char* string_list[];

extern Vertex** vertices;
extern Face** faces;
//extern float cx, cy, cz; 
//extern float x_min, x_max, y_min, y_max, z_min, z_max; 

extern unsigned int vertexcount;
extern unsigned int facecount;
extern int vertexnormals;
extern int facenormals;
extern float cx, cy, cz; 
extern float x_min, x_max, y_min, y_max, z_min, z_max; 
#endif
