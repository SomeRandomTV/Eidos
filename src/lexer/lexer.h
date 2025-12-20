#pragma once

/*
This file contains all functions/variables necassary for the lexical analysis of the compiler 
Simple Finite State Automata will be used to move between strings and convert lexemes to tokens 
*/

#include "stddef.h"

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
    GREATER_OP,         // >
    LESSER_OP,          // <
    GEQUAL_OP,          // >=
    LEQUAL_OP,          // <=
    ASSIGN_OP,          // =
    EQUAL_OP,           // ==
    LEFT_PAREN,         // (
    RIGHT_PAREN,        // )
    LEFT_CURL,          // {
    RIGHT_CURL,         // }

} TokenType;


typedef struct Token {
    size_t line;            // line the token/lexeme are on
    char *lexeme;          // lexeme type, "print", "(", ")"
    TokenType tokenType;    // respective token type
} Token;


/*
Initializes the lexer for lexical analysis 
- Creates emtpy token struct
- Opens file
*/
void init_lexer();

void parse_ident();
