#include "common.h"
#include <string.h>
#ifndef M_PI
  #define M_PI 3.14159265358979323846
#endif

#ifndef ERROR_MESSAGE_MAX_LENGTH
  #define ERROR_MESSAGE_MAX_LENGTH 2048
#endif

void printGlError(GLuint errorcode, unsigned int step) {
  printf("OpenGL Error: %x in step %u\n", errorcode, step);
  switch (errorcode) {
  case GL_INVALID_ENUM:
    printf("GLenum argument out of range.\n");
    break;
  case GL_INVALID_VALUE:
    printf("Numeric argument out of range.\n");
    break;
  case GL_INVALID_OPERATION:
    printf("Operation illegal in current state.\n");
    break;
  case GL_INVALID_FRAMEBUFFER_OPERATION:
    printf("Invalid frame buffer operation.\n");
    break;
  case GL_OUT_OF_MEMORY:
    printf("Not enough memory left to execute function.\n");
    break;
  default:
    printf("Unknown OpenGL Error.\n");
  }
}

void makeGlObject(struct GlObjectDataSet *vds) {
  GLenum error = GL_NO_ERROR;

  glGenVertexArrays(1, &vds->vao);
  if ((error = glGetError()) != GL_NO_ERROR)
    printGlError(error, 1);

  glGenBuffers(1, &vds->vbo);
  if ((error = glGetError()) != GL_NO_ERROR)
    printGlError(error, 2);

  glBindVertexArray(vds->vao);
  if ((error = glGetError()) != GL_NO_ERROR)
    printGlError(error, 3);

  glBindBuffer(GL_ARRAY_BUFFER, vds->vbo);
  if ((error = glGetError()) != GL_NO_ERROR)
    printGlError(error, 4);

  glBufferData(GL_ARRAY_BUFFER, vds->vertexDataBufferSize, vds->vertexDataBuffer, GL_DYNAMIC_DRAW);
  if ((error = glGetError()) != GL_NO_ERROR)
    printGlError(error, 5);

  if (vds->indexCount > 0) {
    glGenBuffers(1, &vds->ibo);
    if ((error = glGetError()) != GL_NO_ERROR)
      printGlError(error, 6);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vds->ibo);
    if ((error = glGetError()) != GL_NO_ERROR)
      printGlError(error, 7);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, vds->indexCount * sizeof(GLuint), vds->vertexIndexBuffer, GL_STATIC_DRAW);
    if ((error = glGetError()) != GL_NO_ERROR)
      printGlError(error, 8);
  }
}

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

char *readShaderFile(const char *filename) {
  FILE *f = fopen(filename, "rb");
  if (f == NULL)
    return NULL;
  fseek(f, 0, SEEK_END);

  long fsize = ftell(f);
  fseek(f, 0, SEEK_SET);

  if (fsize == 0) {
    fclose(f);
    return NULL;
  }
  char *string = malloc(fsize + 1);
  if (fread(string, fsize, 1, f) < 1) {
    printf("Error loading shader: %s", filename);
  }
  fclose(f);
  string[fsize] = 0;
  return string;
}

GLuint makeGlShader(const char *source, GLuint type) {
  GLint success;
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &source, NULL);
  glCompileShader(shader);
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char infoLog[ERROR_MESSAGE_MAX_LENGTH];
    glGetShaderInfoLog(shader, ERROR_MESSAGE_MAX_LENGTH, NULL, infoLog);
    switch (type) {
      case GL_VERTEX_SHADER: printf("Vertex shader compilation failed: %s\n", infoLog); break;
      case GL_FRAGMENT_SHADER: printf("Fragment shader compilation failed: %s\n", infoLog); break;
      default: printf("Type %i shader compilation failed: %s\n", type, infoLog);
    }
  }
  return shader;
}

void linkGlShaders(GLuint shaderProgram, GLuint vertexShader, GLuint fragmentShader) {
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  GLuint success;
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    char infoLog[ERROR_MESSAGE_MAX_LENGTH];
    glGetProgramInfoLog(shaderProgram, ERROR_MESSAGE_MAX_LENGTH, NULL, infoLog);
    printf("Shader program linking failed: %s\n", infoLog);
  }
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
}

GLint getGlUniformLocation(GLuint shaderProgram, const char* varname) {
  GLint uniform = glGetUniformLocation(shaderProgram, varname);
  char infoLog[ERROR_MESSAGE_MAX_LENGTH];
  glGetShaderInfoLog(shaderProgram, ERROR_MESSAGE_MAX_LENGTH, NULL, infoLog);
  if(uniform == GL_INVALID_VALUE || uniform == GL_INVALID_OPERATION || uniform == -1){
    printf("%s %s\nError (%i): %s\n", varname, "uniform could not be found!", uniform, infoLog);
    return -1;
  }
  return uniform;
}

void setGlUniform1f(GLuint shaderProgram, const char* varname, GLfloat value) {
  GLuint uniform = getGlUniformLocation(shaderProgram, varname);
  if(uniform != -1) glUniform1f(uniform, value);
}

void setGlUniform2f(GLuint shaderProgram, const char* varname, GLfloat float1, GLfloat float2) {
  GLuint uniform = getGlUniformLocation(shaderProgram, varname);
  if(uniform != -1) glUniform2f(uniform, float1, float2);
}

void setGlUniform1uiv(GLuint shaderProgram, const char* varname, GLuint size, GLuint* uints) {
  GLuint uniform = getGlUniformLocation(shaderProgram, varname);
  if(uniform != -1) glUniform1uiv(uniform, size, uints);
}

void setGlUniform4fv(GLuint shaderProgram, const char* varname, GLuint size, GLfloat* floats) {
  GLuint uniform = getGlUniformLocation(shaderProgram, varname);
  if(uniform != -1) glUniform4fv(uniform, size, floats);
}

void drawGlObject(struct GlObjectDataSet *ods) {
  GLenum error = GL_NO_ERROR;
  glBindVertexArray(ods->vao);
#if DEBUG
  if (error = glGetError() != GL_NO_ERROR)
    printGlError(error, 1);
#endif

  glBindBuffer(GL_ARRAY_BUFFER, ods->vbo);
#if DEBUG
  if (error = glGetError() != GL_NO_ERROR)
    printGlError(error, 2);
#endif

  glBufferSubData(GL_ARRAY_BUFFER, 0, ods->vertexDataBufferSize,ods->vertexDataBuffer);
#if DEBUG
  // printf("size: %zu\tData:\n", ods->vertexDataBufferSize);
  if (error = glGetError() != GL_NO_ERROR)
    printGlError(error, 3);
#endif

  if (ods->indexCount > 0) {
    glDrawElements(ods->primitiveType, ods->indexCount, GL_UNSIGNED_INT, 0);
#if DEBUG
    if (error = glGetError() != GL_NO_ERROR)
      printGlError(error, 4);
#endif
  } else {
    glDrawArrays(ods->primitiveType, 0, ods->vertexCount);
#if DEBUG
    if (error = glGetError() != GL_NO_ERROR)
      printGlError(error, 5);
#endif
  }
}

struct GlObjectDataSet initDefaultGlObject(void) {
  struct GlObjectDataSet ods;
  memset(&ods, 0, sizeof(struct GlObjectDataSet));
  return ods;
}

void deleteGlObject(struct GlObjectDataSet *ods) {
  free(ods->vertexDataBuffer);
  free(ods->vertexIndexBuffer);
  glDeleteVertexArrays(1, &ods->vao);
  glDeleteBuffers(1, &ods->vbo);
  memset(ods, 0, sizeof(struct GlObjectDataSet));
}

