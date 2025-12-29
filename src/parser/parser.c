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
    program->data.program.stmts = parse_stmts(parser);
    
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

    return stmt;

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

    if (!match(parser, SEMICOLON)) {
        free(identifier);
        return NULL;
    }

    advance(parser);    // consume the ;

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


    if (!match(parser, SEMICOLON)) {
        free(identifier);
        return NULL;
    }

    advance(parser);    // consume the ;

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
    Parses If-statements: if (<conditional>) { <stmts> } [else { <stmts> }]
    */

    advance(parser);    // move to the opening paren
    if (!match(parser, LEFT_PAREN)) {
        return NULL;
    }
    advance(parser);    // should be at the start of the conditional

    ASTNode *conditional = parse_conditional(parser);
    
    if (!match(parser, RIGHT_PAREN)) {
        free(conditional);
        return NULL;
    }
    
    advance(parser);    // consume right paren
    
    if (!match(parser, LEFT_CURL)) {
        free(conditional);
        return NULL;
    }
    
    advance(parser);    // consume left curl

    // now in the then_block 
    ASTNode *then_block = parse_stmts(parser);

    if (!match(parser, RIGHT_CURL)) {
        free(then_block);
        free(conditional);
        return NULL;
    }
    
    advance(parser);    // consume right curl

    // Allocate the if statement node
    ASTNode *if_stmt = (ASTNode*)malloc(sizeof(ASTNode));
    if (!if_stmt) {
        fprintf(stderr, "Error: Failed to allocate memory for if statement node\n");
        free(then_block);
        free(conditional);
        return NULL;
    }

    if_stmt->type = AST_IF_STMT_NODE;
    if_stmt->data.if_stmt.condition = conditional;
    if_stmt->data.if_stmt.then_block = then_block;
    
    // Check for optional else clause
    if (parser->current_token.tokenType == KEYWORD_ELSE) {
        advance(parser);    // consume 'else'
        
        if (!match(parser, LEFT_CURL)) {
            free(if_stmt);
            free(then_block);
            free(conditional);
            return NULL;
        }
        
        advance(parser);    // consume left curl
        
        ASTNode *else_block = parse_stmts(parser);
        
        if (!match(parser, RIGHT_CURL)) {
            free(else_block);
            free(if_stmt);
            free(then_block);
            free(conditional);
            return NULL;
        }
        
        advance(parser);    // consume right curl
        
        if_stmt->data.if_stmt.else_block = else_block;
    } else {
        if_stmt->data.if_stmt.else_block = NULL;
    }

    return if_stmt;

}

static ASTNode* parse_loop_stmt(Parser *parser) {
    /*
    Parses loop statements: loop (<conditional>) { <stmts> }
    
    TODO:
    1. Advance past 'loop' keyword
    2. Check for and advance past LEFT_PAREN
    3. Parse conditional with parse_conditional()
    4. Check for and advance past RIGHT_PAREN
    5. Check for and advance past LEFT_CURL
    6. Parse loop body with parse_stmts()
    7. Check for and advance past RIGHT_CURL
    8. Create AST_LOOP_NODE
    9. Set condition and body
    10. Return loop node
    
    This follows the SAME pattern as parse_if_stmt but simpler (no else clause)
    */

}

static ASTNode* parse_io_stmt(Parser *parser) {
    /*
    Parses I/O statements: print(<expr>) and input(IDENTIFIER)
    
    TODO:
    1. Check if current token is KEYWORD_PRINT or KEYWORD_INPUT
    2. For print:
       - Advance past 'print'
       - Check for LEFT_PAREN
       - Advance past '('
       - Parse expression with parse_expr()
       - Check for RIGHT_PAREN
       - Advance past ')'
       - Create AST_PRINT_NODE
       - Set expression to print
       - Return print node
    3. For input:
       - Advance past 'input'
       - Check for LEFT_PAREN
       - Advance past '('
       - Check for IDENTIFIER
       - Save identifier name
       - Advance past identifier
       - Check for RIGHT_PAREN
       - Advance past ')'
       - Create AST_INPUT_NODE
       - Set target identifier
       - Return input node
    4. Don't forget semicolons at the end!
    */

}


static ASTNode* parse_expr(Parser *parser) {
    /*
    Parses expressions with + and - operators (lowest precedence)
    Handles: <term> (('+' | '-') <term>)*
    
    TODO:
    1. Parse first term with parse_term()
    2. If no + or - operator, return the term directly (not a binary expr!)
    3. If + or - found, enter loop:
       - Allocate binary_expr node INSIDE the loop (not before!)
       - Set binary_expr->left = left_term (the accumulated result)
       - Save operator and advance
       - Parse right_term
       - Set binary_expr->right = right_term
       - Update left_term = binary_expr (so next iteration wraps this one)
    4. After loop, return left_term (which is now the full expression tree)
    
    CRITICAL ISSUES TO FIX:
    - Don't allocate binary_expr before knowing if you need it
    - Must update left_term in the loop or you lose chaining (a+b+c breaks)
    - Return left_term if there's no operator (just a number or variable)
    - Logic error: using || instead of && in the if condition
    */
    
    ASTNode *left_term = parse_term(parser);

    if (parser->current_token.tokenType != SUB_OP || parser->current_token.tokenType != PLUS_OP) {
        return left_term;
    }

    ASTNode *binary_expr = (ASTNode*)malloc(sizeof(ASTNode));

    if (!binary_expr) {
        fprintf(stderr, "Error: Failed to allocate memory for binary expression");
        free(left_term);
        return NULL;
    }

    while (parser->current_token.tokenType == SUB_OP || parser->current_token.tokenType == PLUS_OP) {
        char *operator = strdup(parser->current_token.lexeme);
        advance(parser);
        ASTNode* right_term = parse_term(parser);

        // TODO: Build binary_expr node here with left_term, operator, right_term
        // TODO: Set left_term = binary_expr for next iteration
    }

    // TODO: Return the final expression (currently nothing is returned!)

}
static ASTNode* parse_unary_expr(Parser *parser) {
    /*
    Parses unary expressions: ++x, --x, x++, x--, -expr, !expr
    
    TODO:
    1. Check if operator comes before or after identifier:
       - Prefix: INC_OP/DEC_OP then IDENTIFIER (++x, --x)
       - Postfix: IDENTIFIER then INC_OP/DEC_OP (x++, x--)
       - Unary minus/not: SUB_OP/NOT_OP then expression (-5, !true)
    2. For prefix/postfix on identifiers:
       - Parse identifier
       - Create AST_UNARY_EXPR node
       - Store operator and identifier
       - Mark as prefix or postfix
    3. For unary operators on expressions:
       - Save operator
       - Advance
       - Parse expression/factor recursively
       - Create unary expression node
    4. Return unary expression node
    
    Currently called from parse_stmt for x++ and x-- cases
    Also needs to be called from parse_factor for -expr and !expr
    */

}

static ASTNode *parse_binary_expr(Parser *parser) {
    /*
    PROBABLY DON'T NEED THIS FUNCTION
    
    Binary expressions are already handled by parse_expr and parse_term.
    This might have been a planning mistake - you can likely delete this.
    
    Unless you want to use it as a helper function that parse_expr and 
    parse_term both call with different operator sets?
    
    Either implement as a generic helper OR delete it entirely.
    */

}

static ASTNode* parse_term(Parser *parser) {
    /*
    Parses terms with * and / operators (higher precedence than +/-)
    Handles: <factor> (('*' | '/') <factor>)*
    
    TODO:
    1. Parse first factor with parse_factor()
    2. If no * or / operator, return the factor directly
    3. If * or / found, enter loop:
       - Allocate binary_expr node INSIDE loop
       - Set left = accumulated result, op = operator, right = next factor
       - Update left_factor = binary_expr for chaining
    4. Return the final term expression
    
    This is the SAME pattern as parse_expr, just for different operators
    */

}

static ASTNode* parse_factor(Parser *parser) {
    /*
    Parses the highest precedence elements: numbers, identifiers, parentheses
    Handles: NUMBER | IDENTIFIER | '(' <expr> ')' | <unary_expr>
    
    TODO:
    1. Check current token type with switch/if-else
    2. If NUMBER_TOK:
       - Create AST_NUMBER_LITERAL node (ADD THIS TO ast.h!)
       - Store the integer/float value (need to parse lexeme)
       - Advance parser
       - Return number node
    3. If IDENTIFIER:
       - Create AST_IDENTIFIER node (ADD THIS TO ast.h!)
       - Store identifier name (strdup the lexeme)
       - Advance parser
       - Return identifier node
    4. If LEFT_PAREN:
       - Advance past '('
       - Recursively call parse_expr() to handle the inside
       - Check for RIGHT_PAREN
       - Advance past ')'
       - Return the inner expression
    5. If INC_OP or DEC_OP:
       - Call parse_unary_expr()
    6. Otherwise:
       - Error: unexpected token
    
    CRITICAL: You need to add NUMBER_LITERAL and IDENTIFIER node types to ast.h!
    Without these, you can't represent actual values in your AST.
    */

}

static ASTNode* parse_conditional(Parser *parser) {
    /*
    Parses conditional expressions for if/loop statements
    Handles: <expr> ('==' | '!=' | '<' | '>' | '<=' | '>=') <expr>
    
    TODO:
    1. Parse left side with parse_expr()
    2. Check if current token is a comparison operator:
       - EQ_OP (==)
       - NOT_EQ_OP (!=)
       - LESS_THAN (<)
       - GREATER_THAN (>)
       - LESS_THAN_EQ (<=)
       - GREATER_THAN_EQ (>=)
    3. If comparison operator found:
       - Save the operator lexeme
       - Advance parser
       - Parse right side with parse_expr()
       - Create AST_CONDITIONAL_NODE
       - Set left, op, right
       - Return conditional node
    4. If no comparison operator:
       - Error: expected comparison operator in conditional
    
    This creates nodes like: (x > 5) or (a == b)
    Used by if statements and loop conditions
    */

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


