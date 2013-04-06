
typedef enum {
    EVertexShader,
    EFragmentShader,
} EShaderType;
int readShader(char *fileName, EShaderType shaderType, char *shaderText,
               int size);
int readShaderSource(char *fileName, GLcharARB **vertexShader,
                     GLcharARB **fragmentShader);
