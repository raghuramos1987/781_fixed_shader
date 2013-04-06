#include "globals.h"
Vertex** vertices = 0;
Face** faces = 0;
unsigned int vertexcount = 0;
unsigned int facecount = 0;
int vertexnormals = 0;
int facenormals = 0;
float cx, cy, cz; 
float x_min, x_max, y_min, y_max, z_min, z_max; 
char *string_list[] = {
  "x", "y", "z", "nx", "ny", "nz", "vertex_indices"
};
