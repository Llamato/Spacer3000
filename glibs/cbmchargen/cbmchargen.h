#ifndef CBMCHARGEN_H
#define CBMCHARGEN_H
    #include <stddef.h>
    #include "../common.h"
    #define CBM_CHARGEN_SIZE 2048
    #define CBM_CHAR_SIZE 8
    #define CBM_MAX_STRING_LENGTH 255
    struct cbmText {
        struct Vector2 position;
        float scale;

        char *cbmChargenBytes;
        char *petsciiString;
        char *screenCodeString;

        struct GlObjectDataSet glData;
    };
    char* loadChargen(char* filename);
    char asciiToPetscii(char asciiChar);
    char petsciiToScreencode(char petsciiChar);
    char* petsciiStringToScreencodeString(char* petsciiString);
    char* asciiStringToPetsciiString(char *asciiString);
    void cbmBitmapFromChar(char* returnBucket, char* chargen, char asciiChar);
    char* cbmBitmapsFromString(char* chargen, char* asciiString);
    struct cbmText makeText(struct Vector2 position, struct Vector2 dimensions, char *cbmChargen, char *text, struct Color textColor, struct Color backgroundColor);
    
#endif