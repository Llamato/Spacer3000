#ifndef CBMCHARMODE_H
#define CBMCHARMODE_H
    #include <stddef.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <unistd.h>
    #include "../common.h"
    #define CBM_CHARSET_SIZE 2048
    #define CBM_CHARSETS_PER_CHARGEN 2
    #define CBM_CHARGEN_SIZE CBM_CHARSETS_PER_CHARGEN*CBM_CHARSET_SIZE
    #define CBM_CHAR_SIZE 8
    #define CBM_CHARSET_CHARCOUNT CBM_CHARSET_SIZE / CBM_CHAR_SIZE
    #define CBM_SCREEN_COLUMNS 40
    #define CBM_SCREEN_ROWS 25
    #define CBM_SCREEN_SIZE CBM_SCREEN_COLUMNS*CBM_SCREEN_ROWS
    #define CBM_COLOR_PALLET_SIZE 16
    #define CBM_COLOR_BLACK 0
    #define CBM_COLOR_WHITE 1
    #define CBM_COLOR_RED 2
    #define CBM_COLOR_PINK 4
    #define CBM_SCREENCODE_CLEAR_CHAR 32
    struct cbmScreenPosition {
      uint16_t column;
      uint16_t row; 
    };
    struct cbmScreen {
        struct Vector2 position;
        struct Vector2 dimensions;

        unsigned char* chargen;
        const struct Color* colorPallet;
        
        unsigned char* chars;
        unsigned char* colors;

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
    unsigned char asciiToPetscii(unsigned char asciiChar);
    unsigned char* asciiStringToPetsciiString(char *asciiString);
    unsigned char petsciiToGraphScreencode(unsigned char p);
    unsigned char petsciiToCasedScreencode(unsigned char p);
    unsigned char* petsciiStringToScreencodeString(unsigned char* petsciiString);
    uint8_t getClosestPalletColor(struct Color* pallet, struct Color color);
    struct GlObjectDataSet getTextRectangle(struct Vector2 center, struct Vector2 dimensions);
    struct cbmScreen makeCbmScreen(struct Vector2 position, struct Vector2 dimensions, char* chargen, const struct Color* colorPallet);
    void clearCbmScreen(struct cbmScreen* screen);
    void writeChargenToCbmScreen(struct cbmScreen* screen, size_t offset);
    void writeStringToCbmScreen(struct cbmScreen* screen, struct cbmScreenPosition screenPosition, char* asciiString, uint8_t palletColor);
    void makeTextShaderObject(GLuint shaderProgram, struct GlObjectDataSet *vds);
    void drawCbmScreen(struct cbmScreen* screen, GLfloat width, GLfloat height);
#endif