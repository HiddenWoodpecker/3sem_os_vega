#include <stdio.h>
#include <string.h>
#include <assert.h>
typedef struct {
    int number_all;
    int number_nonblank;
    int show_ends;
    char* filename;
} Options;

Options parse_arguments(int count, char** values) {
    Options opts = {0};
    
    for (int i = 1; i < count; i++) {
        if (values[i][0] == '-') {
            for (char* s = values[i] + 1; *s; s++) {
                switch (*s) {
                    case 'n': opts.number_all = 1; break;
                    case 'b': opts.number_nonblank = 1; break;
                    case 'E': opts.show_ends = 1; break;
                }
            }
        } else {
            if (opts.filename) {
		assert(1==0 && "Too many files specified");
            } else {
                opts.filename = values[i];
            }
        }
    }
    return opts;
}


int main(int argc, char** argv) {
    Options options = parse_arguments(argc, argv);
 
    FILE* file = options.filename ? fopen(options.filename, "r") : stdin;
    if (!file) {
        printf("f: unable to open %s\n", options.filename);
        return 1;
    }
    
    char buffer[1024];
    int line_counter = 1;
    
    while (fgets(buffer, sizeof(buffer), file)) {
        buffer[strlen(buffer) - 1] = '\0';
        
        if (options.number_nonblank) {
            printf( "%d\t", line_counter );
            if (buffer[0]) line_counter++;
        } else if (options.number_all) {
            printf("%6d\t", line_counter++);
        }
        
        printf("%s", buffer);
        printf(options.show_ends ? "$\n" : "\n");
    }
    
    if (file != stdin) fclose(file);
    return 0;
}
