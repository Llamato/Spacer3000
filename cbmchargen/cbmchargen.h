#ifndef CBMTOBMP_H
#define CBMTOBMP_H
    #include <stddef.h>
    #define CBM_CHAR_SIZE 8
    #define CBM_MAX_STRING_LENGTH 255
    typedef struct {
        char ascii;
        char cpmBitmap[8];
    } cbmChar;
    char* loadChargen(char* filename);
    void cbmBitmapFromChar(char* returnBucket, char* chargen, char asciiChar);
    char* cbmBitmapsFromString(char* chargen, char* asciiString);
    
#endif