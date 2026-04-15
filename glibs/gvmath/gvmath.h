#ifndef GVMATH_H
#define GVMATH_H
    #include "../common.h"
    GLfloat gabsf(GLfloat value);
    GLfloat max(GLfloat values[], size_t numValues);
    GLfloat min(GLfloat values[], size_t numValues);
    GLfloat *combineVertexDataArrays(GLfloat *array1, size_t size1, GLfloat *array2, size_t size2);
    void scaleVertexDataArray(GLfloat *dataArray, size_t vertexCount, GLfloat scale, unsigned int stride);
    struct Vector2 getTriangleMiddleFromVertexPositions(struct Vector2 vertex0Position, struct Vector2 vertex1Position, struct Vector2 vertex2Position);
    GLfloat getMagnitude(struct Vector2 *vector);
    void normalize(struct Vector2 *vector);
    struct Vector2 getVectorBetweenPoints(struct Vector2 *from, struct Vector2 *to);
    GLfloat getDistance(struct Vector2 *from, struct Vector2 *to);
    struct Vector2 getDirection(struct Vector2 *from, struct Vector2 *to);
    struct Vector2 getPerpendicularVector(struct Vector2 vector);
#endif