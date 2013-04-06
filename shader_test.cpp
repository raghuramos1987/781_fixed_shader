////////////////////////////////////////////////////////
//
// 3D sample program
//
// Han-Wei Shen
//
////////////////////////////////////////////////////////

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <math.h>

#include <GL/glew.h>
#include <GL/glut.h> 
#include <GL/gl.h>
#include <GL/glext.h>
#include "globals.h"
#include "shader_test.h"
#include "bitmap.h"
#include "shaderSetup.h"
#define TITLE "781 Lab4"
#define X_RES 600
#define Y_RES 400
#define X_POS 600
#define Y_POS 400

GLhandleARB programObject;
GLhandleARB VertexShaderObject = 0;
GLhandleARB FragmentShaderObject = 0;

GLcharARB *vertexShaderSource;
GLcharARB *fragmentShaderSource;

int readShaderSource(char*, GLcharARB**, GLcharARB**); 
GLfloat* vert;
GLfloat* norm;
GLuint* ind;
float max_diff = 0;

GLfloat globalMatrix[16] = {
	1,0,0,0, 
	0,1,0,0,
	0,0,1,0,
	0,0,0,1
}; 
GLfloat modelviewmat[]={1,0,0,0,
                          0,1,0,0,
                          0,0,1,0,
                          0,0,0,1};
char *Cubemaps[6] = {

        "cubemap_blue_sofa_positive_x.bmp",
        "cubemap_blue_sofa_negative_x.bmp",
        "cubemap_blue_sofa_positive_y.bmp",
        "cubemap_blue_sofa_negative_y.bmp",
        "cubemap_blue_sofa_positive_z.bmp",
        "cubemap_blue_sofa_negative_z.bmp"
};
//char *Cubemaps[6] = {
    //"try.bmp",
    //"try.bmp",
    //"try.bmp",
    //"try.bmp",
    //"try.bmp",
    //"try.bmp"
//};
int win_width, win_height; 

float eye_x=0, eye_y=0, eye_z=0.5; 

int press_x, press_y; 
int release_x, release_y; 
float x_angle = 0.0; 
float y_angle = 0.0; 
float scale_size = 1; 

int xform_mode = 0; 

#define XFORM_NONE    0 
#define XFORM_ROTATE  1
#define XFORM_SCALE 2 
#define NUM_BUFFERS 2
#define glError() { \
        GLenum err = glGetError(); \
        while (err != GL_NO_ERROR) { \
                        fprintf(stdout, \
                                "glError: %s caught at %s:%u\n",\
                                (char *)gluErrorString(err),\
                                __FILE__, __LINE__); \
                        err = glGetError(); \
                } \
}
#define VERTS 0

#define BUFFER_OFFSET(n) ((GLubyte *)NULL + n)


int show_axis=-1; 
int poly_fill = 0;

bool transform_global = true; 
bool use_glsl = true; 
GLuint buffers[NUM_BUFFERS];

////////////////////////////////////////////////////////
// 4 x 4 matrix inversion - this is a hack because I return the 
// result to a global ...
//
float result[4][4] = {
  {1,0,0,0},
  {0,1,0,0},
  {0,0,1,0},
  {0,0,0,1}
}; 

using namespace std;

GLuint cubemap_texture;
GLubyte disp_map[317][324];
BITMAPINFO *BitmapInfo; /* Bitmap information */
GLubyte    *BitmapBits; /* Bitmap data */

void SetupCubeMap(char *argv[]);
void drawSkyBox( void ) ;
void drawSphere( void ) ;
void Transpose(GLfloat *mat);
void DrawPly();

///////////////////////////////////////////
void CopyVertNorm()
{
    unsigned int i, j, k;
    unsigned int v;
    for(i=2,j=0;i<(vertexcount*3);i+=3,j++)
    {
        vert[i-2] = vertices[j]->x;
        vert[i-1] = vertices[j]->y;
        vert[i] = vertices[j]->z;
        norm[i-2] = vertices[j]->nx;
        norm[i-1] = vertices[j]->ny;
        norm[i] = vertices[j]->nz;
    }
    k = 0;

    for(j=0;j<facecount;j++)
    {
        for(i=0;i < faces[j]->count;i++,k++)
        {
            ind[k] = faces[j]->vertices[i];
        }
    }
}
void swap(GLfloat* x, GLfloat* y)
{
    GLfloat temp;
    temp = *y;
    *y = *x;
    *x = temp;
}
void Transpose(GLfloat *mat)
{
    swap(mat+1, mat+4);
    swap(mat+2, mat+8);
    swap(mat+6, mat+9);
    swap(mat+3, mat+12);
    swap(mat+7, mat+13);
    swap(mat+11, mat+14);
}

void draw_axes()
{
  glLineWidth(3);        
  glBegin(GL_LINES); 
    glColor3f(1,0,0);    // red: x; green: y; blue: z
    glVertex3f(0,0,0); 
    glVertex3f(4,0,0); 
    glColor3f(0,1,0);  
    glVertex3f(0,0,0); 
    glVertex3f(0,4,0); 
    glColor3f(0,0,1); 
    glVertex3f(0,0,0);
    glVertex3f(0,0,4);
  glEnd(); 
  glLineWidth(1.0); 
}

///////////////////////////////////////

void set_lighting()
{
  GLfloat light_ambient[] = {.0,.0,.0,1}; 
  GLfloat light_diffuse[] = {.8,.8,.8,1};
  GLfloat light_specular[] = {1,1,1,1}; 

  GLfloat mat_specular[] = {.7, .7, .7,1}; 
  GLfloat mat_shine[] = {60}; 

  //  glShadeModel(GL_FLAT); 
  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE); 
  glEnable(GL_NORMALIZE); 
  glEnable(GL_COLOR_MATERIAL);  
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE); 

  glLightfv(GL_LIGHT0,GL_AMBIENT, light_ambient); 
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse); 
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular); 
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular); 
  glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shine); 
}

//////////////////////////////////////////////////////

void display()
{
  GLfloat light_pos[] = {0,0,0,1}; 

  glClearColor(0,0,0,1); 
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  glEnable(GL_DEPTH_TEST);  
  glEnable(GL_LIGHTING); 
  glEnable(GL_LIGHT0); 

  set_lighting(); 

  //----------------------------------------------
  // now define projection and modelview matrix 
  glMatrixMode(GL_PROJECTION); 
  glLoadIdentity(); 
  gluPerspective(60, 1, .001, 10000); 

  glMatrixMode(GL_MODELVIEW); 
  glLoadIdentity(); 
  glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
  gluLookAt(eye_x,eye_y,eye_z,0,0,0,0,1,0);

  //apply the global transformation to the whole world 
  glMultMatrixf((const float*)globalMatrix); 

  // change the polygon drawing mode as well as the lighting mode 
  if (show_axis == 1) {
    glDisable(GL_LIGHTING); 
    draw_axes(); 
    glEnable(GL_LIGHTING); 
  }
  if (!poly_fill)glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  if (!poly_fill) glDisable(GL_LIGHTING); 
  else glEnable(GL_LIGHTING); 

//----------------------------------------------
// now draw some simiple objects in the scene
    //DrawPly();
  glPushMatrix(); 
  drawSkyBox();
  drawSphere();
  glPopMatrix(); 

  //glColor3f(0,1,0); 
  //glPushMatrix(); 
  //glTranslatef(0,0.03,0); 
  //glScalef(1, .01, 1);       // floor
  //glutSolidCube(1); 
  //glPopMatrix(); 


  glColor3f(0,0,1); 
  glPushMatrix(); 
  glTranslatef(-.3, 0.12, -.3);   // sphere object
  glutSolidSphere(.1, 10, 10); 
  glPopMatrix(); 
//----------------------------------------------
// now draw my model(s)
// put your drawing code here ...
//----------------------------------------------

// now swap buffer to draw
  glutSwapBuffers(); 
}

///////////////////////////////////////////////////////////////////
//          Mouse callbacks to specify transformations 
///////////////////////////////////////////////////////////////////
void mymouse(int button, int state, int x, int y)
{
  GLdouble winx, winy, winz; 
  GLdouble winx2, winy2, winz2; 

  if (transform_global == true) {
    if (state == GLUT_DOWN) {
      press_x = x; press_y = y; 
      if (button == GLUT_LEFT_BUTTON)
    xform_mode = XFORM_ROTATE; 
      else if (button == GLUT_RIGHT_BUTTON) 
    xform_mode = XFORM_SCALE; 
    }
    else if (state == GLUT_UP) {
      xform_mode = XFORM_NONE; 
    }
  }
}

/////////////////////////////////////////////////////////

void mymotion(int x, int y)
{
  float dx, dy, dz; 

  if (transform_global == true) {
    if (xform_mode==XFORM_ROTATE) {
      x_angle += (x - press_x)/5.0; 
      if (x_angle > 180) x_angle -= 360; 
      else if (x_angle <-180) x_angle += 360; 
      press_x = x; 
	   
      y_angle += (y - press_y)/5.0; 
      if (y_angle > 180) y_angle -= 360; 
      else if (y_angle <-180) y_angle += 360; 
      press_y = y; 
    }
    else if (xform_mode == XFORM_SCALE){
      float old_size = scale_size;
      scale_size *= (1+ (y - press_y)/60.0); 
	if (scale_size <0) scale_size = old_size; 
	  press_y = y; 
    }
    glMatrixMode(GL_MODELVIEW); 
    //  global transformation 
    glLoadIdentity(); 
    glRotatef(x_angle, 0, 1,0); 
    glRotatef(y_angle, 1,0,0); 
    glScalef(scale_size, scale_size, scale_size); 
    glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat*)globalMatrix); 
  glutPostRedisplay(); 
  }

}




///////////////////////////////////////////////////////////////

void mykey(unsigned char key, int x, int y)
{
    switch(key) 
    {
        case 27: 
            exit(1);
            //free(BitmapBits);free(BitmapInfo);
            //glDetachShader(programObject, fragmentShaderObject);
            //glDetachShader(programObject, vertexShaderObject);
            //glDeleteShader(fragmentShaderObject);
            //glDeleteShader(vertexShaderObject);
            //glDeleteProgram(programObject);
            break;
        case 'f': poly_fill = !poly_fill; 
            break; 
        case 'n': 
            if (show_axis == 1) show_axis=-1;
            else show_axis = 1; 
            break;
        case 's': use_glsl = !use_glsl; 
            if (use_glsl)
            {
               cout<<"using own shader"<<endl; fflush(stdout);
               glUseProgramObjectARB(programObject);
            }
            else glUseProgramObjectARB(0);
          break; 
        case 'g': 
        if (transform_global==true) transform_global = false; 
        else transform_global = true; 
        break; 
    }
    glutPostRedisplay(); 
}
void SetupCubeMap(char *argv[])
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);
    glEnable(GL_DEPTH_TEST);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    //glGenTextures(1, &cubemap_texture);
    //glBindTexture(GL_TEXTURE_2D, cubemap_texture);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        //BitmapBits = LoadDIBitmap(Cubemaps[0], &BitmapInfo);
    //glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, BitmapInfo->bmiHeader.biWidth,
                     //BitmapInfo->bmiHeader.biWidth,
//, 0, GL_RGB, GL_UNSIGNED_BYTE, BitmapBits);
    glGenTextures(1, &cubemap_texture);
    //glEnable(GL_TEXTURE_CUBE_MAP);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_texture);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER,
    GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER,
    GL_LINEAR);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_REPLACE);
    for (int i=0; i<6; i++) 
    {
        BitmapBits = LoadDIBitmap(Cubemaps[i], &BitmapInfo);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, 0, GL_RGB,
                     BitmapInfo->bmiHeader.biWidth,
                     BitmapInfo->bmiHeader.biWidth,
                     0, GL_RGB, GL_UNSIGNED_BYTE, BitmapBits);
        gluBuild2DMipmaps(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, GL_RGB, 
                          128, 128, GL_RGB, GL_UNSIGNED_BYTE,
                          BitmapBits);
        free(BitmapBits);free(BitmapInfo);
    }
}  
   
///////////////////////////////////////////////////////////////
static void validateProgram(GLuint program) {
    const unsigned int BUFFER_SIZE = 512;
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    GLsizei length = 0;
    
    memset(buffer, 0, BUFFER_SIZE);
    glGetProgramInfoLog(program, BUFFER_SIZE, &length, buffer);
    if (length > 0)
        cout<< "Program " << program << " link error: " << buffer << endl;
    glValidateProgram(program);
    GLint status;
    glGetProgramiv(program, GL_VALIDATE_STATUS, &status);
    if (status == GL_FALSE)
        cout << "Error validating shader " << program << endl;
}
static void validateShader(GLuint shader, const char* file = 0) {
    const unsigned int BUFFER_SIZE = 512;
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    GLsizei length = 0;
    
    glGetShaderInfoLog(shader, BUFFER_SIZE, &length, buffer);
    if (length > 0) {
        cout << "Shader " << shader << " (" << (file?file:"") << 
            ") compile error: " << buffer << endl;
    }
}

void SetupGLSL(){
 
        GLhandleARB vertexShaderObject;
        GLhandleARB fragmentShaderObject;
      
        programObject = glCreateProgramObjectARB();
	vertexShaderObject = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
	fragmentShaderObject = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

	readShaderSource("test", &vertexShaderSource, &fragmentShaderSource); 

	glShaderSourceARB(vertexShaderObject,1,(const GLcharARB**)&vertexShaderSource,NULL);
	glShaderSourceARB(fragmentShaderObject,1,(const GLcharARB**)&fragmentShaderSource,NULL);

	glCompileShaderARB(vertexShaderObject);
        validateShader(vertexShaderObject, "test.vert");
	glCompileShaderARB(fragmentShaderObject);
        validateShader(fragmentShaderObject, "test.frag");

	glAttachObjectARB(programObject, vertexShaderObject);
	glAttachObjectARB(programObject, fragmentShaderObject);

	glLinkProgramARB(programObject);
        validateProgram(programObject);
   //GLint loc;
   //loc = glGetUniformLocation(programObject, "cubeMap");
   //glUniform1i(loc, 0);
   glError();
}


///////////////////////////////////////////////////////////////

void CalcScale()
{
    max_diff = (max_diff < (x_max - cx))? (x_max - cx):max_diff;
    max_diff = (max_diff < (y_max - cy))? (y_max - cy):max_diff;
    max_diff = (max_diff < (z_max - cz))? (z_max - cz):max_diff;
}
void SetupVbo()
{
    vert = new GLfloat[vertexcount*3];
    norm = new GLfloat[vertexcount*3];
    ind = new GLuint[facecount*faces[0]->count];
    CopyVertNorm();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glGenBuffers(NUM_BUFFERS, buffers);
    glBindBuffer(GL_ARRAY_BUFFER, buffers[VERTS]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vert)*6*vertexcount, NULL,
                 GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vert)*3*vertexcount,
                    vert);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vert)*3*vertexcount, 
            sizeof(norm)*3*vertexcount, norm);
}
void OpenGLInit(int argc, char** argv) 
{
  glutInit(&argc, argv); 
  glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH); 
  
    glutInitWindowSize(X_RES, Y_RES); 
    win_width = X_RES; win_height = Y_RES; 
    glutCreateWindow(TITLE);
  glutDisplayFunc(display); 
  glutMouseFunc(mymouse); 
  glutMotionFunc(mymotion);
  glutKeyboardFunc(mykey); 

  glewInit(); 

  if (glewGetExtension("GL_ARB_fragment_shader")      != GL_TRUE ||
      glewGetExtension("GL_ARB_vertex_shader")        != GL_TRUE ||
      glewGetExtension("GL_ARB_shader_objects")       != GL_TRUE ||
      glewGetExtension("GL_ARB_shading_language_100") != GL_TRUE)
    {
      fprintf(stderr, "Driver does not support OpenGL Shading Language\n");
      exit(1);
    }
  else fprintf(stderr, "GLSL supported and ready to go\n"); 
  
   glError();
  SetupCubeMap(argv);
   glError();
  SetupVbo();
   glError();
  SetupGLSL(); 
   glError();
               glUseProgramObjectARB(programObject);
   glError();

  glutMainLoop(); 
}

void DrawPly()
{
    GLenum i;
    GLfloat tempx, tempy, tempz;
    glMatrixMode(GL_MODELVIEW);
    glGetFloatv(GL_MODELVIEW_MATRIX, modelviewmat);
  glLoadIdentity(); 
  gluLookAt(eye_x,eye_y,eye_z,0,0,0,0,1,0);
  //cout<<sizeof(ind);fflush(stdout);

  //apply the global transformation to the whole world 
    //glScalef(1/max_diff, 1/max_diff, 1/max_diff);
  glMultMatrixf((const float*)globalMatrix); 
    glColor3f(1,1,1); 
    glTranslatef(-cx+0.2, -cy+.1, -cz);
    glBindBuffer(GL_ARRAY_BUFFER, buffers[VERTS]);
    glVertexPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(0));
    glEnableClientState(GL_VERTEX_ARRAY);
    glNormalPointer(GL_FLOAT, 0, 3*vertexcount*sizeof(vert)+
                    BUFFER_OFFSET(0));
    glEnableClientState(GL_NORMAL_ARRAY);
    glError();
    if(faces[0]->count == 3)
        i = GL_TRIANGLES;
    else if (faces[0]->count == 4)
        i = GL_QUADS;
    glDrawElements(i, facecount*faces[0]->count, GL_UNSIGNED_INT, 
            ind);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //glBegin(GL_TRIANGLES);
    //glVertex3f(vert[0], vert[1], vert[2]);
    //glVertex3f(vert[0], vert[1], vert[2]);
    //glVertex3f(vert[0], vert[1], vert[2]);
    //glEnd();
    glLoadMatrixf(modelviewmat);
}
void DrawObj(GLenum in)
{
   glError();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslatef(0,0.13,0); 
    glutSolidSphere(.1,50, 50);
    glPopMatrix();
    glColor3f(1,0,0); 
    glPushMatrix(); 
    glTranslatef(.3, 0.08, .3);     // cube object
    glutSolidCube(.1); 
    glPopMatrix(); 
    DrawPly();
    glMatrixMode(in);
    return;
}

void drawSphere()
{
   glEnable( GL_TEXTURE_CUBE_MAP);
   glBindTexture( GL_TEXTURE_CUBE_MAP, cubemap_texture);
   glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
   glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
   glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
   glError();

   glEnable(GL_TEXTURE_GEN_S);
   glEnable(GL_TEXTURE_GEN_T);
   glEnable(GL_TEXTURE_GEN_R);
    glMatrixMode(GL_TEXTURE);
    glPushMatrix();
    Transpose(globalMatrix);
    glMultMatrixf(globalMatrix);
    Transpose(globalMatrix);
    DrawObj(GL_TEXTURE);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

   glDisable(GL_TEXTURE_GEN_S);
   glDisable(GL_TEXTURE_GEN_T);
   glDisable(GL_TEXTURE_GEN_R);
   glDisable( GL_TEXTURE_CUBE_MAP);
}


void drawSkyBox( void ) 
{
        //Define planes for our cube
        GLfloat xPlane[] = { 1.0f, 0.0f, 0.0f, 0.0f };
        GLfloat yPlane[] = { 0.0f, 1.0f, 0.0f, 0.0f };
        GLfloat zPlane[] = { 0.0f, 0.0f, 1.0f, 0.0f };

        //Enable texture coordinates generation
        glEnable( GL_TEXTURE_GEN_S );
        glEnable( GL_TEXTURE_GEN_T );
        glEnable( GL_TEXTURE_GEN_R );

        //Set texture environment parameter
        glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );

        //Enable the cubemap extension
        glEnable( GL_TEXTURE_CUBE_MAP);
    glBindTexture( GL_TEXTURE_CUBE_MAP, cubemap_texture);

        //Generate texture coordinates
        glTexGeni( GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );
        glTexGeni( GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );
        glTexGeni( GL_R, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );

        glTexGenfv( GL_S, GL_OBJECT_PLANE, xPlane );
        glTexGenfv( GL_T, GL_OBJECT_PLANE, yPlane );
        glTexGenfv( GL_R, GL_OBJECT_PLANE, zPlane );

        //Skybox rendering needs disabling of z buffer
        glDisable( GL_DEPTH_TEST );

        glMatrixMode( GL_MODELVIEW );

        //Draw the cube
        glBegin( GL_QUADS );
                //Right
        glTexCoord2f( 0, 0 );
        glVertex3f( 1, -.3, -1 );

        glTexCoord2f( 1, 0 );
        glVertex3f( 1, -.3, 1 );

        glTexCoord2f( 1, 1 );
        glVertex3f( 1, 1, 1 );

        glTexCoord2f( 0, 1 );
        glVertex3f( 1, 1, -1 );

                //Left
                glTexCoord2f( 0, 0 );
        glVertex3f( -1, -.3, 1 );

        glTexCoord2f( 1, 0 );
        glVertex3f( -1, -.3, -1 );

        glTexCoord2f( 1, 1 );
        glVertex3f( -1, 1, -1 );

        glTexCoord2f( 0, 1 );
        glVertex3f( -1, 1, 1 );

                //Top
                glTexCoord2f( 0, 0 );
        glVertex3f( -1, 1, -1 );

        glTexCoord2f( 1, 0 );
        glVertex3f( 1, 1, -1 );

        glTexCoord2f( 1, 1 );
        glVertex3f( 1, 1, 1 );

        glTexCoord2f( 0, 1 );
        glVertex3f( -1, 1, 1 );

                //Bottom
                glTexCoord2f( 0, 0 );
        glVertex3f( -1, -.3, 1 );

        glTexCoord2f( 1, 0 );
        glVertex3f( 1, -.3, 1 );

        glTexCoord2f( 1, 1 );
        glVertex3f( 1, -.3, -1 );

        glTexCoord2f( 0, 1 );
        glVertex3f( -1, -.3, -1 );
                //Front
        glTexCoord2f( 0, 0 );
        glVertex3f( -1, -.3, -1 );

        glTexCoord2f( 1, 0 );
        glVertex3f( 1, -.3, -1 );

        glTexCoord2f( 1, 1 );
        glVertex3f( 1, 1, -1 );

        glTexCoord2f( 0, 1 );
        glVertex3f( -1, 1, -1 );


                //Back
                glTexCoord2f( 0, 0 );
        glVertex3f( 1, -.3, 1 );

        glTexCoord2f( 1, 0 );
        glVertex3f( -1, -.3, 1 );

        glTexCoord2f( 1, 1 );
        glVertex3f( -1, 1, 1 );

        glTexCoord2f( 0, 1 );
        glVertex3f( 1, 1, 1 );



    glEnd();

        glEnable( GL_DEPTH_TEST );

        glDisable( GL_TEXTURE_CUBE_MAP);

        glDisable( GL_TEXTURE_GEN_S );
        glDisable( GL_TEXTURE_GEN_T );
        glDisable( GL_TEXTURE_GEN_R );
}



