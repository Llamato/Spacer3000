#ifndef ENGINE_COMMON_H
#define ENGINE_COMMON_H
    #include <GLFW/glfw3.h>
    #include "../glad/glad.h"

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

    struct GlObjectDataSet getRectangle(struct Vector2 center, struct Vector2 dimensions);
    struct GlObjectDataSet getTextRectangle(struct Vector2 center, struct Vector2 dimensions, struct Color textColor, struct Color backgroundColor);
    GLfloat *getTrianglefanCircle(struct Vector2 center, GLfloat radius, GLint polyCount, struct Color color);
#endif