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


void parser_free(Parser *parser) {
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

ASTNode* parse_program(Parser *parser) {
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

static ASTNode* parse_stmts(Parser *parser) {
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

static ASTNode* parse_stmt(Parser *parser) {

    if (parser->current_token.tokenType == SEMICOLON) {
        return NULL;       
    } 

    ASTNode *stmt = (ASTNode*)malloc(sizeof(ASTNode));
    if (!stmt) {
        fprintf(stderr, "Error: Failed to allocate statement node\n");
    }

    switch (parser->current_token.tokenType) {

    case KEYWORD_LET:   // var declaration, let x = 5;
        stmt = parse_var_decl(parser);
        break;

    case IDENTIFIER:    // x = 6;

        if (parser->peek_token.tokenType == INC_OP || parser->peek_token.tokenType == DEC_OP) {
            stmt = parse_unary_expr(parser);
        } else if (parser->peek_token.tokenType == ASSIGN_OP) {
            stmt = parse_assignment_stmt(parser);
        } else {
            parser_error(parser, ASSIGN_OP);
        }
        break;

    case KEYWORD_IF:
        stmt = parse_if_stmt(parser);
        break;


    default:
        break;
    }

}

static ASTNode* parse_var_decl(Parser *parser) { 
    /*
    Parses the VAR_DECL node (let IDENTIFIER = <expr>) 

    args:
        parser (Parser) -> syntax parser instance 

    returns:
        var_decl (ASTNode) -> variable declaration statement
    */

    advance(parser);

    if (!match(parser, IDENTIFIER)) {
        return NULL;
    }

    char *identifier = strdup(parser->current_token.lexeme);
    advance(parser);    // now at '='

    if (!match(parser, ASSIGN_OP)) {
        free(identifier);
        return NULL;
    }

    advance(parser);    // move to the <expr>

    ASTNode *value = parse_expr(parser);    // parse the expression to get value 
    ASTNode *var_decl = (ASTNode*)malloc(sizeof(ASTNode));

    if (!var_decl) {
        fprintf(stderr, "Error: Failed to allocate memory for var declaration Node"); 
        free(identifier);
        return NULL;
    }

    var_decl->type = AST_VAR_DECL_NODE;
    var_decl->data.var_decl.identifer = identifier;
    var_decl->data.var_decl.value = value;

    return var_decl;
}

static ASTNode* parse_assignment_stmt(Parser *parser) {
    /*
    Parses the assignment node IDENT '=' <expr>

    args:
        parser (fuck you)

    returns: 
        assignment_stmt (ASTNode) -> parsed assignment node
    */

    // get the identifer before moving on
    char *identifier = strdup(parser->current_token.lexeme);

    advance(parser);    // should be at '='
    advance(parser);    // move to expression

    ASTNode *value = parse_expr(parser);
    ASTNode *assignment_node = (ASTNode*)malloc((sizeof(ASTNode)));

    if (!assignment_node) {
        fprintf(stderr, "Error: Failed to allocate memory for assignment node");
        free(identifier);
        return NULL;
    }

    assignment_node->type = AST_ASSIGN_NODE;
    assignment_node->data.assignment.identifier = identifier;
    assignment_node->data.assignment.value = value;

    return assignment_node;

}

static ASTNode* parse_if_stmt(Parser *parser) {
    /*
    Parses If-statements: if (<conditional>) { <stmts> }
    */

    advance(parser);    // move to the opening paren
    if (!match(parser->current_token.tokenType, LEFT_PAREN)) {
        return NULL;
    }
    advance(parser);    // should be at the start of the conditional

    ASTNode *conditional = parse_conditional(parser);
    
    if (!match(parser->current_token.tokenType, RIGHT_PAREN) || !match(parser->peek_token.tokenType, LEFT_CURL)) {
        free(conditional);
        return NULL;
    }

    advance(parser);    // consume right paren
    advnace(parser);    // conume left curl

    // now in the then_block 

    ASTNode *then_block = parse_stmts(parser);

    if (!match(parser->current_token.tokenType, RIGHT_CURL)) {
        // TODO: FUck you
    }



    

}

static ASTNode* parse_loop_stmt(Parser *parser) {

}

static ASTNode* parse_io_stmt(Parser *parser) {

}


static ASTNode* parse_expr(Parser *parser) {

}
static ASTNode* parse_unary_expr(Parser *parser) {


}

static ASTNode *parse_binary_expr(Parser *parser) {

}

static ASTNode* parse_term(Parser *parser) {

}

static ASTNode* parse_factor(Parser *parser) {

}

static ASTNode* parse_conditional(Parser *parser) {

}

static void advance(Parser *parser) {
    /*
    Advances the parser throughout the tokens 

    args:
        parser (Parser) -> Parser instance
    */

    // free the old current_token's lexeme (parser owns the memory)
    if (parser->current_token.lexeme) {
        free((void*)parser->current_token.lexeme);
    }
    
    // move peek to current
    parser->current_token = parser->peek_token;
    
    // get new peek token from lexer
    parser->peek_token = next_token(parser->lexer);
}


static bool match(Parser *parser, TokenType expectedType) {
    /*
    Matches the current token with the expected token

    args:
        parser (Parser) -> Parser instance 
        expectedType (TokenType) -> the excpected token type

    returns:
        false: If tokens do not match, calls parser_error()
        true: If tokens match
    */
    if (parser->current_token.tokenType != expectedType) {
        parser_error(parser, expectedType);
        return false;
    }
    return true;
}

static void parser_error(Parser *parser, TokenType expectedType) {
    /*
    Throws error when parser finds an error with the token it got and the expected token

    args:
        parser (Parser) -> Fuck you
        expectedType (TokenType) -> Fuck you
    */
    fprintf(stderr, "Parse Error at line %zu, column %zu:\n", 
            parser->current_token.line, 
            parser->current_token.col);
    fprintf(stderr, "  Unexpected token: %d (lexeme: '%s')\n", 
            parser->current_token.tokenType,
            parser->current_token.lexeme ? parser->current_token.lexeme : "NULL");
    fprintf(stderr, "  Expected token: %d\n", expectedType);
    exit(1);
}


