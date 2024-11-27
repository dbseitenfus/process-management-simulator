//
//  main.c
//  process-management-simulator
//
//  Created by Daniel Seitenfus on 11/11/24.
//

#include <stdio.h>
#include <stdlib.h>

void init(void);

int main(int argc, const char * argv[]) {
    init();
    return 0;
}

FILE* open_file(const char *filename) {
    FILE *fptr = fopen(filename, "r");
    
    if(fptr == NULL) {
      printf("Not able to open the file.");
        return NULL;
    }

    return fptr;
}

void init(void) {
    FILE *file = open_file("input.txt");
    char line[100];
    fgets(line, sizeof(line), file) ;
    int nProc = line[0] - '0';
    int nDisp = line[2] - '0';
    printf("nProc: %d\n", nProc);
    printf("nDisp: %d\n", nDisp);
}
