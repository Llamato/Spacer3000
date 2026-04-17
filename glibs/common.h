#ifndef ENGINE_COMMON_H
#define ENGINE_COMMON_H
    #ifndef GLFW_INCLUDE_NONE
        #define GLFW_INCLUDE_NONE
    #endif
    #include "glad/glad.h"
    #include <GLFW/glfw3.h>
    #include <stddef.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <math.h>

    #ifndef VECTOR_X
        #define VECTOR_X 0
    #endif
    #ifndef VECTOR_Y
        #define VECTOR_Y 1
    #endif
    #ifndef VECTOR_Z
        #define VECTOR_Z 2
    #endif

    #ifndef COLOR_R
        #define COLOR_R 0
    #endif
    #ifndef COLOR_G
        #define COLOR_G 1
    #endif
    #ifndef COLOR_B
        #define COLOR_B 2
    #endif
    #ifndef COLOR_A
        #define COLOR_A 3
    #endif

    #ifndef FLOATS_IN_POINT
        #define FLOATS_IN_POINT 3
    #endif
    #ifndef FLOATS_IN_COLOR
        #define FLOATS_IN_COLOR 4
    #endif

    #ifndef VERTS_IN_RECTANGLE
        #define VERTS_IN_RECTANGLE 4
    #endif
    #ifndef BOTTOM_LEFT_VERTEX_INDEX
        #define BOTTOM_LEFT_VERTEX_INDEX 0
    #endif
    #ifndef TOP_LEFT_VERTEX_INDEX
        #define TOP_LEFT_VERTEX_INDEX 1
    #endif
    #ifndef BOTTOM_RIGHT_VERTEX_INDEX
        #define BOTTOM_RIGHT_VERTEX_INDEX 2
    #endif
    #ifndef TOP_RIGHT_VERTEX_INDEX
        #define TOP_RIGHT_VERTEX_INDEX 3
    #endif

    #ifndef VERTS_IN_TRIANGLE
        #define VERTS_IN_TRIANGLE 3
    #endif
    #ifndef TRIANGLE_VERTEX_LEFT
        #define TRIANGLE_VERTEX_LEFT 0
    #endif
    #ifndef TRIANGLE_VERTEX_MIDDLE
        #define TRIANGLE_VERTEX_MIDDLE 2
    #endif
    #ifndef TRIANGLE_VERTEX_RIGHT
        #define TRIANGLE_VERTEX_RIGHT 1
    #endif

    struct Vector2 {
        GLfloat x;
        GLfloat y;
    };

    struct Color {
        GLfloat red;
        GLfloat green;
        GLfloat blue;
        GLfloat alpha;
    };

    struct GlObjectDataSet {
        // Data
        float *vertexDataBuffer;
        GLuint *vertexIndexBuffer;

        // VAO
        GLuint vao;

        // VBO
        GLuint vbo;
        size_t vertexCount;
        size_t vertexDataBufferSize;

        // IBO
        GLuint ibo;
        size_t indexCount;

        // Draw settings
        GLuint primitiveType;
        GLuint shaderProgram;
    };

    void printGlError(GLuint errorcode, unsigned int step);
    void makeGlObject(struct GlObjectDataSet *vds);
    struct GlObjectDataSet getRectangle(struct Vector2 center, struct Vector2 dimensions);
    struct GlObjectDataSet getTextRectangle(struct Vector2 center, struct Vector2 dimensions, struct Color textColor, struct Color backgroundColor);
    GLfloat *getTrianglefanCircle(struct Vector2 center, GLfloat radius, GLint polyCount, struct Color color);
    char* readShaderFile(const char *filename);
    GLuint makeGlShader(const char *source, GLuint type);
    void linkGlShaders(GLuint shaderProgram, GLuint vertexShader, GLuint fragmentShader);
    GLint getGlUniformLocation(GLuint shaderProgram, const char* varname);
    void setGlUniform1f(GLuint shaderProgram, const char* varname, GLfloat value);
    void setGlUniform2f(GLuint shaderProgram, const char* varname, GLfloat float1, GLfloat float2);
    void setGlUniform1uiv(GLuint shaderProgram, const char* varname, GLuint size, GLuint* uints);
    void setGlUniform4fv(GLuint shaderProgram, const char* varname, GLuint size, GLfloat* floats);
    void drawGlObject(struct GlObjectDataSet *ods);
    struct GlObjectDataSet initDefaultGlObject(void);
    void deleteGlObject(struct GlObjectDataSet *ods);
#endif