#include "parser.h"
#include <stdio.h>
#include <stdlib.h>

/* ========== PRIVTATE functions ========== */

static ASTNode* parse_stmts(Parser* parser) {


}

static ASTNode* parse_stmt(Parser* parser) {

}

static ASTNode* parse_var_decl(Parser* parser) { 

}

static ASTNode* parse_assignment_stmt(Parser* parser) {

}

static ASTNode* parse_if_stmt(Parser* parser) {

}

static ASTNode* parse_loop_stmt(Parser* parser) {

}

static ASTNode* parse_io_stmt(Parser* parser) {

}

static ASTNode* parse_inc_dec_stmt(Parser* parser) {

}

static ASTNode* parse_expr(Parser* parser) {

}

static ASTNode* parse_term(Parser* parser) {

}

static ASTNode* parse_factor(Parser* parser) {

}

static ASTNode* parse_conditional(Parser* parser) {

}

static void advance(Parser* parser) {
    /*
    Advances the parser throughout the tokens 
    */

    // Free the old current_token's lexeme (parser owns the memory)
    if (parser->current_token.lexeme) {
        free((void*)parser->current_token.lexeme);
    }
    
    // Move peek to current
    parser->current_token = parser->peek_token;
    
    // Get new peek token from lexer
    parser->peek_token = next_token(parser->lexer);
}


static bool match(Parser* parser, TokenType expectedType) {
    
    if (parser->current_token.tokenType != expectedType) {
        parser_error(parser);
        return false;

    } else {
        return true;
    }

}

static void parser_error(Parser* parser) {

    if 

}


/* ========== PUBLIC API ========== */
Parser* parser_init(Lexer *l) {
    /*
    Initializes the parser structer with the current token and following token

    args:
        *l (Lexer) -> Lexer instance 

    returns:
        parser (Parser) -> Syntax Parser instance 
    */
    Parser *parser = (Parser*)malloc(sizeof(Parser));

    parser->lexer = l;                      // save lexer to parser
    parser->current_token = next_token(l);  // get the current toke
    parser->peek_token = next_token(l);     // automatically advance and get the next token for lookahead

    return parser;
    
}


void parser_free(Parser* parser) {
    /*
    Free up tokens and lexemes(The parser now owns the lexemes) of current and peek tokens 
    */

    if (!parser) {
        return;
    } 

    if (parser->current_token.tokenType) {
        free(parser->current_token.tokenType);
    } 

    if (parser->current_token.lexeme) {
        free(parser->current_token.lexeme);
    }

    if (parser->peek_token.tokenType) {
        free(parser->peek_token.lexeme);
    } 
    if (parser->peek_token.lexeme) {
        free(parser->peek_token.lexeme);
    }

}
ASTNode* parse_program(Parser* parser);  // Main entry point
