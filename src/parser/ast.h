#pragma once 
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

typedef enum {

    // Program Nodes 
    AST_PROGRAM_NODE, 
    AST_STMTS_NODE, 

    // Stmt nodes 
    AST_VAR_DECL_NODE,
    AST_ASSIGN_NODE,
    AST_IF_STMT_NODE,
    AST_FOR_LOOP_NODE,
    AST_WHILE_LOOP_NODE,
    AST_PRINT_NODE,
    AST_READ_NODE,
    AST_INC_DEC_NODE,

    // Expression Nodes
    AST_BINARY_EXPR,
    AST_COMPARISON_NODE,
    AST_IDENTIFIER,
    AST_INT_LIT,
    AST_UNARY_EXPR, 

} ASTNodeType;


typedef struct ASTNode {
    ASTNodeType type;

    union {

        // AST_VAR_DECL: let x = 5;
        struct {
            char *identifer;
            struct ASTNode* value;
        } var_decl;

        // BINARY_

    } data;
} ASTNode;

