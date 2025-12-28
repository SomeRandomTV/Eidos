#include "parser.h"
#include <stdio.h>
#include <stdlib.h>



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

    // Free lexeme strings (don't free tokenType - it's an enum, not allocated memory)
    if (parser->current_token.lexeme) {
        free((void*)parser->current_token.lexeme);
    }

    if (parser->peek_token.lexeme) {
        free((void*)parser->peek_token.lexeme);
    }

    // Free parser struct itself
    free(parser);
}

ASTNode* parse_program(Parser* parser) {
    // Create the program root node
    ASTNode *program = (ASTNode*)malloc(sizeof(ASTNode));
    if (!program) {
        fprintf(stderr, "Error: Failed to allocate AST program node\n");
        exit(1);
    }
    
    program->type = AST_PROGRAM_NODE;
    program->data.program.statements = parse_stmts(parser);
    
    // Expect EOF at the end of the program
    if (parser->current_token.tokenType != EOF_TOK) {
        parser_error(parser, EOF_TOK);
    }
    
    return program;
}


/* ========== PRIVTATE helper functions ========== */

static ASTNode* parse_stmts(Parser* parser) {
    /*
    Parses STMTS nodes reccursively 

    args:
        parser (Parser) -> pointer to Parser Instance

    returns:
        stmts (ASTNode) -> Linked List of Stmt Nodes 
    */

    if (parser->current_token.tokenType == EOF_TOK || parser->current_token.tokenType == RIGHT_CURL) {
        return NULL;
    }

    // Create a statement list node
    ASTNode *stmts = (ASTNode*)malloc(sizeof(ASTNode));
    if (!stmts) {
        fprintf(stderr, "Error: Failed to allocate statement list node\n");
        exit(1);
    }

    stmts->type = AST_STMTS_NODE;
    stmts->data.stmts.stmt = parse_stmt(parser);
    stmts->data.stmts.next = parse_stmts(parser);

    return stmts;


}

static ASTNode* parse_stmt(Parser* parser) {

    if (parser->current_token.tokenType == SEMICOLON) {
        return NULL;       
    } 

    ASTNode *stmt = (ASTNode*)malloc(sizeof(ASTNode));
    if (!stmt) {
        fprintf(stderr, "Error: Failed to allocate statement node\n");
    }

    switch (parser->current_token.tokenType) {

    case KEYWORD_LET:   // var declaration 
        
        stmt->type = AST_ASSIGN_NODE;
        stmt->data.assign.identifier = NULL; // PARSE IDENTIFIER 
        stmt->data.assign.value = NULL;      // Parse Expression
        break;
    
    default:
        break;
    }

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
        parser_error(parser, expectedType);
        return false;
    }
    return true;
}

static void parser_error(Parser* parser, TokenType expectedType) {
    fprintf(stderr, "Parse Error at line %zu, column %zu:\n", 
            parser->current_token.line, 
            parser->current_token.col);
    fprintf(stderr, "  Unexpected token: %d (lexeme: '%s')\n", 
            parser->current_token.tokenType,
            parser->current_token.lexeme ? parser->current_token.lexeme : "NULL");
    fprintf(stderr, "  Expected token: %d\n", expectedType);
    exit(1);
}


