#include "cbmcharmode.h"
#include "cbmchargen/cbmcharmode.h"
#include "common.h"
#include <stddef.h>

char* loadChargen(char* filename) {
    if(access(filename, F_OK) != 0){
        printf("Error : %s does not exist\n", filename);
        return 0;
    }
    FILE *f = fopen(filename, "rb");
    if(f == NULL) {
        printf("Error opening chargen: %s\n", filename);
        return (char*)1;
    }
    char *bytes = malloc(CBM_CHARGEN_SIZE);
    if(fread(bytes, CBM_CHAR_SIZE, CBM_CHARSETS_PER_CHARGEN*CBM_CHARSET_CHARCOUNT, f) < 1){
        printf("Error loading chargen: %s\n", filename);
    }
    fclose(f);
    return bytes;
}

unsigned char asciiToPetscii(unsigned char a) {
    if (a >= 'A' && a <= 'Z') return a + 128;
    if (a >= 'a' && a <= 'z') return a - 32;
    return a;
}


unsigned char* asciiStringToPetsciiString(char *asciiString) {
    unsigned int currentChar = 0;
    unsigned char* petsciiString = malloc((strlen(asciiString)+1) * sizeof(char));
    while(asciiString[currentChar] != '\0') {
        petsciiString[currentChar] = asciiToPetscii(asciiString[currentChar]);
        currentChar++;
    }
    petsciiString[currentChar] = '\0';
    return petsciiString;
}

unsigned char petsciiToGraphScreencode(unsigned char p) {
    if (p < 0x20)  return p + 128;
    if (p < 0x40)  return p;
    if (p < 0x60)  return p - 64;
    if (p < 0x80)  return p - 32;
    if (p < 0xA0)  return p + 64;
    if (p < 0xC0)  return p - 64;
    if (p < 0xFF)  return p - 128;
    return 0x5E; // Pi
}

unsigned char petsciiToCasedScreencode(unsigned char p) {
    if (p >= 65 && p <= 90) return p - 64;
    if (p >= 193 && p <= 218) return p - 128;
    if (p >= 32 && p <= 63) return p;
    if (p >= 96 && p <= 127) return p - 32;
    if (p >= 160 && p <= 192) return p - 64;
    if (p >= 224) return p - 128;
    return p;
}

unsigned char* petsciiStringToScreencodeString(unsigned char *petsciiString){
    unsigned int currentChar = 0;
    unsigned char* screencodeString = malloc((strlen((char*) petsciiString)+1) * sizeof(char));
    while(petsciiString[currentChar] != '\0') {
        screencodeString[currentChar] = petsciiToCasedScreencode(petsciiString[currentChar]);
        currentChar++;
    }
    screencodeString[currentChar] = '\0';
    return screencodeString;
}

uint8_t getClosestPalletColor(struct Color* pallet, struct Color color) {
    uint8_t palletColor = 0;
    float minimumDifference = 3.0f;
    for(size_t currentPalletColor = 0; currentPalletColor < CBM_COLOR_PALLET_SIZE; currentPalletColor++) {
        float redDifference = powf(pallet->red - color.red, 2.0f);
        float greenDifference = powf(pallet->green - color.green, 2.0f);
        float blueDifference = powf(pallet->blue - color.blue, 2.0f);
        float sumOfDifferences = redDifference + greenDifference + blueDifference;
        if(minimumDifference > sumOfDifferences){
            minimumDifference = sumOfDifferences;
            palletColor = currentPalletColor;
        }
    }
    return palletColor;
}

struct GlObjectDataSet getTextRectangle(struct Vector2 center, struct Vector2 dimensions) {
    const size_t floatsInVertex = FLOATS_IN_POINT;

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
    
    // Vertex indexing
    rectangle.indexCount = 2 * VERTS_IN_TRIANGLE;
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

struct cbmScreen makeCbmScreen(struct Vector2 position, struct Vector2 dimensions, char* chargen, const struct Color* colorPallet) {
    struct cbmScreen screen;
    screen.chargen = chargen;
    screen.colorPallet = colorPallet;
    screen.chars = malloc(CBM_SCREEN_SIZE);
    screen.colors = malloc(CBM_SCREEN_SIZE);
    screen.glData = getTextRectangle(position, dimensions);
    return screen;
}

void clearCbmScreen(struct cbmScreen* screen) {
    for(size_t currentBlock = 0; currentBlock < CBM_SCREEN_SIZE; currentBlock++) {
        screen->chars[currentBlock] = CBM_SCREENCODE_CLEAR_CHAR;
        screen->colors[currentBlock] = CBM_COLOR_WHITE;
    } 
}

void writeChargenToCbmScreen(struct cbmScreen* screen, size_t offset) {
    for(size_t currentChar = 0; currentChar < CBM_CHARSET_CHARCOUNT; currentChar++) {
        screen->chars[currentChar+offset] = currentChar;
        screen->colors[currentChar+offset] = CBM_COLOR_PINK;
    }
}

void writeStringToCbmScreen(struct cbmScreen* screen, struct cbmScreenPosition screenPosition, char* asciiString, uint8_t palletColor) {
    unsigned char* petsciiString = asciiStringToPetsciiString(asciiString);
    unsigned char* screencodeString = petsciiStringToScreencodeString(petsciiString);
    size_t stringStartIndex = screenPosition.row * CBM_SCREEN_COLUMNS + screenPosition.column;
    size_t currentPosition = 0;
    char currentScreencode = screencodeString[currentPosition];
    while(currentScreencode != '\0') {
        screen->chars[stringStartIndex+currentPosition] = currentScreencode;
        screen->colors[stringStartIndex+currentPosition] = palletColor;
        currentPosition++;
        currentScreencode = screencodeString[currentPosition];
    }
    free(petsciiString);
    free(screencodeString);
}

void makeTextShaderObject(GLuint shaderProgram, struct GlObjectDataSet *vds) {
    makeGlObject(vds);
    vds->shaderProgram = shaderProgram;

    // Position attribute (location 0)
    const size_t floatsInVertex = FLOATS_IN_POINT;
    glVertexAttribPointer(0, FLOATS_IN_POINT, GL_FLOAT, GL_FALSE, floatsInVertex * sizeof(GLfloat), (void *)0);
    glEnableVertexAttribArray(0);
}

void drawCbmScreen(struct cbmScreen* screen, GLfloat width, GLfloat height) { 
    glUseProgram(screen->glData.shaderProgram);
    setGlUniform2f(screen->glData.shaderProgram, "iResolution", width, height);
    GLuint paddedChargenBytes[CBM_CHARSET_SIZE];
    for(size_t currentByte = 0; currentByte < CBM_CHARSET_SIZE; currentByte++) {
        paddedChargenBytes[currentByte] = screen->chargen[currentByte];
    }
    setGlUniform1uiv(screen->glData.shaderProgram, "chargen", CBM_CHARSET_SIZE, paddedChargenBytes);
    GLuint paddedCharBytes[CBM_SCREEN_SIZE];
    for(size_t currentByte = 0; currentByte < CBM_SCREEN_SIZE; currentByte++){
        paddedCharBytes[currentByte] = screen->chars[currentByte];
    }
    setGlUniform4fv(screen->glData.shaderProgram, "colorPallet", CBM_COLOR_PALLET_SIZE, (GLfloat*) c64colorPallet);
    setGlUniform1uiv(screen->glData.shaderProgram, "screen", CBM_SCREEN_SIZE, paddedCharBytes);
    GLuint paddedColorBytes[CBM_SCREEN_SIZE];
    for(size_t currentByte = 0; currentByte < CBM_SCREEN_SIZE; currentByte++) {
        paddedColorBytes[currentByte] = screen->colors[currentByte];
    }
    setGlUniform1uiv(screen->glData.shaderProgram, "colors", CBM_SCREEN_SIZE, paddedColorBytes);
    drawGlObject(&screen->glData);
}