#pragma once

/*
This file contains all functions/variables necassary for the lexical analysis of the compiler 
Simple Finite State Automata will be used to move between strings and convert lexemes to tokens 
*/

#include <stddef.h>

// token types for this compiler
typedef enum TokenType {
    KEYWORD_PRINT,      // print
    KEYWORD_READ,       // read
    KEYWORD_FOR,        // for
    KEYWORD_LET,        // let
    KEYWORD_WHILE,      // while
    IDENTIFIER,         // a, d, counter, x, y, z
    INT_LIT,            // 1,2,3,234,5432345
    PLUS_OP,            // +
    SUB_OP,             // -
    MULT_OP,            // *
    DIV_OP,             // /
    NOT_OP,             // !
    INC_OP,             // ++
    DEC_OP,             // --
    GREATER_OP,         // >
    LESSER_OP,          // <
    GEQUAL_OP,          // >=
    LEQUAL_OP,          // <=
    NEQUAL_OP,          // !=
    ASSIGN_OP,          // =
    EQUAL_OP,           // ==
    LEFT_PAREN,         // (
    RIGHT_PAREN,        // )
    LEFT_CURL,          // {
    RIGHT_CURL,         // }
    EOF_TOK,            // End-of-File
    UNKNOWN,            // Unknown token
} TokenType;


typedef struct Token {
    size_t line;            // line the token/lexeme are on
    size_t col;             // column location of the token/lexeme
    TokenType tokenType;    // respective token type
    const char *lexeme;     // lexeme type, "print", "(", ")"
} Token;

typedef struct Lexer {
    const char *src;        // source code read from file
    size_t pos;             // position of lexer 
    size_t line;            // current line
    size_t col;             // current column
} Lexer;


/* ========== Public API Functions ========== */

/*
Initializes the lexer for lexical analysis 
Initializes the structures above 
*/
void init_lexer(Lexer *l, const char *src);

/*
Processes the next lexeme and returns the token 
all the the request of the Syntax Parser
*/
Token next_token(Lexer *l);
