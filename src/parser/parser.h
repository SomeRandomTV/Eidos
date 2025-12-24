#pragma once

#include "../lexer/lexer.h"
#include "ast.h"
#include <stdbool.h>

typedef struct Parser {
    Lexer* lexer;
    Token current_token;
    Token peek_token;
} Parser;

// Parser initialization and cleanup
Parser* parser_init(Lexer* lexer);
void parser_free(Parser* parser);

// Core parsing functions
ASTNode* parse_program(Parser* parser);
ASTNode* parse_stmts(Parser* parser);
ASTNode* parse_stmt(Parser* parser);

// Statement parsing
ASTNode* parse_var_decl(Parser* parser);
ASTNode* parse_assignment_stmt(Parser* parser);
ASTNode* parse_if_stmt(Parser* parser);
ASTNode* parse_loop_stmt(Parser* parser);
ASTNode* parse_io_stmt(Parser* parser);
ASTNode* parse_inc_dec_stmt(Parser* parser);

// Expression parsing
ASTNode* parse_expr(Parser* parser);
ASTNode* parse_term(Parser* parser);
ASTNode* parse_factor(Parser* parser);
ASTNode* parse_conditional(Parser* parser);

// Helper functions
void advance(Parser* parser);
bool match(Parser* parser, TokenType type);
bool expect(Parser* parser, TokenType type);
void parser_error(Parser* parser, const char* message);