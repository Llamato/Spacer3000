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
    GLfloat gclamp(GLfloat value, GLfloat max, GLfloat min);
    void translateVertexArray(GLfloat *vertexDataBuffer, size_t vertexCount, struct Vector2 *translationVector, unsigned int stride);
    void translateOrigin(GLfloat *vertexDataBuffer, size_t vertexCount, struct Vector2 *from, struct Vector2 *to, unsigned int stride);
    struct Vector2 rotateVector(struct Vector2 vector, float angle);
    void rotateVertexArray(GLfloat *vertexDataBuffer, size_t vertexCount, float rotationAngle, unsigned int stride);
    void convertScreenSpaceToLocal(GLfloat *vertexDataBuffer, size_t vertexCount, unsigned int stride);
    struct Vector2 convertPolarToCatesian(struct Vector2 polarVector);
    struct Vector2 getOutwardFacingEdgeNormal(struct Vector2 *edgeVector);
    struct Vector2 getInwardFacingEdgeNormal(struct Vector2 *edgeVector);
    struct Vector2 scaleVector(struct Vector2 *vector, GLfloat scaler);
    struct Vector2 addVectors(struct Vector2 *v1, struct Vector2 *v2);
    struct Vector2 subtractVectors(struct Vector2 *v1, struct Vector2 *v2);
    struct Vector2 multiplyVectors(struct Vector2 *v1, struct Vector2 *v2);
    GLfloat dotProduct(struct Vector2 *v1, struct Vector2 *v2);
    struct Vector2 projectVertexToLine(struct Vector2 *point, struct Vector2 *line);
    struct Vector2 *getPointsFromGlData(GLfloat *glData, size_t vertexCount, unsigned int stride);
#endif