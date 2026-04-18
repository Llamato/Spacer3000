#include "cbmcharmode.h"
#include "cbmchargen/cbmcharmode.h"
#include "common.h"

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
    if(fread(bytes, 8, 256, f) < 1){
        printf("Error loading chargen: %s\n", filename);
    }
    fclose(f);
    return bytes;
}

char asciiToPetscii(char asciiChar) {
    if(asciiChar >= 'A' && asciiChar <= 'Z'){
        return asciiChar - '@';
    }
    return asciiChar;
}

char* asciiStringToPetsciiString(char *asciiString) {
    unsigned int currentChar = 0;
    char* petsciiString = malloc((strlen(asciiString)+1) * sizeof(char));
    while(asciiString[currentChar] != '\0') {
        petsciiString[currentChar] = asciiToPetscii(asciiString[currentChar]);
        currentChar++;
    }
    petsciiString[currentChar] = '\0';
    return petsciiString;
}

char petsciiToScreencode(char petsciiChar) {
    char screencode;
    if(petsciiChar < (char) 0x20) {
        screencode = petsciiChar + 128;
    }else if(petsciiChar >= (char) 0x20 && petsciiChar < (char) 0x40) {
        screencode = petsciiChar;
    }else if(petsciiChar >= (char) 0x40 && petsciiChar < (char) 0x60) {
        screencode = petsciiChar - 64;
    }else if(petsciiChar >= (char) 0x60 && petsciiChar < (char) 0x80) {
        screencode = petsciiChar - 32;
    }else if(petsciiChar >= (char) 0x80 && petsciiChar < (char) 0xA0) {
        screencode = petsciiChar + 64;
    }else if(petsciiChar >= (char) 0xA0 && petsciiChar < (char) 0xC0) {
        screencode = petsciiChar - 64;
    }else if(petsciiChar >= (char) 0xC0 && petsciiChar < (char) 0xE0) {
        screencode = petsciiChar - 128;
    }else if(petsciiChar >= (char) 0xE0 && petsciiChar < (char) 0xFF) {
        screencode = petsciiChar - 128;
    }else if(petsciiChar == (char) 0xFF) {
        screencode = 0x5E;
    }
    return screencode;
}

char* petsciiStringToScreencodeString(char *petsciiString){
    unsigned int currentChar = 0;
    char* screencodeString = malloc((strlen(petsciiString)+1) * sizeof(char));
    while(petsciiString[currentChar] != '\0') {
        petsciiString[currentChar] = petsciiToScreencode(petsciiString[currentChar]);
        currentChar++;
    }
    return petsciiString;
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
    for(size_t currentBlock = 0; currentBlock < CBM_CHARGEN_SIZE; currentBlock++) {
        screen->chars[currentBlock+offset] = screen->chargen[currentBlock];
        screen->colors[currentBlock+offset] = currentBlock % CBM_COLOR_PALLET_SIZE;
    }
}

void writeStringToCbmScreen(struct cbmScreen* screen, struct Vector2 screenPosition, char* asciiString, uint8_t palletColor) {
    char* petsciiString = asciiStringToPetsciiString(asciiString);
    char* screencodeString = petsciiStringToScreencodeString(petsciiString);
    size_t stringStartIndex = screenPosition.y * CBM_SCREEN_COLUMNS + screenPosition.x;
    size_t currentPosition = 0;
    char screencodeChar;
    while((screencodeChar = asciiString[currentPosition])) {
        screen->chars[currentPosition] = screencodeString[currentPosition];
        screen->colors[currentPosition] = palletColor;
    }
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
    GLuint paddedChargenBytes[CBM_CHARGEN_SIZE];
    for(size_t currentByte = 0; currentByte < CBM_CHARGEN_SIZE; currentByte++) {
        paddedChargenBytes[currentByte] = screen->chargen[currentByte];
    }
    setGlUniform1uiv(screen->glData.shaderProgram, "chargen", CBM_CHARGEN_SIZE, paddedChargenBytes);
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