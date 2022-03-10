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

typedef enum {
    op_nop,
    op_exit,
    op_argstart,
    op_argend,
    not_op,
    count_op
} ops;

char* keywords[] = {
    "nop",
    "exit",
    "(",
    ")",
    "_notop__"
};

/* typedef union {
    char string[16];
    int integer;
    char character;
} parameter; */

typedef struct {
    int row;
    int col;
    ops opnum;
    char* info;
} token;

#endif