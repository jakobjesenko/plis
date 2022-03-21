#ifndef PLIS_HEADER
#define PLIS_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <ctype.h>

#define BRACKET_STACK_DEPTH 128
#define TOKEN_ARRAY_LENGTH 256
#define MAX_WORD_LENGTH 32
#define MAX_ARGUMENT_COUNT 8
#define WRITE_BUFFER_LENGTH 128
#define MAX_STRING_COUNT 128

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
    op_add,
    op_sub,
    op_mul,
    op_div,
    op_mod,
    op_parseint,
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
    "add",
    "sub",
    "mul",
    "div",
    "mod",
    "parseint",
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

#endif