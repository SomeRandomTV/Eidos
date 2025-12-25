#include "lexer.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ===== Helper Functions ===== */

/* 
Return the next char in src code without consuming it

args: 
    *l (Lexer) Lexer at a position

returns: 
    (char) -> copy of the next char in code
*/
static char peek(Lexer *l) {
    return l->src[l->pos];
}

/*
Consume the next character and move forward in the code
args:
    *l (Lexer) -> Lexer at a position

returns: 
    c (char) -> THE next character in src code
*/
static char advance(Lexer *l) {

    char c = l->src[l->pos++];

    if (c == '\n') {
        l->line++;
        l->col=1;
    } else {
        l->col++;
    }

    return c;

}

/*
Creates a new token after processing lexeme

args: 
    *l (Lexer) -> Lexer struct at pos
    type (TokenType) -> Lexeme's corresponding token
    *lexeme (char) -> the current lexeme
    line (size_t) -> line of current lexeme/token
    col (size_t) -> column of current lexeme/token

returns: 
    t (TokenType) -> Newly created token from lexeme
*/
static Token make_token(Lexer *l, TokenType type, const char *lexeme) {
    Token t;
    t.tokenType = type;    
    t.lexeme = lexeme;
    t.line = l->line;
    t.col = l->col;
    
    return t;
}

/*
Prints token information for debugging

args:
    *l (Lexer) -> Lexer instance
    start (size_t) -> Starting position of lexeme
*/
static char *build_lexeme(Lexer *l, size_t start) {
    size_t len = l->pos - start;
    char *lexeme = malloc(len + 1);
    strncpy(lexeme, l->src + start, len);
    lexeme[len] = '\0';
    return lexeme;
}

/*
Prints token information for debugging

args:
    *lexeme (char) -> Lexeme string
    type (TokenType) -> Token type
*/
static void print_token_info(const char *lexeme, TokenType type) {
    const char *token_name = "UNKNOWN";
    
    switch (type) {
        case KEYWORD_PRINT: token_name = "KEYWORD_PRINT"; break;
        case KEYWORD_READ: token_name = "KEYWORD_READ"; break;
        case KEYWORD_FOR: token_name = "KEYWORD_FOR"; break;
        case KEYWORD_LET: token_name = "KEYWORD_LET"; break;
        case KEYWORD_WHILE: token_name = "KEYWORD_WHILE"; break;
        case KEYWORD_IF: token_name = "KEYWORD_IF"; break;
        case KEYWORD_ELSE: token_name = "KEYWORD_ELSE"; break;
        case IDENTIFIER: token_name = "IDENTIFIER"; break;
        case INT_LIT: token_name = "INT_LIT"; break;
        case PLUS_OP: token_name = "PLUS_OP"; break;
        case SUB_OP: token_name = "SUB_OP"; break;
        case MULT_OP: token_name = "MULT_OP"; break;
        case DIV_OP: token_name = "DIV_OP"; break;
        case NOT_OP: token_name = "NOT_OP"; break;
        case INC_OP: token_name = "INC_OP"; break;
        case DEC_OP: token_name = "DEC_OP"; break;
        case GREATER_OP: token_name = "GREATER_OP"; break;
        case LESSER_OP: token_name = "LESSER_OP"; break;
        case GEQUAL_OP: token_name = "GEQUAL_OP"; break;
        case LEQUAL_OP: token_name = "LEQUAL_OP"; break;
        case NEQUAL_OP: token_name = "NEQUAL_OP"; break;
        case ASSIGN_OP: token_name = "ASSIGN_OP"; break;
        case EQUAL_OP: token_name = "EQUAL_OP"; break;
        case LEFT_PAREN: token_name = "LEFT_PAREN"; break;
        case RIGHT_PAREN: token_name = "RIGHT_PAREN"; break;
        case LEFT_CURL: token_name = "LEFT_CURL"; break;
        case RIGHT_CURL: token_name = "RIGHT_CURL"; break;
        case SEMICOLON: token_name = "SEMICOLON"; break;
        case EOF_TOK: token_name = "EOF_TOK"; break;
        default: token_name = "UNKNOWN"; break;
    }
    
    printf("%s %s\n", lexeme, token_name);
}

/*
Classify lexeme and return appropriate token type

args:
    *lexeme (char) -> Lexeme string to classify

returns:
    (TokenType) -> Classified token type
*/
static TokenType classify_token(const char *lexeme) {
    // Keywords
    if (strcmp(lexeme, "print") == 0) return KEYWORD_PRINT;
    if (strcmp(lexeme, "read") == 0) return KEYWORD_READ;
    if (strcmp(lexeme, "for") == 0) return KEYWORD_FOR;
    if (strcmp(lexeme, "let") == 0) return KEYWORD_LET;
    if (strcmp(lexeme, "while") == 0) return KEYWORD_WHILE;
    if (strcmp(lexeme, "if") == 0) return KEYWORD_IF;
    if (strcmp(lexeme, "else") == 0) return KEYWORD_ELSE;
    
    // Operators
    if (strcmp(lexeme, "+") == 0) return PLUS_OP;
    if (strcmp(lexeme, "-") == 0) return SUB_OP;
    if (strcmp(lexeme, "*") == 0) return MULT_OP;
    if (strcmp(lexeme, "/") == 0) return DIV_OP;
    if (strcmp(lexeme, "!") == 0) return NOT_OP;
    if (strcmp(lexeme, "++") == 0) return INC_OP;
    if (strcmp(lexeme, "--") == 0) return DEC_OP;
    if (strcmp(lexeme, ">") == 0) return GREATER_OP;
    if (strcmp(lexeme, "<") == 0) return LESSER_OP;
    if (strcmp(lexeme, ">=") == 0) return GEQUAL_OP;
    if (strcmp(lexeme, "<=") == 0) return LEQUAL_OP;
    if (strcmp(lexeme, "!=") == 0) return NEQUAL_OP;
    if (strcmp(lexeme, "=") == 0) return ASSIGN_OP;
    if (strcmp(lexeme, "==") == 0) return EQUAL_OP;
    
    // Delimiters
    if (strcmp(lexeme, "(") == 0) return LEFT_PAREN;
    if (strcmp(lexeme, ")") == 0) return RIGHT_PAREN;
    if (strcmp(lexeme, "{") == 0) return LEFT_CURL;
    if (strcmp(lexeme, "}") == 0) return RIGHT_CURL;
    if (strcmp(lexeme, ";") == 0) return SEMICOLON;

    
    // Check if it's a number
    if (isdigit(lexeme[0])) {
        for (size_t i = 0; lexeme[i] != '\0'; i++) {
            if (!isdigit(lexeme[i])) return UNKNOWN;
        }
        return INT_LIT;
    }
    
    // Check if it's an identifier
    if (isalpha(lexeme[0]) || lexeme[0] == '_') {
        for (size_t i = 1; lexeme[i] != '\0'; i++) {
            if (!isalnum(lexeme[i]) && lexeme[i] != '_') return UNKNOWN;
        }
        return IDENTIFIER;
    }
    
    return UNKNOWN;
}

/* ========== Public API ========== */
/*
Initializes the lexer instance

args: 
    *l (Lexer) -> Lexer instance 
    *src (char) -> Source code
*/
void init_lexer(Lexer *l, const char *src) {
    l->src = src;
    l->pos = 0;
    l->line = 1;
    l->col = 1;
}



/* ========== PUBLIC API ========== */

/*
Processes lexeme and returns token

args: 
    *l (Lexer) -> Lexer instance 

returns:
    token (Token) -> Processed token 
*/
Token next_token(Lexer *l) {
    // skip whitespace
    while (isspace(peek(l))) {
        advance(l);
    }
    
    char curr_c = peek(l);
    size_t start = l->pos;
    
    // check for end of input
    if (curr_c == '\0') {
        char *lexeme = build_lexeme(l, start);
        return make_token(l, EOF_TOK, lexeme);
    }

    // numbers
    if (isdigit(curr_c)) {
        while (isdigit(peek(l))) {
            advance(l);
        }
        char *lexeme = build_lexeme(l, start);
        TokenType type = classify_token(lexeme);
        print_token_info(lexeme, type);
        Token t = make_token(l, type, lexeme);
        return t;
    }

    // identifiers and Keywords
    if (isalpha(curr_c) || curr_c == '_') {
        while (isalnum(peek(l)) || peek(l) == '_') {
            advance(l);
        }
        char *lexeme = build_lexeme(l, start);
        TokenType type = classify_token(lexeme);
        print_token_info(lexeme, type);
        Token t = make_token(l, type, lexeme);
        return t;
    }

    // operators and delimiters
    advance(l);
    
    // check for two-character operators
    char next_c = peek(l);
    if ((curr_c == '+' && next_c == '+') ||
        (curr_c == '-' && next_c == '-') ||
        (curr_c == '=' && next_c == '=') ||
        (curr_c == '!' && next_c == '=') ||
        (curr_c == '<' && next_c == '=') ||
        (curr_c == '>' && next_c == '=')) {
        advance(l);
    }
    
    char *lexeme = build_lexeme(l, start);
    TokenType type = classify_token(lexeme);
    print_token_info(lexeme, type);
    Token t = make_token(l, type, lexeme);
    return t;
}

