#include "stdio.h"
#include "stdlib.h"

#include "stdint.h"

int main(int argc, char** argv) {

    FILE *fp;
    
    fp = fopen(argv[1], "r");
    if (fp == NULL) {
        exit(EXIT_FAILURE);
    }

    uint8_t wavanal[60];
    fread(&wavanal, sizeof(wavanal), 1, fp);
    
    for (int i = 0; i < 60; i++) {
        printf("%d: (0x%02x | %c)\n", i, wavanal[i], wavanal[i]);
    }

    fclose(fp);
    exit(EXIT_SUCCESS);

}