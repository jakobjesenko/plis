#ifndef PLIS_HEADER
#define PLIS_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <ctype.h>

// compiler constants
#define BRACKET_STACK_DEPTH 128
#define TOKEN_ARRAY_LENGTH 256
#define MAX_WORD_LENGTH 32
#define MAX_ARGUMENT_COUNT 8
#define MAX_STRING_COUNT 128

// program constants
#define WRITE_BUFFER_LENGTH 128
#define MAX_ALLOCATED_NUMBERS 64
#define ALLOCATED_GP_MEMORY 256

typedef enum {
    op_nop,
    op_exit,
    op_putc,
    op_getc,
    op_prints,
    op_reads,
    op_chain,
    op_bitand,
    op_bitor,
    op_bitnot,
    op_shiftl,
    op_shiftr,
    op_shiftla,
    op_shiftra,
    op_add,
    op_sub,
    op_mul,
    op_div,
    op_mod,
    op_not,
    op_and,
    op_or,
    op_eq,
    op_neq,
    op_lt,
    op_gt,
    op_le,
    op_ge,
    op_if,
    op_while,
    op_memset,
    op_memget,
    op_parseint,
    op_inttostr,
    op_testingop,
    op_argstart,
    op_argend,
    not_op,
    count_op
} ops;

char* keywords[] = {
    "nop",
    "exit",
    "putc",
    "getc",
    "prints",
    "reads",
    "chain",
    "bitand",
    "bitor",
    "bitnot",
    "shiftl",
    "shiftr",
    "shiftla",
    "shiftra",
    "add",
    "sub",
    "mul",
    "div",
    "mod",
    "!",
    "&&",
    "||",
    "==",
    "!=",
    "<",
    ">",
    "<=",
    ">=",
    "if",
    "while",
    "memset",
    "memget",
    "parseint",
    "inttostr",
    "testingop",
    "(",
    ")",
    "notop__",
};

typedef struct astNode {
    ops opnum;
    char* info;
    struct astNode* child[MAX_ARGUMENT_COUNT];
} astNode;

typedef struct {
    int row;
    int col;
    ops opnum;
    char* info;
} token;

typedef struct {
    bool help;
    bool tokens;
    bool tree;
    bool assembly;
    bool nooutbuffer;
} flagList;

static bool buffered_write = true;
static int putc_calls_count = 0;
static int string_variable_count = 0;
static int empty_number_count = 0;

#endif