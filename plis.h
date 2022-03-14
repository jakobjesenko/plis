#ifndef PLIS_HEADER
#define PLIS_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#define BRACKET_STACK_DEPTH 128
#define TOKEN_ARRAY_LENGHT 256
#define MAX_WORD_LENGTH 32
#define MAX_ARGUMENT_COUNT 8

typedef enum {
    op_nop,
    op_exit,
    op_testingop,
    op_argstart,
    op_argend,
    not_op,
    count_op
} ops;

char* keywords[] = {
    "nop",
    "exit",
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
} flagList;

#endif