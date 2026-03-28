#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cbmchargen.h"

char* loadChargen(char* filename) {
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
    char *bytes = malloc(fsize + 1 + 1024);
    if(fread(bytes, fsize, 1, f) < 1){
        printf("Error loading chargen: %s", filename);
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