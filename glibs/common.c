#include "common.h"

#include <math.h>

#ifndef M_PI
  #define M_PI 3.14159265358979323846
#endif

struct GlObjectDataSet getRectangle(struct Vector2 center, struct Vector2 dimensions) {
  struct GlObjectDataSet rectangle;
  rectangle.vertexCount = VERTS_IN_RECTANGLE;
  rectangle.vertexDataBufferSize = rectangle.vertexCount * FLOATS_IN_POINT * sizeof(GLfloat);
  rectangle.vertexDataBuffer = malloc(rectangle.vertexDataBufferSize);

  const size_t floatsInVertex = FLOATS_IN_POINT + 2 * FLOATS_IN_COLOR;
  GLfloat left = center.x - dimensions.x / 2.0f;
  GLfloat right = center.x + dimensions.x / 2.0f;
  GLfloat bottom = center.y - dimensions.y / 2.0f;
  GLfloat top = center.y + dimensions.y / 2.0f;

  rectangle.vertexDataBuffer[BOTTOM_LEFT_VERTEX_INDEX * FLOATS_IN_POINT + VECTOR_X] = left;
  rectangle.vertexDataBuffer[BOTTOM_LEFT_VERTEX_INDEX * FLOATS_IN_POINT + VECTOR_Y] = bottom;
  rectangle.vertexDataBuffer[BOTTOM_LEFT_VERTEX_INDEX * FLOATS_IN_POINT + VECTOR_Z] = 0.0f;
  rectangle.vertexDataBuffer[TOP_LEFT_VERTEX_INDEX * FLOATS_IN_POINT + VECTOR_X] = left;
  rectangle.vertexDataBuffer[TOP_LEFT_VERTEX_INDEX * FLOATS_IN_POINT + VECTOR_Y] = top;
  rectangle.vertexDataBuffer[TOP_LEFT_VERTEX_INDEX * FLOATS_IN_POINT + VECTOR_Z] = 0.0f;
  rectangle.vertexDataBuffer[BOTTOM_RIGHT_VERTEX_INDEX * FLOATS_IN_POINT + VECTOR_X] = right;
  rectangle.vertexDataBuffer[BOTTOM_RIGHT_VERTEX_INDEX * FLOATS_IN_POINT + VECTOR_Y] = bottom;
  rectangle.vertexDataBuffer[BOTTOM_RIGHT_VERTEX_INDEX * FLOATS_IN_POINT + VECTOR_Z] = 0.0f;
  rectangle.vertexDataBuffer[TOP_RIGHT_VERTEX_INDEX * FLOATS_IN_POINT + VECTOR_X] = right;
  rectangle.vertexDataBuffer[TOP_RIGHT_VERTEX_INDEX * FLOATS_IN_POINT + VECTOR_Y] = top;
  rectangle.vertexDataBuffer[TOP_RIGHT_VERTEX_INDEX * FLOATS_IN_POINT + VECTOR_Z] = 0.0f;
  rectangle.indexCount = 6;
  rectangle.vertexIndexBuffer = malloc(rectangle.indexCount * sizeof(GLuint));
  rectangle.vertexIndexBuffer[0] = BOTTOM_LEFT_VERTEX_INDEX;
  rectangle.vertexIndexBuffer[1] = TOP_LEFT_VERTEX_INDEX;
  rectangle.vertexIndexBuffer[2] = BOTTOM_RIGHT_VERTEX_INDEX;
  rectangle.vertexIndexBuffer[3] = TOP_LEFT_VERTEX_INDEX;
  rectangle.vertexIndexBuffer[4] = TOP_RIGHT_VERTEX_INDEX;
  rectangle.vertexIndexBuffer[5] = BOTTOM_RIGHT_VERTEX_INDEX;
  rectangle.primitiveType = GL_TRIANGLES;
  return rectangle;
}

struct GlObjectDataSet getTextRectangle(struct Vector2 center, struct Vector2 dimensions, struct Color textColor, struct Color backgroundColor) {
    const size_t floatsInVertex = FLOATS_IN_POINT + 2 * FLOATS_IN_COLOR;

    struct GlObjectDataSet rectangle;
    rectangle.vertexCount = VERTS_IN_RECTANGLE;
    rectangle.vertexDataBufferSize = rectangle.vertexCount * floatsInVertex * sizeof(GLfloat);
    rectangle.vertexDataBuffer = malloc(rectangle.vertexDataBufferSize);

    GLfloat left = center.x - dimensions.x / 2.0f;
    GLfloat right = center.x + dimensions.x / 2.0f;
    GLfloat bottom = center.y - dimensions.y / 2.0f;
    GLfloat top = center.y + dimensions.y / 2.0f;
    
    // Vertex 0: bottom-left
    rectangle.vertexDataBuffer[BOTTOM_LEFT_VERTEX_INDEX * floatsInVertex + VECTOR_X] = left;
    rectangle.vertexDataBuffer[BOTTOM_LEFT_VERTEX_INDEX * floatsInVertex + VECTOR_Y] = bottom;
    rectangle.vertexDataBuffer[BOTTOM_LEFT_VERTEX_INDEX * floatsInVertex + VECTOR_Z] = 0.0f;
    
    // Vertex 1: top-left
    rectangle.vertexDataBuffer[TOP_LEFT_VERTEX_INDEX * floatsInVertex + VECTOR_X] = left;
    rectangle.vertexDataBuffer[TOP_LEFT_VERTEX_INDEX * floatsInVertex + VECTOR_Y] = top;
    rectangle.vertexDataBuffer[TOP_LEFT_VERTEX_INDEX * floatsInVertex + VECTOR_Z] = 0.0f;
    
    // Vertex 2: bottom-right
    rectangle.vertexDataBuffer[BOTTOM_RIGHT_VERTEX_INDEX * floatsInVertex + VECTOR_X] = right;
    rectangle.vertexDataBuffer[BOTTOM_RIGHT_VERTEX_INDEX * floatsInVertex + VECTOR_Y] = bottom;
    rectangle.vertexDataBuffer[BOTTOM_RIGHT_VERTEX_INDEX * floatsInVertex + VECTOR_Z] = 0.0f;
    
    // Vertex 3: top-right
    rectangle.vertexDataBuffer[TOP_RIGHT_VERTEX_INDEX * floatsInVertex + VECTOR_X] = right;
    rectangle.vertexDataBuffer[TOP_RIGHT_VERTEX_INDEX * floatsInVertex + VECTOR_Y] = top;
    rectangle.vertexDataBuffer[TOP_RIGHT_VERTEX_INDEX * floatsInVertex + VECTOR_Z] = 0.0f;
    
    // Set colors
    for (size_t i = 0; i < VERTS_IN_RECTANGLE; i++) {
      size_t base = i * floatsInVertex;
      
      // Text color
      rectangle.vertexDataBuffer[base + FLOATS_IN_POINT + COLOR_R] = textColor.red;
      rectangle.vertexDataBuffer[base + FLOATS_IN_POINT + COLOR_G] = textColor.green;
      rectangle.vertexDataBuffer[base + FLOATS_IN_POINT + COLOR_B] = textColor.blue;
      rectangle.vertexDataBuffer[base + FLOATS_IN_POINT + COLOR_A] = textColor.alpha;
      
      // Background color
      rectangle.vertexDataBuffer[base + FLOATS_IN_POINT + FLOATS_IN_COLOR + COLOR_R] = backgroundColor.red;
      rectangle.vertexDataBuffer[base + FLOATS_IN_POINT + FLOATS_IN_COLOR + COLOR_G] = backgroundColor.green;
      rectangle.vertexDataBuffer[base + FLOATS_IN_POINT + FLOATS_IN_COLOR + COLOR_B] = backgroundColor.blue;
      rectangle.vertexDataBuffer[base + FLOATS_IN_POINT + FLOATS_IN_COLOR + COLOR_A] = backgroundColor.alpha;
    }
    
    rectangle.indexCount = 6;
    rectangle.vertexIndexBuffer = malloc(rectangle.indexCount * sizeof(GLuint));
    rectangle.vertexIndexBuffer[0] = 0;
    rectangle.vertexIndexBuffer[1] = 1;
    rectangle.vertexIndexBuffer[2] = 2;
    rectangle.vertexIndexBuffer[3] = 1;
    rectangle.vertexIndexBuffer[4] = 3;
    rectangle.vertexIndexBuffer[5] = 2;
    rectangle.primitiveType = GL_TRIANGLES;
    
    return rectangle;
}

GLfloat *getTrianglefanCircle(struct Vector2 center, GLfloat radius, GLint polyCount, struct Color color) {
  float rotAngle = M_PI * 2.0f / polyCount;
  GLfloat vertCount = polyCount + 2;
  GLfloat *circleData = malloc(vertCount * (FLOATS_IN_POINT + FLOATS_IN_COLOR) * sizeof(GLfloat));

  circleData[VECTOR_X] = center.x;
  circleData[VECTOR_Y] = center.y;
  circleData[VECTOR_Z] = 0.0f;
  circleData[FLOATS_IN_POINT + COLOR_R] = color.red;
  circleData[FLOATS_IN_POINT + COLOR_G] = color.green;
  circleData[FLOATS_IN_POINT + COLOR_B] = color.blue;
  circleData[FLOATS_IN_POINT + COLOR_A] = color.alpha;

  for (unsigned int currentVertex = 1; currentVertex < vertCount; currentVertex++) {
    GLfloat currentX = center.x + radius * cosf(rotAngle * currentVertex);
    GLfloat currentY = center.y + radius * sinf(rotAngle * currentVertex);
    size_t currentIndex = currentVertex * (FLOATS_IN_POINT + FLOATS_IN_COLOR);
    circleData[currentIndex + VECTOR_X] = currentX;
    circleData[currentIndex + VECTOR_Y] = currentY;
    circleData[currentIndex + VECTOR_Z] = 0.0f;
    circleData[currentIndex + FLOATS_IN_POINT + COLOR_R] = color.red;
    circleData[currentIndex + FLOATS_IN_POINT + COLOR_G] = color.green;
    circleData[currentIndex + FLOATS_IN_POINT + COLOR_B] = color.blue;
    circleData[currentIndex + FLOATS_IN_POINT + COLOR_A] = color.alpha;
  }
  return circleData;
}