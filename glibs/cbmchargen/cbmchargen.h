#ifndef CBMCHARGEN_H
#define CBMCHARGEN_H
    #include <stddef.h>
    #include <stdlib.h>
    #include <stdio.h>
    #include <string.h>
    #include <unistd.h>
    #include "../common.h"
    #define CBM_CHARGEN_SIZE 2048
    #define CBM_CHAR_SIZE 8
    #define CBM_SCREEN_SIZE 1000
    #define CBM_COLOR_PALLET_SIZE 16
    struct cbmText {
        struct Vector2 position;
        float scale;

        char *cbmChargenBytes;
        char *petsciiString;
        char *screenCodeString;

        struct GlObjectDataSet glData;
    };
    static const struct Color c64colorPallet[] = {
  {0.0f, 0.0f, 0.0f, 1.0f}, //Black
  {.95f, .95f, .95f, 1.0f}, //White
  {(GLfloat)0x88/0xff, 0.0f, 0.0f, 1.0f}, //Red
  {(GLfloat)0xaa/0xff, 1.0f, (GLfloat)0xee/0xff, 1.0f}, //Tile
  {(GLfloat)0xcc/0xff, (GLfloat)0x44/0xff, (GLfloat)0xcc/0xff, 1.0f}, //Violet
  {0.0f, (GLfloat)0xcc/0xff, (GLfloat)0x55/0xff, 1.0f}, //Green
  {0.0f, 0.0f, (GLfloat)0xaa/0xff, 1.0f}, //Blue
  {(GLfloat)0xee/0xff, (GLfloat)0xee/0xff, (GLfloat)0x77/0xff, 1.0f}, //Yellow
  {(GLfloat)0xdd/0xff, (GLfloat)0x88/0xff, (GLfloat)0x55/0xff, 1.0f}, //Orange
  {(GLfloat)0x66/0xff, (GLfloat)0x88/0xff, (GLfloat)0x55/0xff, 1.0f}, //Brown
  {1.0f, (GLfloat)0x77/0xff, (GLfloat)0x77/0xff, 1.0f}, //Light red
  {(GLfloat)0x33/0xff, (GLfloat)0x33/0xff, (GLfloat)0x33/0xff, 1.0f}, //Gray
  {(GLfloat)0x77/0xff, (GLfloat)0x77/0xff, (GLfloat)0x77/0xff, 1.0f}, //Light gray
  {(GLfloat)0xaa/0xff, 1.0f, (GLfloat)0x66/0xff, 1.0f}, //Light green
  {0.0f, (GLfloat)0x88/0xff, 1.0f, 1.0f}, //Light blue
  {(GLfloat)0xbb/0xff, (GLfloat)0xbb/0xff, (GLfloat)0xbb/0xff, 1.0f} //Light gray
    };
    char* loadChargen(char* filename);
    char asciiToPetscii(char asciiChar);
    char petsciiToScreencode(char petsciiChar);
    char* petsciiStringToScreencodeString(char* petsciiString);
    char* asciiStringToPetsciiString(char *asciiString);
    void cbmBitmapFromChar(char* returnBucket, char* chargen, char asciiChar);
    char* cbmBitmapsFromString(char* chargen, char* asciiString);
    struct cbmText makeText(struct Vector2 position, struct Vector2 dimensions, char *cbmChargen, char *text, struct Color textColor, struct Color backgroundColor);
    void makeTextShaderObject(GLuint shaderProgram, struct GlObjectDataSet *vds);
    void drawText(struct GlObjectDataSet *vds, char* chargen, GLfloat width, GLfloat height);
#endif