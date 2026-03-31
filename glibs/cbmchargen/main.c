#include <stdio.h>
#include "cbmchargen.h"

void main(int argc, char* argv[]) {
    for(size_t currentArgument = 2; currentArgument < argc; currentArgument++) {
        printf("%s\n", cbmBitmapsFromString(loadChargen(argv[1]), argv[currentArgument]));
    }
}