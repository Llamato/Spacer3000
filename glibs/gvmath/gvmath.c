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