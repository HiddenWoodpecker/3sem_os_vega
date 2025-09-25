#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
    const char *pattern = NULL;
    const char *filename = NULL;

    for (int i = 1; i < argc; i++) {
            if (pattern == NULL) {
                pattern = argv[i];
            } else if (filename == NULL) {
                filename = argv[i];
            }
        }
    

    if (pattern == NULL) {
        printf("g: pattern required\n");
        return 1;
    }

    FILE *file;
    if (filename != NULL) {
        file = fopen(filename, "r");
        if (!file) {
            printf("g: unable to open %s\n", filename);
            return 1;
        }
    } else {
        file = stdin;
    }

    char line[4096];
    int line_num = 1;

    while (fgets(line, sizeof(line), file)) {
        size_t len = strlen(line);
        if (len > 0 && line[len-1] == '\n') {
            line[len-1] = '\0';
        }

        if (strstr(line, pattern)) {
            printf("%s\n", line); 
        }
        line_num++;
    }

    if (file != stdin) {
        fclose(file);
    }

    return 0;
}
