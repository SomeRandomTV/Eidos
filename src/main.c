#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer/lexer.h"

static char *read_file(const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) {
        perror("fopen");
        exit(1);
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    char *buffer = malloc(size + 1);
    fread(buffer, 1, size, f);
    buffer[size] = '\0';

    fclose(f);
    return buffer;
}

int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("EEOR: Not enough arguments given. Exiting now.\n");
        return -1;
    } 
    if (argv == NULL) {
        printf("ERROR: File not given. Exiting Now.\n");
        return -2;
    }

    char *source = read_file(argv[1]);

    Lexer lexer;
    init_lexer(&lexer, source);

    Token tok;
    printf("Lexeme Token\n");
    do {
        tok = next_token(&lexer);

        if (tok.lexeme) {
            free((void *)tok.lexeme);
        }
    } while (tok.tokenType != EOF_TOK);

    free(source);

    return 0;

    
}