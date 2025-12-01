#include "glad/glad.h"
#include "glad/khrplatform.h"
#include <GLFW/glfw3.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

//unix specific
#include <unistd.h>


//OpenGL specific definitions
#define ERROR_MESSAGE_MAX_LENGTH 512

//Playfield
#define PLAYFIELD_WIDTH 1024
#define PLAYFIELD_HEIGHT 1024

//Camera Definitions
#define CAMERA_ZOOM_SPEED 1.0f
#define CAMERA_ZOOM_INITIAL 0.5f
#define CAMERA_ZOOM_MAX 1.0f
#define CAMERA_ZOOM_MIN 0.1f

//Key Map
#define INCREASE_THRUST_KEY GLFW_KEY_LEFT_SHIFT
#define DECREASE_THRUST_KEY GLFW_KEY_LEFT_CONTROL
#define MAX_THRUST_KEY GLFW_KEY_Z
#define ALT_MAX_THRUST_KEY GLFW_KEY_Y
#define KILL_THRUST_KEY GLFW_KEY_H
#define INCREASE_ZOOM_KEY GLFW_KEY_I
#define DECREASE_ZOOM_KEY GLFW_KEY_K

//Vertex data format
#define VECTOR_X 0
#define VECTOR_Y 1
#define VECTOR_Z 2
#define COLOR_R 3
#define COLOR_G 4
#define COLOR_B 5
#define FLOATS_IN_VERTEX 6

//Triangle
#define VERTS_IN_TRIANGLE 3
#define TRIANGLE_VERTEX_LEFT 0
#define TRIANGLE_VERTEX_MIDDLE 2
#define TRIANGLE_VERTEX_RIGHT 1

//Rectangle
#define FLOATS_IN_POINT 3
#define VERTS_IN_RECTANGLE 4

//World Definitions
#define GRAVITATIONAL_CONSTANT 0.8f
#define PHYSICS_TIME_DELTA 1.0f/320.0f
#define WORLD_BACKGROUND_COLOR_R 0.0f
#define WORLD_BACKGROUND_COLOR_G 0.0f
#define WORLD_BACKGROUND_COLOR_B 0.0f

//Planet Definitions
#define PLANET_POLY_COUNT 64
#define PLANET_VERT_COUNT PLANET_POLY_COUNT + 2
#define PLANETN_FLOAT_COUNT PLANET_VERT_COUNT * FLOATS_IN_VERTEX
#define PLANET_POSITION_X 0.0f
#define PLANET_POSITION_Y -1.25f
#define PLANET_RADIUS 0.75f
#define PLANET_COLOR_R 0.5f
#define PLANET_COLOR_G 0.5f
#define PLANET_COLOR_B 1.0f
#define PLANET_MASS 20.0f

//Pad Definitions
#define DEFAULT_PAD_ANGLE M_PI / 2

//Ship Definitions
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

struct GlObjectDataSet{
    //Data
    GLfloat* vertexDataBuffer;
    GLuint* vertexIndexBuffer;

    //VAO
    GLuint vao;

    //VBO
    GLuint vbo;
    size_t vertexCount;
    size_t vertexDataBufferSize;

    //IBO
    GLuint ibo;
    size_t indexCount; 

    //Draw settings
    GLint primitiveType;
    GLuint shaderProgram;
};

struct Vector2{
    GLfloat x;
    GLfloat y;
};

struct Color{
    GLfloat red;
    GLfloat green;
    GLfloat blue;
};

struct Camera{
    struct Vector2 position;
    struct Vector2 fieldOfView;
    float zoom;
};

struct Spaceship{
    //Physical Data
    struct Vector2 position;
    struct Vector2 velocity;
    struct Vector2 acceleration;
    GLfloat thrust;
    GLfloat mass;
    float orientation;

    //Structural Data
    struct Color color;
    struct GlObjectDataSet bodyGlData;
    struct GlObjectDataSet thrustTriangleGlData;
};

struct Planet{
    //Physical Data
    struct Vector2 position;
    GLfloat radius;
    float mass;

    //Structural Data
    struct Color color;
    struct GlObjectDataSet glData;
};

struct Pad{
    float angle;
    struct Planet *parentPlanet;
    struct GlObjectDataSet glData;
};

void printGlError(GLenum error, unsigned int step) {
    printf("OpenGL Error: %x in step %u\n", error, step);
}

void printVertexArray(GLfloat* vertexDataArray, size_t vertexCount, unsigned int stride){ //Debug!!!
    printf("x\ty\t,z\n");
    for(int currentVertex = 0; currentVertex < vertexCount; currentVertex++){
        printf(
            "%i:\t%f\t%f\t%f\n", 
            currentVertex, 
            vertexDataArray[currentVertex * stride+VECTOR_X], 
            vertexDataArray[currentVertex * stride+VECTOR_Y], 
            vertexDataArray[currentVertex * stride+VECTOR_Z]
        );
    }
}

char* readShaderFile(const char* filename){
    FILE *f = fopen(filename, "rb");
    if(f == NULL)
        return NULL;
    fseek(f, 0, SEEK_END);

    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    if(fsize == 0){
        fclose(f);
        return NULL;
    }
    char *string = malloc(fsize + 1);
    if(fread(string, fsize, 1, f) < 1){
        printf("Error loading shader: %s", filename);
    }
    fclose(f);
    string[fsize] = 0;
    return string;
}

GLfloat gabsf(GLfloat value){
    return value < 0.0f ? value * -1.0f : value;
}

GLfloat* combineVertexDataArrays(GLfloat* array1, size_t size1, GLfloat* array2, size_t size2){
    size_t combinedSize = size1 + size2;
    GLfloat* combinedArray = malloc(combinedSize * sizeof(GLfloat));
    for(size_t i = 0; i < size1; i++){
        combinedArray[i] = array1[i];
    }
    for(size_t i = 0; i < size2; i++){
        combinedArray[size1 + i] = array2[i];
    }
    return combinedArray;
}

void scaleVertexDataArray(GLfloat* dataArray, size_t vertexCount, GLfloat scale, unsigned int stride){
    for(size_t i = 0; i < vertexCount; i++){
        size_t baseIndex = i * stride;
        dataArray[baseIndex] *=  scale;
        dataArray[baseIndex + 1] *= scale;
        dataArray[baseIndex + 2] *= scale;
    }
}

struct Vector2 getTriangleMiddleFromVertexPositions(struct Vector2 vertex0Position, struct Vector2 vertex1Position, struct Vector2 vertex2Position){
    struct Vector2 middle;
    middle.x = (vertex0Position.x + vertex1Position.x + vertex2Position.x) / 3.0f;
    middle.y = (vertex0Position.y + vertex1Position.y + vertex2Position.y) / 3.0f;
    return middle;
}

GLfloat* getTrianglefanCircle(GLfloat centerX, GLfloat centerY, GLfloat radius, GLint polyCount, GLfloat colorR, GLfloat colorG, GLfloat colorB){
    float rotAngle = M_PI * 2.0f / polyCount;
    GLfloat vertCount = polyCount + 2;
    GLfloat* circleData = malloc(vertCount * FLOATS_IN_VERTEX * sizeof(GLfloat));

    circleData[0] = centerX;
    circleData[1] = centerY;
    circleData[2] = 0.0f;
    circleData[3] = colorR;
    circleData[4] = colorG;
    circleData[5] = colorB;

    for(unsigned int currentVertex = 1; currentVertex < vertCount; currentVertex++){
      GLfloat currentX = centerX + radius * cosf(rotAngle * currentVertex);
      GLfloat currentY = centerY + radius * sinf(rotAngle * currentVertex);
      size_t currentIndex = currentVertex * FLOATS_IN_VERTEX;
      circleData[currentIndex] = currentX;
      circleData[currentIndex+1] = currentY;
      circleData[currentIndex+2] = 0.0f;
      circleData[currentIndex+3] = colorR;
      circleData[currentIndex+4] = colorG;
      circleData[currentIndex+5] = colorB;
    }
    return circleData;
}

struct GlObjectDataSet getRectangle(struct Vector2 center, struct Vector2 dimensions){
    struct GlObjectDataSet rectangle;
    rectangle.vertexCount = VERTS_IN_RECTANGLE;
    rectangle.vertexDataBufferSize = rectangle.vertexCount * FLOATS_IN_POINT * sizeof(GLfloat);
    rectangle.vertexDataBuffer = malloc(rectangle.vertexDataBufferSize); 
    rectangle.vertexDataBuffer[0] = center.x - dimensions.x / 2;
    rectangle.vertexDataBuffer[1] = center.y - dimensions.y / 2;
    rectangle.vertexDataBuffer[2] = 0.0f;
    rectangle.vertexDataBuffer[3] = center.x - dimensions.x / 2;
    rectangle.vertexDataBuffer[4] = center.y + dimensions.y / 2;
    rectangle.vertexDataBuffer[5] = 0.0f;
    rectangle.vertexDataBuffer[6] = center.x + dimensions.x / 2;
    rectangle.vertexDataBuffer[7] = center.y - dimensions.y / 2;
    rectangle.vertexDataBuffer[8] = 0.0f;
    rectangle.vertexDataBuffer[9] = center.x + dimensions.x / 2;
    rectangle.vertexDataBuffer[10] = center.y + dimensions.y / 2;
    rectangle.vertexDataBuffer[11] = 0.0f;
    rectangle.indexCount = 6;
    rectangle.vertexIndexBuffer = malloc(rectangle.indexCount * sizeof(GLuint));
    rectangle.vertexIndexBuffer[0] = 0;  // bottom-left
    rectangle.vertexIndexBuffer[1] = 1;  // top-left  
    rectangle.vertexIndexBuffer[2] = 2;  // bottom-right
    rectangle.vertexIndexBuffer[3] = 1;  // top-left
    rectangle.vertexIndexBuffer[4] = 3;  // top-right
    rectangle.vertexIndexBuffer[5] = 2;  // bottom-right
    rectangle.primitiveType = GL_TRIANGLES;
    return rectangle;
}

GLfloat getMagnitude(struct Vector2 *vector){
    return sqrtf(pow(vector->x, 2) + pow(vector->y, 2));
}

void normalize(struct Vector2 *vector){
    GLfloat magnitude = getMagnitude(vector);
    if(magnitude > 0.00001f){
        vector->x /= magnitude;
        vector->y /= magnitude;
    }else{
        vector->x = 0.0f;
        vector->y = 0.0f;
    }
}

struct Vector2 getVectorBetweenPoints(struct Vector2 *from, struct Vector2 *to){
    struct Vector2 wayVector;
    wayVector.x = to->x - from->x;
    wayVector.y = to->y - from->y;
    return wayVector;
}

GLfloat getDistance(struct Vector2 *from, struct Vector2 *to){
    struct Vector2 wayVector = getVectorBetweenPoints(from, to);
    return gabsf(getMagnitude(&wayVector));
}

struct Vector2 getDirection(struct Vector2 *from, struct Vector2 *to){
    struct Vector2 direction;
    direction = getVectorBetweenPoints(from, to);
    normalize(&direction);
    return direction;
}

struct Vector2 getPerpendicularVector(struct Vector2 vector){
    struct Vector2 parallelVector;
    parallelVector.x = -vector.y;
    parallelVector.y = vector.x;
    return parallelVector;
}

//Engine variables
double gameLoopStartTime = 0;
double gameLoopEndTime = 1;
double frameTime = 1;
double timeAccumulator = 0;

//Gamestate functions
GLfloat gclamp(GLfloat value, GLfloat max, GLfloat min){
    if(value < min){
        value = min;
    }else if(value > max){
        value = max;
    }
    return value;
}

void translateVertexArray(GLfloat *vertexDataArray, size_t vertexCount, struct Vector2 *translationVector, unsigned int stride){
    for(size_t currentVertex = 0; currentVertex < vertexCount; currentVertex++){
        vertexDataArray[currentVertex * stride + VECTOR_X] += translationVector->x;
        vertexDataArray[currentVertex * stride + VECTOR_Y] += translationVector->y;
    }
}

void translateOrigin(GLfloat *vertexDataArray, size_t vertexCount, struct Vector2 *from, struct Vector2 *to, unsigned int stride){
    struct Vector2 offset;
    offset.x = from->x - to->x;
    offset.y = from->y - to->y;
    for(size_t currentVertexStartIndex = 0; currentVertexStartIndex < vertexCount * stride; currentVertexStartIndex += stride){
        vertexDataArray[currentVertexStartIndex + VECTOR_X] += offset.x;
        vertexDataArray[currentVertexStartIndex + VECTOR_Y] += offset.y;
    }
}

struct Vector2 rotateVector(struct Vector2 vector, float angle){
    struct Vector2 rotated;
    rotated.x = vector.x * cosf(angle) - vector.y * sinf(angle);
    rotated.y = vector.x * sinf(angle) + vector.y * cosf(angle);
    return rotated;
}

void rotateVertexArray(GLfloat* vertexArray, size_t vertexCount, float rotationAngle, unsigned int stride){
    for(size_t currentVertexStartIndex = 0; currentVertexStartIndex < vertexCount * stride; currentVertexStartIndex += stride){
        GLfloat newX = vertexArray[currentVertexStartIndex] * cosf(rotationAngle) - vertexArray[currentVertexStartIndex +1] * sinf(rotationAngle);
        GLfloat newY = vertexArray[currentVertexStartIndex] * sinf(rotationAngle) + vertexArray[currentVertexStartIndex +1] * cosf(rotationAngle); 
        vertexArray[currentVertexStartIndex] = newX;
        vertexArray[currentVertexStartIndex +1] = newY;
    }
}

void convertScreenSpaceToLocal(GLfloat* vertexArray, size_t vertexCount, unsigned int stride){
    GLfloat xSum = 0;
    GLfloat ySum = 0;
    for(size_t currentVertex = 0; currentVertex < vertexCount; currentVertex++){
        xSum += vertexArray[currentVertex * stride + VECTOR_X];
        ySum += vertexArray[currentVertex * stride + VECTOR_Y];
    }
    struct Vector2 localCenter;
    localCenter.x = xSum / vertexCount;
    localCenter.y = ySum / vertexCount;
    for(size_t currentVertex = 0; currentVertex < vertexCount; currentVertex++){
        vertexArray[currentVertex * stride + VECTOR_X] -= localCenter.x;
        vertexArray[currentVertex * stride + VECTOR_Y] -= localCenter.y;
    }
}

struct Vector2 convertPolarToCatesian(struct Vector2 polarVector){
    struct Vector2 catesianVector;
    catesianVector.x = polarVector.x * cosf(polarVector.y);
    catesianVector.y = polarVector.x * sinf(polarVector.y);
    return catesianVector;
}

void resetTriangleVertices(GLfloat* vertexDataArray){
    GLfloat defaultTriangleVertices[] = { //TODO: Remove this and make it dynamic somehow
        -0.25f, -0.144f, 0.0f, 0x1f/256.0f, 0x67/256.0f, 0xe0/256.0f,    // bottom-left
        -0.25f, 0.144f, 0.0f, 0x1f/256.0f, 0x67/256.0f, 0xe0/256.0f,   // top-left  
        0.25f, 0.0f, 0.0f, 0x1f/256.0f, 0x67/256.0f, 0xe0/256.0f,  // tip-right
    };

    for(size_t currentVertex = 0; currentVertex < VERTS_IN_TRIANGLE; currentVertex++){
        vertexDataArray[currentVertex * FLOATS_IN_VERTEX + VECTOR_X] = defaultTriangleVertices[currentVertex * FLOATS_IN_VERTEX + VECTOR_X];
        vertexDataArray[currentVertex * FLOATS_IN_VERTEX + VECTOR_Y] = defaultTriangleVertices[currentVertex * FLOATS_IN_VERTEX + VECTOR_Y];
        vertexDataArray[currentVertex * FLOATS_IN_VERTEX + VECTOR_Z] = defaultTriangleVertices[currentVertex * FLOATS_IN_VERTEX + VECTOR_Z];
        vertexDataArray[currentVertex * FLOATS_IN_VERTEX + COLOR_R] = defaultTriangleVertices[currentVertex * FLOATS_IN_VERTEX + COLOR_R];
        vertexDataArray[currentVertex * FLOATS_IN_VERTEX + COLOR_G] = defaultTriangleVertices[currentVertex * FLOATS_IN_VERTEX + COLOR_G];
        vertexDataArray[currentVertex * FLOATS_IN_VERTEX + COLOR_B] = defaultTriangleVertices[currentVertex * FLOATS_IN_VERTEX + COLOR_B];
    }
}

struct Color* getTriangleVertexColorsFromColor(struct Color color) {
    struct Color* vertexColors = malloc(VERTS_IN_TRIANGLE * sizeof(struct Color));
    for(size_t currentVertex = 0; currentVertex < VERTS_IN_TRIANGLE; currentVertex++) {
        vertexColors[currentVertex].red = color.red;
        vertexColors[currentVertex].green = color.green;
        vertexColors[currentVertex].blue = color.blue;
    }
    return vertexColors;
}

void setTriangleVertexColorsFromColor(GLfloat* vertexBufferData, struct Color color){
    for(size_t currentVertex = 0; currentVertex < VERTS_IN_TRIANGLE; currentVertex++){
        vertexBufferData[currentVertex * FLOATS_IN_VERTEX + COLOR_R] = color.red;
        vertexBufferData[currentVertex * FLOATS_IN_VERTEX + COLOR_G] = color.green;
        vertexBufferData[currentVertex * FLOATS_IN_VERTEX + COLOR_B] = color.blue;
    }
}

void setTriangleVertexColorsFromColors(GLfloat* vertexBufferData, struct Color* colors){
    for(size_t currentVertex = 0; currentVertex < VERTS_IN_TRIANGLE; currentVertex++) {
        vertexBufferData[currentVertex * FLOATS_IN_VERTEX + COLOR_R] = colors[currentVertex].red;
        vertexBufferData[currentVertex * FLOATS_IN_VERTEX + COLOR_G] = colors[currentVertex].green;
        vertexBufferData[currentVertex * FLOATS_IN_VERTEX + COLOR_B] = colors[currentVertex].blue;
    }
}

GLfloat* getTriangleVertices(struct Vector2 position, GLfloat orientation){
    GLfloat* vertexDataArray = malloc(VERTS_IN_TRIANGLE * FLOATS_IN_VERTEX * sizeof(GLfloat));
    resetTriangleVertices(vertexDataArray);
    rotateVertexArray(vertexDataArray, VERTS_IN_TRIANGLE, orientation, FLOATS_IN_VERTEX);
    translateVertexArray(vertexDataArray, VERTS_IN_TRIANGLE, &position, FLOATS_IN_VERTEX);
    return vertexDataArray;
}

struct GlObjectDataSet getTriangle(struct Vector2 center, GLfloat orientation) {
    struct GlObjectDataSet glData;
    glData.vertexCount = VERTS_IN_TRIANGLE;
    glData.vertexDataBufferSize = VERTS_IN_TRIANGLE * FLOATS_IN_VERTEX * sizeof(GLfloat);
    glData.vertexDataBuffer = getTriangleVertices(center, orientation);
    glData.primitiveType = GL_TRIANGLES;
    return glData;
}

void updateCamera(struct Camera *cam, struct Spaceship *ship, float deltaTime){
    cam->position.x = ship->position.x;
    cam->position.y = ship->position.y;
}

void updateShipPosition(struct Spaceship *ship, double deltaTime){
    ship->acceleration.x += ship->thrust / ship->mass * cosf(ship->orientation) * deltaTime;
    ship->acceleration.y += ship->thrust / ship->mass * sinf(ship->orientation) * deltaTime;
    ship->velocity.x += ship->acceleration.x * deltaTime;
    ship->velocity.y += ship->acceleration.y * deltaTime;
    ship->position.x += ship->velocity.x * deltaTime; 
    ship->position.y += ship->velocity.y * deltaTime;
}

void updateShipOrientation(struct Spaceship *ship, GLfloat tourge, double deltaTime){
    float newOrientation = fmod(ship->orientation + tourge * deltaTime, 2 * M_PI);
    struct Vector2 screenSpaceOrigin = {0,0};
    ship->orientation = newOrientation;
}

void updateShipThrust(struct Spaceship *ship, GLfloat buttonForce, double deltaTime){
    ship->thrust += buttonForce * deltaTime;
    ship->thrust = gclamp(ship->thrust, SHIP_ENGINE_MAX_THRUST, 0.0f);
}

void updateThrustTriangle(struct Spaceship *ship) {
    struct Vector2 baseCenter;
    baseCenter.x = (ship->bodyGlData.vertexDataBuffer[TRIANGLE_VERTEX_LEFT + VECTOR_X] + ship->bodyGlData.vertexDataBuffer[TRIANGLE_VERTEX_RIGHT * FLOATS_IN_VERTEX + VECTOR_X]) / 2.0f;
    baseCenter.y = (ship->bodyGlData.vertexDataBuffer[TRIANGLE_VERTEX_LEFT + VECTOR_Y] + ship->bodyGlData.vertexDataBuffer[TRIANGLE_VERTEX_RIGHT * FLOATS_IN_VERTEX + VECTOR_Y]) / 2.0f;
    
    struct Vector2 thrustDirection = getDirection(&ship->position, &baseCenter);
    normalize(&thrustDirection);
    GLfloat tipExtend = THRUST_TRIANGLE_TIP_EXTEND/SHIP_ENGINE_MAX_THRUST * ship->thrust;

    struct Vector2 triangleBaseDirection = getPerpendicularVector(thrustDirection);
    
    ship->thrustTriangleGlData.vertexDataBuffer[TRIANGLE_VERTEX_LEFT * FLOATS_IN_VERTEX + VECTOR_X] = baseCenter.x + triangleBaseDirection.x * THRUST_TRIANGLE_BASE_WIDTH;
    ship->thrustTriangleGlData.vertexDataBuffer[TRIANGLE_VERTEX_LEFT * FLOATS_IN_VERTEX + VECTOR_Y] = baseCenter.y + triangleBaseDirection.y * THRUST_TRIANGLE_BASE_WIDTH;
    ship->thrustTriangleGlData.vertexDataBuffer[TRIANGLE_VERTEX_LEFT * FLOATS_IN_VERTEX + VECTOR_Z] = 0.0f;
    
    ship->thrustTriangleGlData.vertexDataBuffer[TRIANGLE_VERTEX_RIGHT * FLOATS_IN_VERTEX + VECTOR_X] = baseCenter.x - triangleBaseDirection.x * THRUST_TRIANGLE_BASE_WIDTH;
    ship->thrustTriangleGlData.vertexDataBuffer[TRIANGLE_VERTEX_RIGHT * FLOATS_IN_VERTEX + VECTOR_Y] = baseCenter.y - triangleBaseDirection.y * THRUST_TRIANGLE_BASE_WIDTH;
    ship->thrustTriangleGlData.vertexDataBuffer[TRIANGLE_VERTEX_RIGHT * FLOATS_IN_VERTEX + VECTOR_Z] = 0.0f;

    ship->thrustTriangleGlData.vertexDataBuffer[TRIANGLE_VERTEX_MIDDLE * FLOATS_IN_VERTEX + VECTOR_X] = baseCenter.x + tipExtend * thrustDirection.x;
    ship->thrustTriangleGlData.vertexDataBuffer[TRIANGLE_VERTEX_MIDDLE * FLOATS_IN_VERTEX + VECTOR_Y] = baseCenter.y + tipExtend * thrustDirection.y;
    ship->thrustTriangleGlData.vertexDataBuffer[TRIANGLE_VERTEX_MIDDLE * FLOATS_IN_VERTEX + VECTOR_Z] = 0.0f;
}

void applyGravity(struct Planet *planet, struct Spaceship *ship, double deltaTime){
    //Optimize equations
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

void applyShipPositionAndOrientation(struct Spaceship *ship){
    resetTriangleVertices(ship->bodyGlData.vertexDataBuffer);
    rotateVertexArray(ship->bodyGlData.vertexDataBuffer, VERTS_IN_TRIANGLE, ship->orientation, FLOATS_IN_VERTEX);
    translateVertexArray(ship->bodyGlData.vertexDataBuffer, VERTS_IN_TRIANGLE, &ship->position, FLOATS_IN_VERTEX);
}

//OpenGL wrapper functions
void makeGlObject(struct GlObjectDataSet *vds){
    GLenum error = GL_NO_ERROR;

    glGenVertexArrays(1, &vds->vao);
    if(error = glGetError() != GL_NO_ERROR) printGlError(error, 1);

    glGenBuffers(1, &vds->vbo);
    if(error = glGetError() != GL_NO_ERROR) printGlError(error, 2);

    glBindVertexArray(vds->vao);
    if(error = glGetError() != GL_NO_ERROR) printGlError(error, 3);

    glBindBuffer(GL_ARRAY_BUFFER, vds->vbo);
    if(error = glGetError() != GL_NO_ERROR) printGlError(error,4);

    glBufferData(GL_ARRAY_BUFFER, vds->vertexDataBufferSize, vds->vertexDataBuffer, GL_DYNAMIC_DRAW);
    if(error = glGetError() != GL_NO_ERROR) printGlError(error, 5);

    if(vds->indexCount > 0){
        glGenBuffers(1, &vds->ibo);
        if(error = glGetError() != GL_NO_ERROR) printGlError(error, 6);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vds->ibo);
        if(error = glGetError() != GL_NO_ERROR) printGlError(error, 7);

        glBufferData(GL_ELEMENT_ARRAY_BUFFER, vds->indexCount * sizeof(GLuint), vds->vertexIndexBuffer, GL_STATIC_DRAW);
        if(error = glGetError() != GL_NO_ERROR) printGlError(error, 8);
    }
}

void makeDefaultShaderObject(struct GlObjectDataSet *vds){
    makeGlObject(vds);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 6 * sizeof(float), (void*) (3*sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
}

void makePadShaderObject(struct GlObjectDataSet *vds) {
    makeGlObject(vds);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);
}

GLuint makeGlShader(const char* source, GLuint type) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success){
        char infoLog[ERROR_MESSAGE_MAX_LENGTH];
        glGetShaderInfoLog(shader, ERROR_MESSAGE_MAX_LENGTH, NULL, infoLog);
        printf("Vertex shader compilation failed: %s\n", infoLog);
    }
    return shader;
}

void linkGlShaders(GLuint shaderProgram, GLuint vertexShader, GLuint fragmentShader){
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

void drawGlObject(struct GlObjectDataSet *ods){
    GLenum error = GL_NO_ERROR;
    glBindVertexArray(ods->vao);
    #if DEBUG
        if(error = glGetError() != GL_NO_ERROR) printGlError(error, 1);
    #endif

    glBindBuffer(GL_ARRAY_BUFFER, ods->vbo);
    #if DEBUG
        if(error = glGetError() != GL_NO_ERROR) printGlError(error, 2);
    #endif

    glBufferSubData(GL_ARRAY_BUFFER, 0, ods->vertexDataBufferSize, ods->vertexDataBuffer);
    #if DEBUG
        //printf("size: %zu\tData:\n", ods->vertexDataBufferSize);
        if(error = glGetError() != GL_NO_ERROR) printGlError(error, 3);
    #endif

    if(ods->indexCount > 0) {
        glDrawElements(ods->primitiveType, ods->indexCount, GL_UNSIGNED_INT, 0);
        #if DEBUG
            if(error = glGetError() != GL_NO_ERROR) printGlError(error, 4);
        #endif
    }else{
        glDrawArrays(ods->primitiveType, 0, ods->vertexCount);
        #if DEBUG
            if(error = glGetError() != GL_NO_ERROR) printGlError(error, 5);
        #endif
    }
}

struct GlObjectDataSet initDefaultGlObject(void){
    struct GlObjectDataSet ods;
    memset(&ods, 0, sizeof(struct GlObjectDataSet));
    return ods;
}

void deleteGlObject(struct GlObjectDataSet *ods){
    /*free(ods->vertexDataBuffer);
    free(ods->vertexIndexBuffer);*/
    glDeleteVertexArrays(1, &ods->vao);
    glDeleteBuffers(1, &ods->vbo);
    memset(ods, 0, sizeof(struct GlObjectDataSet));
}

//Event handlers
int currentWindowWidth = PLAYFIELD_WIDTH;
int currentWindowHeight = PLAYFIELD_HEIGHT;
void windowResizeCallback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
    currentWindowWidth = width;
    currentWindowHeight = height;
}

int windowIsFocused = 1;
void windowFocusCallback(GLFWwindow* window, int focused){
    windowIsFocused = focused;
}

//Object instance management
struct Planet makePlanet(struct Vector2 location, GLfloat radius, float mass, struct Color color){
    struct Planet planet;
    planet.radius = radius;
    planet.position = location;
    planet.mass = mass;
    planet.color = color;
    planet.glData = initDefaultGlObject();
    planet.glData.primitiveType = GL_TRIANGLE_FAN;
    planet.glData.vertexCount = (PLANET_POLY_COUNT + 2);
    planet.glData.vertexDataBufferSize = planet.glData.vertexCount * FLOATS_IN_VERTEX * sizeof(GLfloat);
    planet.glData.vertexDataBuffer = getTrianglefanCircle(location.x, location.y, radius, PLANET_POLY_COUNT, color.red, color.green, color.blue);
    return planet;
}

struct Spaceship makeShip(struct Vector2 position, float orientation, struct Vector2 velocity, struct Color color){
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
    setTriangleVertexColorsFromColor(ship.bodyGlData.vertexDataBuffer, ship.color);
    ship.thrustTriangleGlData = getTriangle(ship.position, ship.orientation + M_PI);
    struct Color thrustTriangleBaseColor = {THRUST_TRIANGLE_COLOR_R, THRUST_TRIANGLE_COLOR_G, THRUST_TRIANGLE_COLOR_B};
    struct Color thrustTriangleTipColor = { THRUST_TRIANGLE_COLOR_R, THRUST_TRIANGLE_COLOR_G + 0.5f, THRUST_TRIANGLE_COLOR_B + 0.5f};
    struct Color colors[] = {thrustTriangleBaseColor, thrustTriangleBaseColor, thrustTriangleTipColor};
    setTriangleVertexColorsFromColors(ship.thrustTriangleGlData.vertexDataBuffer, colors);
    return ship;
}

struct Pad makePad(struct Planet *parentPlanet, float angle){
    struct Pad pad; 
    pad.parentPlanet = parentPlanet;
    pad.angle = angle;
    struct Vector2 origin = {0, 0};
    struct Vector2 dimensions = {parentPlanet->radius / 10, parentPlanet->radius / 1.667};
    struct Vector2 polarPosition = {parentPlanet->radius, angle};
    pad.glData = getRectangle(origin, dimensions);
    rotateVertexArray(pad.glData.vertexDataBuffer, pad.glData.vertexCount, pad.angle, FLOATS_IN_POINT);
    struct Vector2 translationVector = {parentPlanet->position.x, parentPlanet->position.y};
    struct Vector2 planetRadientVector = {parentPlanet->radius * cosf(angle), parentPlanet->radius * sinf(angle)};
    translationVector.x += planetRadientVector.x;
    translationVector.y += planetRadientVector.y;
    translateVertexArray(pad.glData.vertexDataBuffer, VERTS_IN_RECTANGLE, &translationVector, FLOATS_IN_POINT);
    return pad;
}

//Debug functions
void debugFrame(struct Spaceship *playerShip){
    printf("=== FRAME DEBUG ===\n");
    printf("Ship position: (%.3f, %.3f)\n", playerShip->position.x, playerShip->position.y);
    printf("Ship vertices:\n");
    for(int currentVertex = 0; currentVertex < VERTS_IN_TRIANGLE; currentVertex++) {
        printf("  V%d: (%.3f, %.3f, %.3f)\n", currentVertex,
            playerShip->bodyGlData.vertexDataBuffer[currentVertex * FLOATS_IN_VERTEX + VECTOR_X],
            playerShip->bodyGlData.vertexDataBuffer[currentVertex * FLOATS_IN_VERTEX + VECTOR_Y],
            playerShip->bodyGlData.vertexDataBuffer[currentVertex * FLOATS_IN_VERTEX + VECTOR_Z]
        );
    }
}

//Game state variables
int main(int argc, char* argv[]){
    int glfwstatus = glfwInit();
    if(!glfwstatus){
        printf("%s\n", "Failed to init glfw");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow* window = glfwCreateWindow(PLAYFIELD_WIDTH, PLAYFIELD_HEIGHT, "Spacer3000", NULL, NULL);
    glfwSetWindowSizeCallback(window, windowResizeCallback);
    glfwSetWindowFocusCallback(window, windowFocusCallback);
    
    if(window == NULL){
        printf("%s\n", "Failed to create GLFW window");
    }
    glfwMakeContextCurrent(window);

   if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Failed to initialize GLAD with GLFW loader\n");
        return -1;
    }

    glViewport(0, 0, PLAYFIELD_WIDTH, PLAYFIELD_HEIGHT);

    //Camera
    struct Camera camera;
    struct Vector2 cameraPosition;
    cameraPosition.x = 0;
    cameraPosition.y = 0;
    camera.position = cameraPosition;
    camera.zoom = CAMERA_ZOOM_INITIAL;
    
    struct Vector2 paleBlueDotPosition = {PLANET_POSITION_X, PLANET_POSITION_Y};
    struct Color paleBlueColor = {PLANET_COLOR_R, PLANET_COLOR_G, PLANET_COLOR_B};
    struct Planet paleBlueDot = makePlanet(paleBlueDotPosition, PLANET_RADIUS, PLANET_MASS, paleBlueColor);
    struct Pad cssc = makePad(&paleBlueDot, DEFAULT_PAD_ANGLE);

    //Ship
    struct Vector2 initialPlayerShipPosition = {SHIP_INITIAL_POSITION_X, SHIP_INITIAL_POSITION_Y};
    struct Vector2 initialPlayerShipVelocity = {SHIP_INITIAL_VELOCITY_X, SHIP_INITIAL_VELOCITY_Y};
    struct Color playerShipColor = {0x1f/256.0f, 0x67/256.0f, 0xe0/256.0f};
    struct Spaceship playerShip = makeShip(initialPlayerShipPosition, SHIP_INITIAL_ORIENTATION, initialPlayerShipVelocity, playerShipColor);

    //Setup default shader and assign to objects
    const char* defaultVertexShaderSource = readShaderFile("shaders/default.vert");
    GLuint defaultVertexShader = makeGlShader(defaultVertexShaderSource, GL_VERTEX_SHADER);
    const char* defaultFragmentShaderSource = readShaderFile("shaders/default.frag");
    GLuint defaultFragmentShader = makeGlShader(defaultFragmentShaderSource, GL_FRAGMENT_SHADER);
    GLuint defaultShaderProgram = glCreateProgram();
    linkGlShaders(defaultShaderProgram, defaultVertexShader, defaultFragmentShader);
    makeDefaultShaderObject(&playerShip.bodyGlData);
    makeDefaultShaderObject(&playerShip.thrustTriangleGlData);
    makeDefaultShaderObject(&paleBlueDot.glData);
    
    //Setup pad shader and assign to objects
    const char* padVertexShaderSource = readShaderFile("shaders/pad.vert");
    GLuint padVertexShader = makeGlShader(padVertexShaderSource, GL_VERTEX_SHADER);
    const char* padFragmentShaderSource = readShaderFile("shaders/pad.frag");
    GLuint padFragmentShader = makeGlShader(padFragmentShaderSource,GL_FRAGMENT_SHADER);
    GLuint padShaderProgram = glCreateProgram();
    linkGlShaders(padShaderProgram, padVertexShader, padFragmentShader);
    makePadShaderObject(&cssc.glData);
    
    //Unbind the buffers after use
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //Clear screen then enter game loop
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window);

    while(!glfwWindowShouldClose(window)){
        if(!windowIsFocused){
            sleep(1);
            glfwPollEvents();
            continue;
        }

        gameLoopStartTime = glfwGetTime(); //Keep Time

        //Clear screen
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //Set default shader parameters
        glUseProgram(defaultShaderProgram);
        GLuint cameraPositionDefaultShaderPtr = glGetUniformLocation(defaultShaderProgram, "cameraPos");
        glUniform2f(cameraPositionDefaultShaderPtr, camera.position.x, camera.position.y);
        GLuint screenSizeDefaultShaderPtr = glGetUniformLocation(defaultShaderProgram, "screenSize");
        glUniform2f(screenSizeDefaultShaderPtr, currentWindowWidth, currentWindowHeight);
        GLuint zoomDefaultShaderPtr = glGetUniformLocation(defaultShaderProgram, "zoom");
        glUniform1f(zoomDefaultShaderPtr, camera.zoom);
        
        //Draw objects using default shaders
        drawGlObject(&playerShip.bodyGlData);
        drawGlObject(&playerShip.thrustTriangleGlData);
        drawGlObject(&paleBlueDot.glData);
        
        //Set pad shader parameters
        glUseProgram(padShaderProgram);
        GLuint cameraPositionPadShaderPtr = glGetUniformLocation(padShaderProgram, "cameraPos");
        glUniform2f(cameraPositionPadShaderPtr, camera.position.x, camera.position.y);
        GLuint screenSizePadShaderPtr = glGetUniformLocation(padShaderProgram, "screenSize");
        glUniform2f(screenSizePadShaderPtr, currentWindowWidth, currentWindowHeight);
        GLuint zoomPadShaderPtr = glGetUniformLocation(padShaderProgram, "zoom");
        glUniform1f(zoomPadShaderPtr, camera.zoom);

        //Draw objects using pad shader
        drawGlObject(&cssc.glData);
        glfwSwapBuffers(window);
        glfwPollEvents();

        //Keep Time
        gameLoopEndTime = glfwGetTime();
        frameTime = gameLoopEndTime - gameLoopStartTime;
        timeAccumulator += frameTime;
        if(timeAccumulator > PHYSICS_TIME_DELTA){
            //Do input handling here
            if(glfwGetKey(window, INCREASE_THRUST_KEY)){
                updateShipThrust(&playerShip, SHIP_ENGINE_MAX_THRUST, PHYSICS_TIME_DELTA);
            }else if(glfwGetKey(window, DECREASE_THRUST_KEY)){
                updateShipThrust(&playerShip, -SHIP_ENGINE_MAX_THRUST, PHYSICS_TIME_DELTA);
            }else if(glfwGetKey(window, MAX_THRUST_KEY) || glfwGetKey(window, ALT_MAX_THRUST_KEY)){
                playerShip.thrust = SHIP_ENGINE_MAX_THRUST;
            }else if(glfwGetKey(window, KILL_THRUST_KEY)){
                playerShip.thrust = 0;
            }

            if(glfwGetKey(window, INCREASE_ZOOM_KEY)){
                camera.zoom += CAMERA_ZOOM_SPEED * timeAccumulator;
                camera.zoom = gclamp(camera.zoom, CAMERA_ZOOM_MAX, CAMERA_ZOOM_MIN);
            }else if(glfwGetKey(window, DECREASE_ZOOM_KEY)){
                camera.zoom -= CAMERA_ZOOM_SPEED * PHYSICS_TIME_DELTA;
                camera.zoom = gclamp(camera.zoom, CAMERA_ZOOM_MAX, CAMERA_ZOOM_MIN);
            }

            updateShipPosition(&playerShip, timeAccumulator);
            if(glfwGetKey(window, GLFW_KEY_A)){
                updateShipOrientation(&playerShip, SHIP_RCS_TOURGE, timeAccumulator);
            }else if(glfwGetKey(window, GLFW_KEY_D)){
                updateShipOrientation(&playerShip, -SHIP_RCS_TOURGE, timeAccumulator);
            }

            //Do physics here
            playerShip.acceleration.x = 0.0f;
            playerShip.acceleration.y = 0.0f;
            applyShipPositionAndOrientation(&playerShip);
            updateThrustTriangle(&playerShip);
            applyGravity(&paleBlueDot, &playerShip, timeAccumulator);
            updateCamera(&camera, &playerShip, frameTime);
            timeAccumulator = 0; //Keep time
        }
    }

    //Clean up shaders
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