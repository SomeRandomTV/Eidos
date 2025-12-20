#include "lexer.h"
#include "ctype.h"
#include "stdio.h"

/* ===== Helper Functions ===== */

/* 
Return the next char in src code without consuming it
    args: *l (Lexer) Lexer at a position

    returns: char, a copy of the next char in code
*/
static char peek(Lexer *l) {
    return l->src[l->pos];
}

/*
Consume the next character and move forward in the code
    args: *l (Lexer) Lexer at a position

    returns: c, THE next character in src code
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
    args: *l (Lexer) -> Lexer struct at pos
          type (TokenType) -> Lexeme's corresponding token
          *lexeme (char) -> the current lexeme
          line (size_t) -> line of current lexeme/token
          col (size_t) -> column of current lexeme/token
          
    returns: t (TokenType) -> Newly created token from lexeme
*/
static Token make_token(TokenType type, char *lexeme, size_t line, size_t col) {
    Token t;

    t.tokenType = type;
    t.lexeme = lexeme;
    t.line = line;
    t.col = col;

    return t;

}

/* ========== Public API ========== */
/*
Initializes the lexer

args: *l (Lexer) -> Lexer instance 
      *src (char) -> Source code
*/
void init_lexer(Lexer *l, const char *src) {
    l->src = src;
    l->pos = 0;
    l->line = 1;
    l->col = 1;
}

