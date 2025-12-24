/*
This file defines the Abstract Syntax Tree (AST) structure for the Eidos language parser.
It contains:
- ASTNodeType enum: defines all possible node types including program nodes, statements 
    - (variable declarations, assignments, control flow), and expressions (binary operations, 
    - comparisons, literals, identifiers)
- ASTNode struct: a tagged union representing any AST node, with type-specific data for:
    * Program structure (program root, statement lists)
    * Variable operations (declarations, assignments, inc/dec)
    * Control flow (if/else, for loops, while loops)
    * I/O operations (print, read)
    * Expressions (binary operations, comparisons, unary operations, literals)
The AST is built by the syntax parser and used by the semantic analyzer.
*/

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
        // Program Node
        struct {
            struct ASTNode *statements;  // Points to AST_STMTS_NODE
        } program;


        // AST_STMTS_NODE: list of statements
        struct {
            struct ASTNode *stmt;        // current statement
            struct ASTNode *next;        // next statement (linked list), NULL if last
        } stmts;

        // AST_VAR_DECL: let x = 5;
        struct {
            char *identifer;
            struct ASTNode *value;
        } var_decl;

        // AST_ASSIGNMENT_NODE; x = 5;
        struct {
            char *identifier;            // x
            struct ASTNode *value;       // 5
        } assign;


        // AST_IF_STMT_NODE: if (x == 5) { then_block } else { else_block }
        struct {
            struct ASTNode *comparison;     // any <expr> that evaluates to True
            struct ASTNode *then_block;     // block of code to execute after comparison
            struct ASTNode *else_block;     // Else block, NULL if not provided
        } if_stmt;


        // AST_FOR_LOOP_NODE: for (let x = 0; x < 10; x++) { for_block }
        struct {
            struct ASTNode *initializer;    // let x = 0;
            struct ASTNode *comparision;    // x < 10;
            struct ASTNode *step;           // x++
            struct ASTNode *for_block;

        } for_loop;

        // AST_WHILE_LOOP_NODE: while (x < 10) { while_block }
        struct {
            struct ASTNode *conditional;    // x < 10
            struct ASTNode *while_block;    
            
        } while_loop;

        // AST_PRINT_NODE 
        struct {
            struct ASTNode *expression;         // points to an expression to print
        } print_node;

        // AST_READ_NODE
        struct {
            char *identifier;         // points to an identifier to store value
        } read_node;

        // AST_INC_DEC_NODE: x++, --a
        struct {
            char *identifier;       // a
            char *op;               // ++
        } inc_dec_node;

        // AST_BINARY_EXPR let x = a * t;
        struct {
            struct ASTNode *left;       // left operand
            char *op;                   // +, *, /, -, etc
            struct ASTNode *right;      // right operand
        } binary_expr;

    } data;
} ASTNode;

