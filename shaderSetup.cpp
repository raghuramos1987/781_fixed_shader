

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <math.h>
#include <unistd.h>

#include <GL/glew.h>
#include <GL/glut.h> 
#include <GL/gl.h>
#include <GL/glext.h>

///////////////////////////////////////////////////////////////
// GLSL setup code 
///////////////////////////////////////////////////////////////


/////////////////////////////////////
//GLSL specific types and variables


typedef enum {
    EVertexShader,
    EFragmentShader,
} EShaderType;


int shaderSize(char *fileName, EShaderType shaderType)
{
    //
    // Returns the size in bytes of the shader fileName.
    // If an error occurred, it returns -1.
    //
    // File name convention:
    //
    // <fileName>.vert
    // <fileName>.frag
    //
    FILE* fd; 
    char name[100];
    int count = -1;

    strcpy(name, fileName);

    switch (shaderType)
    {
        case EVertexShader:
            strcat(name, ".vert");
            break;
        case EFragmentShader:
            strcat(name, ".frag");
            break;
        default:
            printf("ERROR: unknown shader file type\n");
            exit(1);
            break;
    }

    //
    // Open the file, seek to the end to find its length
    //
#ifdef WIN32 /*[*/
    fd = fopen(name, "r");
    if (fd != NULL)
    {			
		fseek(fd, 0, SEEK_END);
		count = ftell(fd)+1; 
        fclose(fd);
    }
#else /*][*/
    fd = fopen(name, "r");
    if (fd != NULL)
    {
        fseek(fd, 0, SEEK_END);
		count = ftell(fd)+1; 
        fclose(fd);
    }
#endif /*]*/
    fprintf(stderr, "*** %d\n", count); 
    return count;
}


int readShader(char *fileName, EShaderType shaderType, char *shaderText, int size)
{
    //
    // Reads a shader from the supplied file and returns the shader in the
    // arrays passed in. Returns 1 if successful, 0 if an error occurred.
    // The parameter size is an upper limit of the amount of bytes to read.
    // It is ok for it to be too big.
    //
    FILE *fh;
    char name[100];
    int count;

    strcpy(name, fileName);

    switch (shaderType) 
    {
        case EVertexShader:
            strcat(name, ".vert");
            break;
        case EFragmentShader:
            strcat(name, ".frag");
            break;
        default:
            printf("ERROR: unknown shader file type\n");
            exit(1);
            break;
    }

    //
    // Open the file
    //
    fh = fopen(name, "r");
    if (fh==NULL)
        return -1;

    //
    // Get the shader from a file.
    //
    fseek(fh, 0, SEEK_SET);
    count = fread(shaderText, 1, size, fh);
    shaderText[count] = '\0';

    if (ferror(fh))
        count = 0;

    fclose(fh);
    return count;
}


/*public*/
int readShaderSource(char *fileName, GLcharARB **vertexShader, GLcharARB **fragmentShader)
{
    int vSize, fSize;

    //
    // Allocate memory to hold the source of our shaders.
    //
    vSize = shaderSize(fileName, EVertexShader);
    fSize = shaderSize(fileName, EFragmentShader);

    if ((vSize == -1) || (fSize == -1))
    {
        printf("Cannot determine size of the shader %s\n", fileName);
        return 0;
    }

    *vertexShader = (GLcharARB *) malloc(vSize);
    *fragmentShader = (GLcharARB *) malloc(fSize);

    //
    // Read the source code
    //
    if (!readShader(fileName, EVertexShader, *vertexShader, vSize))
    {
        printf("Cannot read the file %s.vert\n", fileName);
        return 0;
    }

    if (!readShader(fileName, EFragmentShader, *fragmentShader, fSize))
    {
        printf("Cannot read the file %s.frag\n", fileName);
        return 0;
    }

    return 1;
}


GLint getUniLoc(GLhandleARB program, const GLcharARB *name)
{
    GLint loc;

    loc = glGetUniformLocationARB(program, name);

    if (loc == -1)
        printf("No such uniform named \"%s\"\n", name);
    return loc;
}
