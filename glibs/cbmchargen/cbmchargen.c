#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../common.h"
#include "cbmchargen.h"

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

void cbmBitmapFromChar(char* returnBucket, char* chargen, char petsciiChar) {
    memcpy(returnBucket, chargen + petsciiChar, CBM_CHAR_SIZE);
}

char* cbmBitmapsFromString(char* chargen, char* asciiString) {
    char* returnBuffer = malloc(CBM_MAX_STRING_LENGTH * sizeof(char) * CBM_CHAR_SIZE);
    for(size_t currentChar = 0; currentChar < CBM_MAX_STRING_LENGTH; currentChar++){
        cbmBitmapFromChar(currentChar * CBM_CHAR_SIZE + returnBuffer, chargen + CBM_CHAR_SIZE * currentChar, asciiToPetscii(asciiString[currentChar]));
    }
    return returnBuffer;
}

struct cbmText makeText(struct Vector2 position, struct Vector2 dimensions, char *cbmChargen, char *text, struct Color textColor, struct Color backgroundColor) {
  struct cbmText cbmstr;

  cbmstr.cbmChargenBytes = cbmChargen;
  cbmstr.petsciiString = asciiStringToPetsciiString(text); 
  cbmstr.screenCodeString = petsciiStringToScreencodeString(cbmstr.petsciiString);
  cbmstr.glData = getTextRectangle(position, dimensions, textColor, backgroundColor);
  return cbmstr;
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