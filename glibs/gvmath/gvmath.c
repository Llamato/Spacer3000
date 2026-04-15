#include <math.h>
#include "gvmath.h"
GLfloat gabsf(GLfloat value) { 
  return value < 0.0f ? value * -1.0f : value; 
}

GLfloat max(GLfloat values[], size_t numValues) {
  GLfloat max = values[0];
  for (size_t currentValue = 1; currentValue < numValues; currentValue++) {
    if (max < values[currentValue]) {
      max = values[currentValue];
    }
  }
  return max;
}

GLfloat min(GLfloat values[], size_t numValues) {
  GLfloat min = values[0];
  for (size_t currentValue = 1; currentValue < numValues; currentValue++) {
    if (min > values[currentValue]) {
      min = values[currentValue];
    }
  }
  return min;
}

GLfloat *combineVertexDataArrays(GLfloat *array1, size_t size1, GLfloat *array2, size_t size2) {
  size_t combinedSize = size1 + size2;
  GLfloat *combinedArray = malloc(combinedSize * sizeof(GLfloat));
  for (size_t i = 0; i < size1; i++) {
    combinedArray[i] = array1[i];
  }
  for (size_t i = 0; i < size2; i++) {
    combinedArray[size1 + i] = array2[i];
  }
  return combinedArray;
}

void scaleVertexDataArray(GLfloat *dataArray, size_t vertexCount, GLfloat scale, unsigned int stride) {
  for (size_t i = 0; i < vertexCount; i++) {
    size_t baseIndex = i * stride;
    dataArray[baseIndex] *= scale;
    dataArray[baseIndex + 1] *= scale;
    dataArray[baseIndex + 2] *= scale;
  }
}

struct Vector2 getTriangleMiddleFromVertexPositions(struct Vector2 vertex0Position, struct Vector2 vertex1Position, struct Vector2 vertex2Position) {
  struct Vector2 middle;
  middle.x = (vertex0Position.x + vertex1Position.x + vertex2Position.x) / 3.0f;
  middle.y = (vertex0Position.y + vertex1Position.y + vertex2Position.y) / 3.0f;
  return middle;
}

GLfloat getMagnitude(struct Vector2 *vector) {
  return sqrtf(pow(vector->x, 2) + pow(vector->y, 2));
}

void normalize(struct Vector2 *vector) {
  GLfloat magnitude = getMagnitude(vector);
  if (magnitude > 0.00001f) {
    vector->x /= magnitude;
    vector->y /= magnitude;
  } else {
    vector->x = 0.0f;
    vector->y = 0.0f;
  }
}

struct Vector2 getVectorBetweenPoints(struct Vector2 *from, struct Vector2 *to) {
  struct Vector2 wayVector;
  wayVector.x = to->x - from->x;
  wayVector.y = to->y - from->y;
  return wayVector;
}

GLfloat getDistance(struct Vector2 *from, struct Vector2 *to) {
  struct Vector2 wayVector = getVectorBetweenPoints(from, to);
  return gabsf(getMagnitude(&wayVector));
}

struct Vector2 getDirection(struct Vector2 *from, struct Vector2 *to) {
  struct Vector2 direction;
  direction = getVectorBetweenPoints(from, to);
  normalize(&direction);
  return direction;
}

struct Vector2 getPerpendicularVector(struct Vector2 vector) {
  struct Vector2 parallelVector;
  parallelVector.x = -vector.y;
  parallelVector.y = vector.x;
  return parallelVector;
}

GLfloat gclamp(GLfloat value, GLfloat max, GLfloat min) {
  if (value < min) {
    value = min;
  } else if (value > max) {
    value = max;
  }
  return value;
}

void translateVertexArray(GLfloat *vertexDataBuffer, size_t vertexCount, struct Vector2 *translationVector, unsigned int stride) {
  for (size_t currentVertex = 0; currentVertex < vertexCount; currentVertex++) {
    vertexDataBuffer[currentVertex * stride + VECTOR_X] += translationVector->x;
    vertexDataBuffer[currentVertex * stride + VECTOR_Y] += translationVector->y;
  }
}

void translateOrigin(GLfloat *vertexDataBuffer, size_t vertexCount, struct Vector2 *from, struct Vector2 *to, unsigned int stride) {
  struct Vector2 offset;
  offset.x = from->x - to->x;
  offset.y = from->y - to->y;
  for (size_t currentVertexStartIndex = 0;
       currentVertexStartIndex < vertexCount * stride;
       currentVertexStartIndex += stride) {
    vertexDataBuffer[currentVertexStartIndex + VECTOR_X] += offset.x;
    vertexDataBuffer[currentVertexStartIndex + VECTOR_Y] += offset.y;
  }
}

struct Vector2 rotateVector(struct Vector2 vector, float angle) {
  struct Vector2 rotated;
  rotated.x = vector.x * cosf(angle) - vector.y * sinf(angle);
  rotated.y = vector.x * sinf(angle) + vector.y * cosf(angle);
  return rotated;
}

void rotateVertexArray(GLfloat *vertexDataBuffer, size_t vertexCount, float rotationAngle, unsigned int stride) {
  for (size_t currentVertexStartIndex = 0; currentVertexStartIndex < vertexCount * stride; currentVertexStartIndex += stride) {
    GLfloat newX = vertexDataBuffer[currentVertexStartIndex] * cosf(rotationAngle) - vertexDataBuffer[currentVertexStartIndex + 1] * sinf(rotationAngle);
    GLfloat newY = vertexDataBuffer[currentVertexStartIndex] * sinf(rotationAngle) + vertexDataBuffer[currentVertexStartIndex + 1] * cosf(rotationAngle);
    vertexDataBuffer[currentVertexStartIndex] = newX;
    vertexDataBuffer[currentVertexStartIndex + 1] = newY;
  }
}

void convertScreenSpaceToLocal(GLfloat *vertexDataBuffer, size_t vertexCount, unsigned int stride) {
  GLfloat xSum = 0;
  GLfloat ySum = 0;
  for (size_t currentVertex = 0; currentVertex < vertexCount; currentVertex++) {
    xSum += vertexDataBuffer[currentVertex * stride + VECTOR_X];
    ySum += vertexDataBuffer[currentVertex * stride + VECTOR_Y];
  }
  struct Vector2 localCenter;
  localCenter.x = xSum / vertexCount;
  localCenter.y = ySum / vertexCount;
  for (size_t currentVertex = 0; currentVertex < vertexCount; currentVertex++) {
    vertexDataBuffer[currentVertex * stride + VECTOR_X] -= localCenter.x;
    vertexDataBuffer[currentVertex * stride + VECTOR_Y] -= localCenter.y;
  }
}

struct Vector2 convertPolarToCatesian(struct Vector2 polarVector) {
  struct Vector2 catesianVector;
  catesianVector.x = polarVector.x * cosf(polarVector.y);
  catesianVector.y = polarVector.x * sinf(polarVector.y);
  return catesianVector;
}

struct Vector2 getOutwardFacingEdgeNormal(struct Vector2 *edgeVector) {
  struct Vector2 outwardFacingNormal = {-edgeVector->y, edgeVector->x};
  return outwardFacingNormal;
}

struct Vector2 getInwardFacingEdgeNormal(struct Vector2 *edgeVector) {
  struct Vector2 inwardFacingNormal = {edgeVector->y, -edgeVector->x};
  return inwardFacingNormal;
}

struct Vector2 scaleVector(struct Vector2 *vector, GLfloat scaler) {
  struct Vector2 result = {vector->x * scaler, vector->y * scaler};
  return result;
}

struct Vector2 addVectors(struct Vector2 *v1, struct Vector2 *v2) {
  struct Vector2 result = {v1->x + v2->x, v1->y + v2->y};
  return result;
}

struct Vector2 subtractVectors(struct Vector2 *v1, struct Vector2 *v2) {
  struct Vector2 result = {v1->x - v2->x, v1->y - v2->y};
  return result;
}

struct Vector2 multiplyVectors(struct Vector2 *v1, struct Vector2 *v2) {
  struct Vector2 result;
  result.x = v1->x * v2->x;
  result.y = v1->y * v2->y;
  return result;
}

GLfloat dotProduct(struct Vector2 *v1, struct Vector2 *v2) {
  return v1->x * v2->x + v1->y * v2->y;
}

struct Vector2 projectVertexToLine(struct Vector2 *point, struct Vector2 *line) {
  GLfloat lineMagnitude = getMagnitude(line);
  GLfloat divisor = lineMagnitude * lineMagnitude;
  GLfloat scaler = dotProduct(point, line) / divisor;
  return scaleVector(line, scaler);
}

struct Vector2 *getPointsFromGlData(GLfloat *glData, size_t vertexCount, unsigned int stride) {
  struct Vector2 *results = (struct Vector2 *)malloc(vertexCount * sizeof(struct Vector2));
  for (size_t currentVertex = 0; currentVertex < vertexCount; currentVertex++) {
    struct Vector2 currentPoint;
    currentPoint.x = glData[currentVertex * stride + VECTOR_X];
    currentPoint.y = glData[currentVertex * stride + VECTOR_Y];
    results[currentVertex] = currentPoint;
  }
  return results;
}