#include "glad/glad.h"
#include "glad/khrplatform.h"
#include <GLFW/glfw3.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// unix specific
#include <unistd.h>

// cbmChargen
#include "cbmchargen/cbmchargen.h"

//Filepaths
#define DEFAULT_VERTEX_SHADER_FILENAME "shaders/default.vert"
#define DEFAULT_FRAGMENT_SHADER_FILENAME "shaders/default.frag"
#define PAD_VERTEX_SHADER_FILENAME "shaders/pad.vert"
#define PAD_FRAGMENT_SHADER_FILENAME "shaders/pad.frag"
#define TEXT_VERTEX_SHADER_FILENAME "shaders/cbmchar.vert"
#define TEXT_FRAGMENT_SHADER_FILENAME "shaders/cbmcharunpacked.frag"
#define CBM_CHARGEN_FILENAME "c64.bin"

// Debug!!!
#define DEBUG 0
#if DEBUG == 1
    #define DEBUG_MEMORY_ADITIVE 1024
#endif 
#if DEBUG == 0
    #define DEBUG_MEMORY_ADITIVE 0
#endif
#ifndef DEBUG
    #define DEBUG_MEMORY_ADITIVE 0
#endif

// OpenGL specific definitions
#define ERROR_MESSAGE_MAX_LENGTH 512

// Playfield
#define PLAYFIELD_WIDTH 1024
#define PLAYFIELD_HEIGHT 1024

// Camera Definitions
#define CAMERA_ZOOM_SPEED 1.0f
#define CAMERA_ZOOM_INITIAL 0.5f
#define CAMERA_ZOOM_MAX 1.0f
#define CAMERA_ZOOM_MIN 0.1f

// Key Map
#define INCREASE_THRUST_KEY GLFW_KEY_LEFT_SHIFT
#define DECREASE_THRUST_KEY GLFW_KEY_LEFT_CONTROL
#define MAX_THRUST_KEY GLFW_KEY_Z
#define ALT_MAX_THRUST_KEY GLFW_KEY_Y
#define KILL_THRUST_KEY GLFW_KEY_H
#define INCREASE_ZOOM_KEY GLFW_KEY_I
#define DECREASE_ZOOM_KEY GLFW_KEY_K

// Vector struct format
#define VECTOR_X 0
#define VECTOR_Y 1
#define VECTOR_Z 2

// Color struct format
#define COLOR_R 0
#define COLOR_G 1
#define COLOR_B 2
#define COLOR_A 3

// Vertex data format
#define FLOATS_IN_POINT 3
#define FLOATS_IN_COLOR 4

// Triangle
#define VERTS_IN_TRIANGLE 3
#define TRIANGLE_VERTEX_LEFT 0
#define TRIANGLE_VERTEX_MIDDLE 2
#define TRIANGLE_VERTEX_RIGHT 1

// Rectangle
#define VERTS_IN_RECTANGLE 4
#define BOTTOM_LEFT_VERTEX_INDEX 0
#define TOP_LEFT_VERTEX_INDEX 1
#define BOTTOM_RIGHT_VERTEX_INDEX 2
#define TOP_RIGHT_VERTEX_INDEX 3

// World definitions
#define GRAVITATIONAL_CONSTANT 0.8f
#define PHYSICS_TIME_DELTA 1.0f / 320.0f
#define WORLD_BACKGROUND_COLOR_R 0.0f
#define WORLD_BACKGROUND_COLOR_G 0.0f
#define WORLD_BACKGROUND_COLOR_B 0.0f

// Planet Definitions
#define PLANET_POLY_COUNT 64
#define PLANET_VERT_COUNT PLANET_POLY_COUNT + 2
#define PLANETN_FLOAT_COUNT PLANET_VERT_COUNT *(FLOATS_IN_POINT + FLOATS_IN_COLOR)
#define PLANET_POSITION_X 0.0f
#define PLANET_POSITION_Y -1.25f
#define PLANET_RADIUS 0.75f
#define PLANET_COLOR_R 0.5f
#define PLANET_COLOR_G 0.5f
#define PLANET_COLOR_B 1.0f
#define PLANET_MASS 20.0f
#define PLANET_COLLISION_TOLERANCE 0.01f

// Pad Definitions
#define DEFAULT_PAD_ANGLE M_PI / 2

// Ship Definitions
#define SHIP_ENGINE_MAX_THRUST 125.0f
#define SHIP_RCS_TOURGE 5.0f
#define SHIP_MASS 1.0f
#define SHIP_INITIAL_POSITION_X 0.0f
#define SHIP_INITIAL_POSITION_Y 2.0f
#define SHIP_INITIAL_VELOCITY_X 2.0f
#define SHIP_INITIAL_VELOCITY_Y 0.0f
#define SHIP_INITIAL_ACCELERATION_X 0.0f
#define SHIP_INITIAL_ACCELERATION_Y 0.0f
#define SHIP_INITIAL_ORIENTATION M_PI / 2
#define SHIP_INITIAL_THRUST 0.0f
#define THRUST_TRIANGLE_BASE_WIDTH 0.1f
#define THRUST_TRIANGLE_TIP_EXTEND 0.1f
#define THRUST_TRIANGLE_COLOR_R 1.0f
#define THRUST_TRIANGLE_COLOR_G 0.0f
#define THRUST_TRIANGLE_COLOR_B 0.0f

struct GlObjectDataSet {
  // Data
  GLfloat *vertexDataBuffer;
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
  GLint primitiveType;
  GLuint shaderProgram;
};

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

struct Camera {
  struct Vector2 position;
  struct Vector2 fieldOfView;
  float zoom;
};

struct Spaceship {

  // Physical Data
  struct Vector2 position;
  struct Vector2 velocity;
  struct Vector2 acceleration;
  GLfloat thrust;
  GLfloat mass;
  float orientation;

  // Structural Data
  struct Color color;
  struct GlObjectDataSet bodyGlData;
  struct GlObjectDataSet thrustTriangleGlData;
};

struct Planet {

  // Physical Data
  struct Vector2 position;
  GLfloat radius;
  float mass;

  // Structural Data
  struct Color color;
  struct GlObjectDataSet glData;
};

struct Pad {
  float angle;
  struct Planet *parentPlanet;
  struct GlObjectDataSet glData;
};

struct cbmText {
  struct Vector2 position;
  float scale;

  char *cbmChargenBytes;
  char *petsciiString;
  char *screenCodeString;

  struct GlObjectDataSet glData;
};

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

// Debug functions
void debugFrame(struct Spaceship *playerShip) {
  printf("=== FRAME DEBUG ===\n");
  printf("Ship position: (%.3f, %.3f)\n", playerShip->position.x, playerShip->position.y);
  printf("Ship vertices:\n");
  for (int currentVertex = 0; currentVertex < VERTS_IN_TRIANGLE; currentVertex++) {
    printf(
        "V%d: (%.3f, %.3f, %.3f)\n", currentVertex,
        playerShip->bodyGlData.vertexDataBuffer[currentVertex * (FLOATS_IN_POINT + FLOATS_IN_COLOR) + VECTOR_X],
        playerShip->bodyGlData.vertexDataBuffer[currentVertex * (FLOATS_IN_POINT + FLOATS_IN_COLOR) + VECTOR_Y],
        playerShip->bodyGlData.vertexDataBuffer[currentVertex * (FLOATS_IN_POINT + FLOATS_IN_COLOR) + VECTOR_Z]
      );
  }
}

void printVertexArray(GLfloat *vertexDataArray, size_t vertexCount, unsigned int stride) {
  printf("x\ty\tz\n");
  for (int currentVertex = 0; currentVertex < vertexCount; currentVertex++) {
    printf("%i:\t%f\t%f\t%f\n", currentVertex, vertexDataArray[currentVertex * stride + VECTOR_X], vertexDataArray[currentVertex * stride + VECTOR_Y], vertexDataArray[currentVertex * stride + VECTOR_Z]);
  }
}

// IO functions
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
  char *string = malloc(fsize + 1 + DEBUG_MEMORY_ADITIVE);
  if (fread(string, fsize, 1, f) < 1) {
    printf("Error loading shader: %s", filename);
  }
  fclose(f);
  string[fsize] = 0;
  return string;
}


// Math functions
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
  GLfloat *combinedArray = malloc(combinedSize * sizeof(GLfloat) + DEBUG_MEMORY_ADITIVE);
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

GLfloat *getTrianglefanCircle(struct Vector2 center, GLfloat radius, GLint polyCount, struct Color color) {
  float rotAngle = M_PI * 2.0f / polyCount;
  GLfloat vertCount = polyCount + 2;
  GLfloat *circleData = malloc(vertCount * (FLOATS_IN_POINT + FLOATS_IN_COLOR) * sizeof(GLfloat) + DEBUG_MEMORY_ADITIVE);

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

struct GlObjectDataSet getRectangle(struct Vector2 center, struct Vector2 dimensions) {
  struct GlObjectDataSet rectangle;
  rectangle.vertexCount = VERTS_IN_RECTANGLE;
  rectangle.vertexDataBufferSize = rectangle.vertexCount * FLOATS_IN_POINT * sizeof(GLfloat);
  rectangle.vertexDataBuffer = malloc(rectangle.vertexDataBufferSize + DEBUG_MEMORY_ADITIVE);

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
  rectangle.vertexIndexBuffer = malloc(rectangle.indexCount * sizeof(GLuint)) + DEBUG_MEMORY_ADITIVE;
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
    rectangle.vertexDataBuffer = malloc(rectangle.vertexDataBufferSize + DEBUG_MEMORY_ADITIVE);

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
    rectangle.vertexIndexBuffer = malloc(rectangle.indexCount * sizeof(GLuint) + DEBUG_MEMORY_ADITIVE);
    rectangle.vertexIndexBuffer[0] = 0;
    rectangle.vertexIndexBuffer[1] = 1;
    rectangle.vertexIndexBuffer[2] = 2;
    rectangle.vertexIndexBuffer[3] = 1;
    rectangle.vertexIndexBuffer[4] = 3;
    rectangle.vertexIndexBuffer[5] = 2;
    rectangle.primitiveType = GL_TRIANGLES;
    
    return rectangle;
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

// Engine variables
float gameLoopStartTime = 0;
float gameLoopEndTime = 1;
float frameTime = 1;
float timeAccumulator = 0;

// Gamestate functions
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
  for (size_t currentVertexStartIndex = 0;
       currentVertexStartIndex < vertexCount * stride;
       currentVertexStartIndex += stride) {
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
  struct Vector2 *results = (struct Vector2 *)malloc(vertexCount * sizeof(struct Vector2) + DEBUG_MEMORY_ADITIVE);
  for (size_t currentVertex = 0; currentVertex < vertexCount; currentVertex++) {
    struct Vector2 currentPoint;
    currentPoint.x = glData[currentVertex * stride + VECTOR_X];
    currentPoint.y = glData[currentVertex * stride + VECTOR_Y];
    results[currentVertex] = currentPoint;
  }
  return results;
}

void resetTriangleVertices(GLfloat *vertexBufferData) {
  GLfloat defaultTriangleVertices[] = {
      // TODO: Remove this and make it dynamic somehow
      -0.25f,        -0.144f,       0.0f, 0x1f / 256.0f,
      0x67 / 256.0f, 0xe0 / 256.0f, 0xff, // bottom-left
      -0.25f,        0.144f,        0.0f, 0x1f / 256.0f,
      0x67 / 256.0f, 0xe0 / 256.0f, 0xff, // top-left
      0.25f,         0.0f,          0.0f, 0x1f / 256.0f,
      0x67 / 256.0f, 0xe0 / 256.0f, 0xff // tip-right
  };

  for (size_t currentVertex = 0; currentVertex < VERTS_IN_TRIANGLE; currentVertex++) {
    vertexBufferData[currentVertex * (FLOATS_IN_POINT + FLOATS_IN_COLOR) + VECTOR_X] = defaultTriangleVertices[currentVertex * (FLOATS_IN_POINT + FLOATS_IN_COLOR) + VECTOR_X];
    vertexBufferData[currentVertex * (FLOATS_IN_POINT + FLOATS_IN_COLOR) + VECTOR_Y] = defaultTriangleVertices[currentVertex * (FLOATS_IN_POINT + FLOATS_IN_COLOR) + VECTOR_Y];
    vertexBufferData[currentVertex * (FLOATS_IN_POINT + FLOATS_IN_COLOR) + VECTOR_Z] = defaultTriangleVertices[currentVertex * (FLOATS_IN_POINT + FLOATS_IN_COLOR) + VECTOR_Z];
    vertexBufferData[currentVertex * (FLOATS_IN_POINT + FLOATS_IN_COLOR) + FLOATS_IN_POINT + COLOR_R] = defaultTriangleVertices[currentVertex * (FLOATS_IN_POINT + FLOATS_IN_COLOR) + FLOATS_IN_POINT + COLOR_R];
    vertexBufferData[currentVertex * (FLOATS_IN_POINT + FLOATS_IN_COLOR) + FLOATS_IN_POINT + COLOR_G] = defaultTriangleVertices[currentVertex * (FLOATS_IN_POINT + FLOATS_IN_COLOR) + FLOATS_IN_POINT + COLOR_G];
    vertexBufferData[currentVertex * (FLOATS_IN_POINT + FLOATS_IN_COLOR) + FLOATS_IN_POINT + COLOR_B] = defaultTriangleVertices[currentVertex * (FLOATS_IN_POINT + FLOATS_IN_COLOR) + FLOATS_IN_POINT + COLOR_B];
    vertexBufferData[currentVertex * (FLOATS_IN_POINT + FLOATS_IN_COLOR) + FLOATS_IN_POINT + COLOR_A] = defaultTriangleVertices[currentVertex * (FLOATS_IN_POINT + FLOATS_IN_COLOR) + FLOATS_IN_POINT + COLOR_A];
  }
}

void setTriangleVertexColorsFromColor(GLfloat *vertexBufferData, struct Color color) {
  for (size_t currentVertex = 0; currentVertex < VERTS_IN_TRIANGLE; currentVertex++) {
    vertexBufferData[currentVertex * (FLOATS_IN_POINT + FLOATS_IN_COLOR) + FLOATS_IN_POINT + COLOR_R] = color.red;
    vertexBufferData[currentVertex * (FLOATS_IN_POINT + FLOATS_IN_COLOR) + FLOATS_IN_POINT + COLOR_G] = color.green;
    vertexBufferData[currentVertex * (FLOATS_IN_POINT + FLOATS_IN_COLOR) + FLOATS_IN_POINT + COLOR_B] = color.blue;
  }
}

void setTriangleVertexColorsFromColors(GLfloat *vertexBufferData, struct Color *colors) {
  for (size_t currentVertex = 0; currentVertex < VERTS_IN_TRIANGLE; currentVertex++) {
    vertexBufferData[currentVertex * (FLOATS_IN_POINT + FLOATS_IN_COLOR) + FLOATS_IN_POINT + COLOR_R] = colors[currentVertex].red;
    vertexBufferData[currentVertex * (FLOATS_IN_POINT + FLOATS_IN_COLOR) + FLOATS_IN_POINT + COLOR_G] = colors[currentVertex].green;
    vertexBufferData[currentVertex * (FLOATS_IN_POINT + FLOATS_IN_COLOR) + FLOATS_IN_POINT + COLOR_B] = colors[currentVertex].blue;
  }
}

GLfloat *getTriangleVertices(struct Vector2 position, GLfloat orientation) {
  GLfloat *vertexDataBuffer = malloc(VERTS_IN_TRIANGLE * (FLOATS_IN_POINT + FLOATS_IN_COLOR) * sizeof(GLfloat) + DEBUG_MEMORY_ADITIVE);
  resetTriangleVertices(vertexDataBuffer);
  rotateVertexArray(vertexDataBuffer, VERTS_IN_TRIANGLE, orientation,(FLOATS_IN_POINT + FLOATS_IN_COLOR));
  translateVertexArray(vertexDataBuffer, VERTS_IN_TRIANGLE, &position,(FLOATS_IN_POINT + FLOATS_IN_COLOR));
  return vertexDataBuffer;
}

struct GlObjectDataSet getTriangle(struct Vector2 center, GLfloat orientation) {
  struct GlObjectDataSet glData;
  glData.vertexCount = VERTS_IN_TRIANGLE;
  glData.vertexDataBufferSize = VERTS_IN_TRIANGLE * (FLOATS_IN_POINT + FLOATS_IN_COLOR) * sizeof(GLfloat);
  glData.vertexDataBuffer = getTriangleVertices(center, orientation);
  glData.primitiveType = GL_TRIANGLES;
  return glData;
}

void updateCamera(struct Camera *cam, struct Spaceship *ship, float deltaTime) {
  cam->position.x = ship->position.x;
  cam->position.y = ship->position.y;
}

void updateShipPosition(struct Spaceship *ship, double deltaTime) {
  ship->acceleration.x += ship->thrust / ship->mass * cosf(ship->orientation) * deltaTime;
  ship->acceleration.y += ship->thrust / ship->mass * sinf(ship->orientation) * deltaTime;
  ship->velocity.x += ship->acceleration.x * deltaTime;
  ship->velocity.y += ship->acceleration.y * deltaTime;
  ship->position.x += ship->velocity.x * deltaTime;
  ship->position.y += ship->velocity.y * deltaTime;
}

void updateShipOrientation(struct Spaceship *ship, GLfloat tourge, double deltaTime) {
  float newOrientation = fmod(ship->orientation + tourge * deltaTime, 2 * M_PI);
  struct Vector2 screenSpaceOrigin = {0, 0};
  ship->orientation = newOrientation;
}

void updateShipThrust(struct Spaceship *ship, GLfloat buttonForce, double deltaTime) {
  ship->thrust += buttonForce * deltaTime;
  ship->thrust = gclamp(ship->thrust, SHIP_ENGINE_MAX_THRUST, 0.0f);
}

void updateThrustTriangle(struct Spaceship *ship) {
  struct Vector2 baseCenter;
  baseCenter.x = (ship->bodyGlData.vertexDataBuffer[TRIANGLE_VERTEX_LEFT + VECTOR_X] + ship->bodyGlData.vertexDataBuffer[TRIANGLE_VERTEX_RIGHT * (FLOATS_IN_POINT + FLOATS_IN_COLOR) + VECTOR_X]) / 2.0f;
  baseCenter.y = (ship->bodyGlData.vertexDataBuffer[TRIANGLE_VERTEX_LEFT + VECTOR_Y] + ship->bodyGlData.vertexDataBuffer[TRIANGLE_VERTEX_RIGHT * (FLOATS_IN_POINT + FLOATS_IN_COLOR) +VECTOR_Y]) / 2.0f;

  struct Vector2 thrustDirection = getDirection(&ship->position, &baseCenter);
  normalize(&thrustDirection);
  GLfloat tipExtend =
      THRUST_TRIANGLE_TIP_EXTEND / SHIP_ENGINE_MAX_THRUST * ship->thrust;

  struct Vector2 triangleBaseDirection =
      getPerpendicularVector(thrustDirection);

  ship->thrustTriangleGlData.vertexDataBuffer[TRIANGLE_VERTEX_LEFT * (FLOATS_IN_POINT + FLOATS_IN_COLOR) + VECTOR_X] = baseCenter.x + triangleBaseDirection.x * THRUST_TRIANGLE_BASE_WIDTH;
  ship->thrustTriangleGlData.vertexDataBuffer[TRIANGLE_VERTEX_LEFT * (FLOATS_IN_POINT + FLOATS_IN_COLOR) + VECTOR_Y] = baseCenter.y + triangleBaseDirection.y * THRUST_TRIANGLE_BASE_WIDTH;
  ship->thrustTriangleGlData.vertexDataBuffer[TRIANGLE_VERTEX_LEFT * (FLOATS_IN_POINT + FLOATS_IN_COLOR) + VECTOR_Z] = 0.0f;
  ship->thrustTriangleGlData.vertexDataBuffer[TRIANGLE_VERTEX_RIGHT * (FLOATS_IN_POINT + FLOATS_IN_COLOR) + VECTOR_X] = baseCenter.x - triangleBaseDirection.x * THRUST_TRIANGLE_BASE_WIDTH;
  ship->thrustTriangleGlData.vertexDataBuffer[TRIANGLE_VERTEX_RIGHT * (FLOATS_IN_POINT + FLOATS_IN_COLOR) + VECTOR_Y] = baseCenter.y - triangleBaseDirection.y * THRUST_TRIANGLE_BASE_WIDTH;
  ship->thrustTriangleGlData.vertexDataBuffer[TRIANGLE_VERTEX_RIGHT * (FLOATS_IN_POINT + FLOATS_IN_COLOR) + VECTOR_Z] = 0.0f;
  ship->thrustTriangleGlData.vertexDataBuffer[TRIANGLE_VERTEX_MIDDLE * (FLOATS_IN_POINT + FLOATS_IN_COLOR) + VECTOR_X] = baseCenter.x + tipExtend * thrustDirection.x;
  ship->thrustTriangleGlData.vertexDataBuffer[TRIANGLE_VERTEX_MIDDLE * (FLOATS_IN_POINT + FLOATS_IN_COLOR) + VECTOR_Y] = baseCenter.y + tipExtend * thrustDirection.y;
  ship->thrustTriangleGlData.vertexDataBuffer[TRIANGLE_VERTEX_MIDDLE * (FLOATS_IN_POINT + FLOATS_IN_COLOR) + VECTOR_Z] = 0.0f;
}

void applyGravity(struct Planet *planet, struct Spaceship *ship, double deltaTime) {
  // Optimize equations
  struct Vector2 offset;
  offset.x = planet->position.x - ship->position.x;
  offset.y = planet->position.y - ship->position.y;
  GLfloat distance = getMagnitude(&offset);
  GLfloat fmagnitude = GRAVITATIONAL_CONSTANT * planet->mass * ship->mass / pow(distance, 2);
  struct Vector2 fdirection = getDirection(&ship->position, &planet->position);
  struct Vector2 force;
  force.x = fmagnitude * fdirection.x;
  force.y = fmagnitude * fdirection.y;
  struct Vector2 acceleration;
  acceleration.x = force.x / ship->mass;
  acceleration.y = force.y / ship->mass;
  ship->acceleration.x += acceleration.x;
  ship->acceleration.y += acceleration.y;
}

void applyShipPositionAndOrientation(struct Spaceship *ship) {
  resetTriangleVertices(ship->bodyGlData.vertexDataBuffer);
  rotateVertexArray(ship->bodyGlData.vertexDataBuffer, VERTS_IN_TRIANGLE,ship->orientation, (FLOATS_IN_POINT + FLOATS_IN_COLOR));
  translateVertexArray(ship->bodyGlData.vertexDataBuffer, VERTS_IN_TRIANGLE,&ship->position, (FLOATS_IN_POINT + FLOATS_IN_COLOR));
}

// OpenGL wrapper functions
void makeGlObject(struct GlObjectDataSet *vds) {
  GLenum error = GL_NO_ERROR;

  glGenVertexArrays(1, &vds->vao);
  if (error = glGetError() != GL_NO_ERROR)
    printGlError(error, 1);

  glGenBuffers(1, &vds->vbo);
  if (error = glGetError() != GL_NO_ERROR)
    printGlError(error, 2);

  glBindVertexArray(vds->vao);
  if (error = glGetError() != GL_NO_ERROR)
    printGlError(error, 3);

  glBindBuffer(GL_ARRAY_BUFFER, vds->vbo);
  if (error = glGetError() != GL_NO_ERROR)
    printGlError(error, 4);

  glBufferData(GL_ARRAY_BUFFER, vds->vertexDataBufferSize,vds->vertexDataBuffer, GL_DYNAMIC_DRAW);
  if (error = glGetError() != GL_NO_ERROR)
    printGlError(error, 5);

  if (vds->indexCount > 0) {
    glGenBuffers(1, &vds->ibo);
    if (error = glGetError() != GL_NO_ERROR)
      printGlError(error, 6);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vds->ibo);
    if (error = glGetError() != GL_NO_ERROR)
      printGlError(error, 7);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, vds->indexCount * sizeof(GLuint),vds->vertexIndexBuffer, GL_STATIC_DRAW);
    if (error = glGetError() != GL_NO_ERROR)
      printGlError(error, 8);
  }
}

void makeDefaultShaderObject(struct GlObjectDataSet *vds) {
  makeGlObject(vds);
  glVertexAttribPointer(0, FLOATS_IN_POINT, GL_FLOAT, GL_FALSE,(FLOATS_IN_POINT + FLOATS_IN_COLOR) * sizeof(GLfloat),(void *)0);
  glVertexAttribPointer(1, FLOATS_IN_COLOR, GL_FLOAT, GL_TRUE,(FLOATS_IN_POINT + FLOATS_IN_COLOR) * sizeof(GLfloat),(void *)(FLOATS_IN_POINT * sizeof(GLfloat)));
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
}

void makePadShaderObject(struct GlObjectDataSet *vds) {
  makeGlObject(vds);
  glVertexAttribPointer(0, FLOATS_IN_POINT, GL_FLOAT, GL_FALSE,FLOATS_IN_POINT * sizeof(GLfloat), (void *)0);
  glEnableVertexAttribArray(0);
}

void makeTextShaderObject(struct GlObjectDataSet *vds) {
    makeGlObject(vds);
  
  const size_t floatsInVertex = FLOATS_IN_POINT + 2 * FLOATS_IN_COLOR;
  
  // Position attribute (location 0)
  glVertexAttribPointer(0, FLOATS_IN_POINT, GL_FLOAT, GL_FALSE, floatsInVertex * sizeof(GLfloat), (void *)0);
  glEnableVertexAttribArray(0);
  
  // Text color attribute (location 1)
  glVertexAttribPointer(1, FLOATS_IN_COLOR, GL_FLOAT, GL_TRUE, floatsInVertex * sizeof(GLfloat), (void *)(FLOATS_IN_POINT * sizeof(GLfloat)));
  glEnableVertexAttribArray(1);
  
  // Background color attribute (location 2)
  glVertexAttribPointer(2, FLOATS_IN_COLOR, GL_FLOAT, GL_TRUE, floatsInVertex * sizeof(GLfloat), (void *)((FLOATS_IN_POINT + FLOATS_IN_COLOR) * sizeof(GLfloat)));
  glEnableVertexAttribArray(2);
}

GLuint makeGlShader(const char *source, GLuint type) {
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &source, NULL);
  glCompileShader(shader);
  GLint success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char infoLog[ERROR_MESSAGE_MAX_LENGTH];
    glGetShaderInfoLog(shader, ERROR_MESSAGE_MAX_LENGTH, NULL, infoLog);
    printf("Vertex shader compilation failed: %s\n", infoLog);
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

// Event handlers
int currentWindowWidth = PLAYFIELD_WIDTH;
int currentWindowHeight = PLAYFIELD_HEIGHT;
void windowResizeCallback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
  currentWindowWidth = width;
  currentWindowHeight = height;
}

int windowIsFocused = 1;
void windowFocusCallback(GLFWwindow *window, int focused) {
  windowIsFocused = focused;
}

// Object instance management
struct Planet makePlanet(struct Vector2 location, GLfloat radius, float mass, struct Color color) {
  struct Planet planet;
  planet.radius = radius;
  planet.position = location;
  planet.mass = mass;
  planet.color = color;
  planet.glData = initDefaultGlObject();
  planet.glData.primitiveType = GL_TRIANGLE_FAN;
  planet.glData.vertexCount = (PLANET_POLY_COUNT + 2);
  planet.glData.vertexDataBufferSize = planet.glData.vertexCount * (FLOATS_IN_POINT + FLOATS_IN_COLOR) * sizeof(GLfloat);
  planet.glData.vertexDataBuffer = getTrianglefanCircle(location, radius, PLANET_POLY_COUNT, color);
  return planet;
}

struct Spaceship makeShip(struct Vector2 position, float orientation,
                          struct Vector2 velocity, struct Color color) {
  struct Spaceship ship;
  ship.position = position;
  ship.orientation = orientation;
  ship.velocity = velocity;
  ship.color = color;
  ship.mass = SHIP_MASS;
  ship.acceleration.x = SHIP_INITIAL_ACCELERATION_X;
  ship.acceleration.y = SHIP_INITIAL_ACCELERATION_Y;
  ship.thrust = SHIP_INITIAL_THRUST;
  ship.bodyGlData = getTriangle(ship.position, ship.orientation);
  setTriangleVertexColorsFromColor(ship.bodyGlData.vertexDataBuffer,ship.color);
  ship.thrustTriangleGlData = getTriangle(ship.position, ship.orientation + M_PI);
  struct Color thrustTriangleBaseColor = {THRUST_TRIANGLE_COLOR_R,THRUST_TRIANGLE_COLOR_G,THRUST_TRIANGLE_COLOR_B};
  struct Color thrustTriangleTipColor = {THRUST_TRIANGLE_COLOR_R, THRUST_TRIANGLE_COLOR_G + 0.5f, THRUST_TRIANGLE_COLOR_B + 0.5f};
  struct Color colors[] = {thrustTriangleBaseColor, thrustTriangleBaseColor,thrustTriangleTipColor};
  setTriangleVertexColorsFromColors(ship.thrustTriangleGlData.vertexDataBuffer,colors);
  return ship;
}

struct Pad makePad(struct Planet *parentPlanet, float angle) {
  struct Pad pad;
  pad.parentPlanet = parentPlanet;
  pad.angle = angle;
  struct Vector2 origin = {0, 0};
  struct Vector2 dimensions = {parentPlanet->radius / 10,parentPlanet->radius / 1.667};
  struct Vector2 polarPosition = {parentPlanet->radius, angle};
  pad.glData = getRectangle(origin, dimensions);
  rotateVertexArray(pad.glData.vertexDataBuffer, pad.glData.vertexCount,pad.angle, FLOATS_IN_POINT);
  struct Vector2 translationVector = {parentPlanet->position.x, parentPlanet->position.y};
  struct Vector2 planetRadientVector = {parentPlanet->radius * cosf(angle), parentPlanet->radius * sinf(angle)};
  translationVector.x += planetRadientVector.x;
  translationVector.y += planetRadientVector.y;
  translateVertexArray(pad.glData.vertexDataBuffer, VERTS_IN_RECTANGLE,&translationVector, FLOATS_IN_POINT);
  return pad;
}

struct cbmText makeText(struct Vector2 position, struct Vector2 dimensions, char *cbmChargen, char *text, struct Color textColor, struct Color backgroundColor) {
  struct cbmText cbmstr;

  /*cbmstr.position = position;
  cbmstr.scale = 1.0f;
  cbmstr.cbmBitmapBytes = cbmBitmapsFromString(cbmChargen, text);

  #if DEBUG == 1
    for(int currentByte = 0; currentByte < strlen(text); currentByte++){
      printf("%#08x ", cbmstr.cbmBitmapBytes[currentByte]);
    }
    printf("\n");
  #endif*/

  cbmstr.cbmChargenBytes = cbmChargen;
  cbmstr.petsciiString = asciiStringToPetsciiString(text); 
  cbmstr.screenCodeString = petsciiStringToScreencodeString(cbmstr.petsciiString);
  cbmstr.glData = getTextRectangle(position, dimensions, textColor, backgroundColor);
  return cbmstr;
}

_Bool isTriangleCollidingWithCircle(struct Spaceship *triangle, struct Planet *circle) {
  struct Vector2 *triangleVertices = getPointsFromGlData(triangle->bodyGlData.vertexDataBuffer, VERTS_IN_TRIANGLE,(FLOATS_IN_POINT + FLOATS_IN_COLOR));
  for (size_t currentVertex = 0; currentVertex < VERTS_IN_TRIANGLE; currentVertex++) {
    struct Vector2 wayVector = getVectorBetweenPoints(&triangleVertices[currentVertex], &circle->position);
    GLfloat distance = getMagnitude(&wayVector);
    if (distance < circle->radius - PLANET_COLLISION_TOLERANCE) {
      free(triangleVertices);
      return KHRONOS_TRUE;
    }
  }
  free(triangleVertices);
  return KHRONOS_FALSE;
}

_Bool isTriangleCollidingWithRectangle(struct Spaceship *triangle, struct Pad *rectangle) {
  _Bool result = KHRONOS_TRUE;
  struct Vector2 *triangleVertices = getPointsFromGlData(triangle->bodyGlData.vertexDataBuffer, VERTS_IN_TRIANGLE,(FLOATS_IN_POINT + FLOATS_IN_COLOR));
  struct Vector2 *rectangleVertices = getPointsFromGlData(rectangle->glData.vertexDataBuffer, VERTS_IN_RECTANGLE, FLOATS_IN_POINT);
  struct Vector2 normals[VERTS_IN_TRIANGLE + VERTS_IN_RECTANGLE];
  for (size_t currentEdge = 0; currentEdge < VERTS_IN_TRIANGLE; currentEdge++) {
    struct Vector2 wayVector = getVectorBetweenPoints(
        &triangleVertices[currentEdge],
        &triangleVertices[(currentEdge + 1) % VERTS_IN_TRIANGLE]);
    normals[currentEdge] = getInwardFacingEdgeNormal(&wayVector);
  }
  for (size_t currentEdge = 0; currentEdge < VERTS_IN_RECTANGLE; currentEdge++) {
    struct Vector2 wayVector = getVectorBetweenPoints(&rectangleVertices[currentEdge], &rectangleVertices[(currentEdge + 1) % VERTS_IN_RECTANGLE]);
    normals[currentEdge + VERTS_IN_TRIANGLE] = getInwardFacingEdgeNormal(&wayVector);
  }
  for (size_t currentNormal = 0; currentNormal < VERTS_IN_RECTANGLE + VERTS_IN_TRIANGLE; currentNormal++) {
    GLfloat triangleMin = dotProduct(&triangleVertices[0], &normals[currentNormal]);
    GLfloat triangleMax = triangleMin;
    for (size_t currentVertex = 1; currentVertex < VERTS_IN_TRIANGLE; currentVertex++) {
      GLfloat triangleCurrent = dotProduct(&triangleVertices[currentVertex], &normals[currentNormal]);
      if (triangleMax < triangleCurrent) {
        triangleMax = triangleCurrent;
      } else if (triangleMin > triangleCurrent) {
        triangleMin = triangleCurrent;
      }
    }
    GLfloat rectangleMin = dotProduct(&rectangleVertices[0], &normals[currentNormal]);
    GLfloat rectangleMax = rectangleMin;
    for (size_t currentVertex = 1; currentVertex < VERTS_IN_RECTANGLE; currentVertex++) {
      GLfloat rectangleCurrent = dotProduct(&rectangleVertices[currentVertex],&normals[currentNormal]);
      if (rectangleMax < rectangleCurrent) {
        rectangleMax = rectangleCurrent;
      } else if (rectangleMin > rectangleCurrent) {
        rectangleMin = rectangleCurrent;
      }
    }
    if (triangleMax < rectangleMin || rectangleMax < triangleMin) {
      result = KHRONOS_FALSE;
      break;
    }
  }
  free(triangleVertices);
  free(rectangleVertices);
  return result;
}

// Game state variables
_Bool gameover = KHRONOS_FALSE;
int main(int argc, char *argv[]) {
  int glfwstatus = glfwInit();
  if (!glfwstatus) {
    printf("%s\n", "Failed to init glfw");
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow(PLAYFIELD_WIDTH, PLAYFIELD_HEIGHT, "Spacer3000", NULL, NULL);
  glfwSetWindowSizeCallback(window, windowResizeCallback);
  glfwSetWindowFocusCallback(window, windowFocusCallback);

  if (window == NULL) {
    printf("%s\n", "Failed to create GLFW window");
  }

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    printf("Failed to initialize GLAD with GLFW loader\n");
    return -1;
  }

  glViewport(0, 0, PLAYFIELD_WIDTH, PLAYFIELD_HEIGHT);

  // Camera
  struct Camera camera;
  struct Vector2 cameraPosition;
  cameraPosition.x = 0;
  cameraPosition.y = 0;
  camera.position = cameraPosition;
  camera.zoom = CAMERA_ZOOM_INITIAL;

  struct Vector2 paleBlueDotPosition = {PLANET_POSITION_X, PLANET_POSITION_Y};
  struct Color paleBlueColor = {PLANET_COLOR_R, PLANET_COLOR_G, PLANET_COLOR_B};
  struct Planet paleBlueDot = makePlanet(paleBlueDotPosition, PLANET_RADIUS,PLANET_MASS, paleBlueColor);
  struct Pad cssc = makePad(&paleBlueDot, DEFAULT_PAD_ANGLE);

  // Ship
  struct Vector2 initialPlayerShipPosition = {SHIP_INITIAL_POSITION_X, SHIP_INITIAL_POSITION_Y};
  struct Vector2 initialPlayerShipVelocity = {SHIP_INITIAL_VELOCITY_X, SHIP_INITIAL_VELOCITY_Y};
  struct Color playerShipColor = {0x1f / 256.0f, 0x67 / 256.0f, 0xe0 / 256.0f};
  struct Spaceship playerShip = makeShip(initialPlayerShipPosition, SHIP_INITIAL_ORIENTATION, initialPlayerShipVelocity, playerShipColor);

  // Setup default shader and assign to objects
  const char *defaultVertexShaderSource = readShaderFile(DEFAULT_VERTEX_SHADER_FILENAME);
  GLuint defaultVertexShader = makeGlShader(defaultVertexShaderSource, GL_VERTEX_SHADER);
  const char *defaultFragmentShaderSource = readShaderFile(DEFAULT_FRAGMENT_SHADER_FILENAME);
  GLuint defaultFragmentShader = makeGlShader(defaultFragmentShaderSource, GL_FRAGMENT_SHADER);
  GLuint defaultShaderProgram = glCreateProgram();
  linkGlShaders(defaultShaderProgram, defaultVertexShader,defaultFragmentShader);
  makeDefaultShaderObject(&playerShip.bodyGlData);
  makeDefaultShaderObject(&playerShip.thrustTriangleGlData);
  makeDefaultShaderObject(&paleBlueDot.glData);

  // Setup pad shader and assign to objects
  const char *padVertexShaderSource = readShaderFile("shaders/pad.vert");
  GLuint padVertexShader = makeGlShader(padVertexShaderSource, GL_VERTEX_SHADER);
  const char *padFragmentShaderSource = readShaderFile("shaders/pad.frag");
  GLuint padFragmentShader = makeGlShader(padFragmentShaderSource, GL_FRAGMENT_SHADER);
  GLuint padShaderProgram = glCreateProgram();
  linkGlShaders(padShaderProgram, padVertexShader, padFragmentShader);
  makePadShaderObject(&cssc.glData);

  // Make gameover screen
  struct Vector2 gameoverTextPosition = {-0.0f, -0.0f};
  struct Vector2 gameoverTextDimensions = {1,1};
  char* c64chargen = loadChargen(CBM_CHARGEN_FILENAME);
  struct Color textColor = {1.0f, 1.0f, 1.0f, 1.0f};
  struct Color backgroundColor = {0.0f, 1.0f, 0.0f, 1.0f};
  struct cbmText gameoverText = makeText(gameoverTextPosition, gameoverTextDimensions, c64chargen,"You crashed!", textColor, backgroundColor);

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  const char *textVertexShaderSource = readShaderFile(TEXT_VERTEX_SHADER_FILENAME);
  GLuint textVertexShader = makeGlShader(textVertexShaderSource, GL_VERTEX_SHADER);
  const char *textFragmentShaderSource = readShaderFile(TEXT_FRAGMENT_SHADER_FILENAME);
  GLuint textFragmentShader = makeGlShader(textFragmentShaderSource, GL_FRAGMENT_SHADER);
  GLuint textShaderProgram = glCreateProgram();
  linkGlShaders(textShaderProgram, textVertexShader, textFragmentShader);
  makeTextShaderObject(&gameoverText.glData);

  // Unbind the buffers after use
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  // Clear screen then enter game loop
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  glfwSwapBuffers(window);

  while (!glfwWindowShouldClose(window)) {
    if (!windowIsFocused) {
      glfwPollEvents();
      continue;
    }

    gameLoopStartTime = glfwGetTime(); // Keep Time

    // Clear screen
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Set default shader parameters
    glUseProgram(defaultShaderProgram);
    GLuint cameraPositionDefaultShaderPtr = glGetUniformLocation(defaultShaderProgram, "cameraPos");
    glUniform2f(cameraPositionDefaultShaderPtr, camera.position.x,camera.position.y);
    GLuint screenSizeDefaultShaderPtr = glGetUniformLocation(defaultShaderProgram, "screenSize");
    glUniform2f(screenSizeDefaultShaderPtr, currentWindowWidth,currentWindowHeight);
    GLuint zoomDefaultShaderPtr = glGetUniformLocation(defaultShaderProgram, "zoom");
    glUniform1f(zoomDefaultShaderPtr, camera.zoom);

    // Draw objects using default shaders
    drawGlObject(&playerShip.bodyGlData);
    drawGlObject(&playerShip.thrustTriangleGlData);
    drawGlObject(&paleBlueDot.glData);

    // Set pad shader parameters
    glUseProgram(padShaderProgram);
    GLuint cameraPositionPadShaderPtr = glGetUniformLocation(padShaderProgram, "cameraPos");
    glUniform2f(cameraPositionPadShaderPtr, camera.position.x,camera.position.y);
    GLuint screenSizePadShaderPtr = glGetUniformLocation(padShaderProgram, "screenSize");
    glUniform2f(screenSizePadShaderPtr, currentWindowWidth,currentWindowHeight);
    GLuint zoomPadShaderPtr = glGetUniformLocation(padShaderProgram, "zoom");
    glUniform1f(zoomPadShaderPtr, camera.zoom);

    // Draw objects using pad shader
    drawGlObject(&cssc.glData);
    glfwSwapBuffers(window);
    glfwPollEvents();

    // Keep Time
    gameLoopEndTime = glfwGetTime();
    frameTime = gameLoopEndTime - gameLoopStartTime;
    timeAccumulator += frameTime;
    if (timeAccumulator > PHYSICS_TIME_DELTA) {

      // Do input handling here
      if (glfwGetKey(window, INCREASE_THRUST_KEY)) {
        updateShipThrust(&playerShip, SHIP_ENGINE_MAX_THRUST,PHYSICS_TIME_DELTA);
      } else if (glfwGetKey(window, DECREASE_THRUST_KEY)) {
        updateShipThrust(&playerShip, -SHIP_ENGINE_MAX_THRUST,PHYSICS_TIME_DELTA);
      } else if (glfwGetKey(window, MAX_THRUST_KEY) || glfwGetKey(window, ALT_MAX_THRUST_KEY)) {
        playerShip.thrust = SHIP_ENGINE_MAX_THRUST;
      } else if (glfwGetKey(window, KILL_THRUST_KEY)) {
        playerShip.thrust = 0;
      }

      if (glfwGetKey(window, INCREASE_ZOOM_KEY)) {
        camera.zoom += CAMERA_ZOOM_SPEED * timeAccumulator;
        camera.zoom = gclamp(camera.zoom, CAMERA_ZOOM_MAX, CAMERA_ZOOM_MIN);
      } else if (glfwGetKey(window, DECREASE_ZOOM_KEY)) {
        camera.zoom -= CAMERA_ZOOM_SPEED * PHYSICS_TIME_DELTA;
        camera.zoom = gclamp(camera.zoom, CAMERA_ZOOM_MAX, CAMERA_ZOOM_MIN);
      }

      updateShipPosition(&playerShip, timeAccumulator);
      if (glfwGetKey(window, GLFW_KEY_A)) {
        updateShipOrientation(&playerShip, SHIP_RCS_TOURGE, timeAccumulator);
      } else if (glfwGetKey(window, GLFW_KEY_D)) {
        updateShipOrientation(&playerShip, -SHIP_RCS_TOURGE, timeAccumulator);
      }

      // Do physics here
      playerShip.acceleration.x = 0.0f;
      playerShip.acceleration.y = 0.0f;
      applyShipPositionAndOrientation(&playerShip);
      if(isTriangleCollidingWithRectangle(&playerShip, &cssc)){
        while (!glfwWindowShouldClose(window)) {
          printf("%s\n", "landed!");
          glfwPollEvents();
        }

        // Make fuel bar
        // Refill fuel here
      }else if(isTriangleCollidingWithCircle(&playerShip, &paleBlueDot)){
        printf("Game Over! Showing screen...\n");
        uint8_t currentCharacterIndex = 0;
        while(!glfwWindowShouldClose(window)){
          gameLoopEndTime = glfwGetTime();
          frameTime = gameLoopEndTime - gameLoopStartTime;
          timeAccumulator += frameTime;
          glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
          glClear(GL_COLOR_BUFFER_BIT);
          glUseProgram(textShaderProgram);
          GLuint resolutionUniform = glGetUniformLocation(textShaderProgram, "iResolution");
          if(resolutionUniform != -1){
            glUniform2f(resolutionUniform, (float)currentWindowWidth, (float)currentWindowHeight);
          }else{
            printf("%s\n", "iResolution uniform could not be found!");
          }

          GLuint chargenUniform = glGetUniformLocation(textShaderProgram, "chargen");
          if(chargenUniform != -1){
            GLuint intBytes[CBM_CHARGEN_SIZE];
            for(int currentByte = 0; currentByte < CBM_CHARGEN_SIZE; currentByte++){
              intBytes[currentByte] = c64chargen[currentByte];
            }
            glUniform1uiv(chargenUniform, CBM_CHARGEN_SIZE, intBytes);
          }else{
            printf("%s\n", "chargenUniform could not be found!");
          }

          GLuint petsciiCodeUniform = glGetUniformLocation(textShaderProgram, "petsciicode");
          if(petsciiCodeUniform != -1){
            glUniform1ui(petsciiCodeUniform, (GLuint)currentCharacterIndex);
            currentCharacterIndex++;
          }else{
            printf("%s\n", "petsciiCodeUniform could not be found!");
          }

          drawGlObject(&gameoverText.glData);
          glfwSwapBuffers(window);
          sleep(1);
          if(glfwGetKey(window, GLFW_KEY_SPACE)) {
              break;
          }
          glfwPollEvents();
        }
      }
      updateThrustTriangle(&playerShip);
      applyGravity(&paleBlueDot, &playerShip, timeAccumulator);
      updateCamera(&camera, &playerShip, frameTime);
      timeAccumulator = 0; // Keep time
    }
  }

  // Clean up shaders
  deleteGlObject(&playerShip.bodyGlData);
  deleteGlObject(&playerShip.thrustTriangleGlData);
  deleteGlObject(&paleBlueDot.glData);
  glDeleteProgram(defaultShaderProgram);
  deleteGlObject(&cssc.glData);
  glDeleteProgram(padShaderProgram);
  glfwDestroyWindow(window);
  glfwTerminate();
  return window == NULL;
}