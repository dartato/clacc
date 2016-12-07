#ifndef _CLACC_H_
#define _CLACC_H_

#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>
#include <inttypes.h>
#include <stdlib.h>

typedef uint8_t ubyte;

typedef struct tok {
    int32_t i;
    ubyte operator;
    char *raw;
} tok;

typedef struct stringNode tokenList;
struct stringNode {
    struct stringNode *next;
    tok *token;
};

typedef struct node list;
struct node {
    struct node *next;
    tokenList *tokens;
    char *raw;
};

typedef struct clac_file_header clac_file;
struct clac_file_header {
    list *functions;
    int functionCount;
};
enum instructions {
    PRINT = 0x01,
    QUIT  = 0x02,
    PLUS  = 0x03,
    MINUS = 0x04,
    MULT  = 0x05,
    DIV   = 0x06,
    MOD   = 0x07,
    POW   = 0x08,
    LT    = 0x09,
    DROP  = 0x0A,
    SWAP  = 0x0B,
    ROT   = 0x0C,
    IF    = 0x0D,
    PICK  = 0x0E,
    SKIP  = 0x0F,

    INT   = 0x10,
    UNK   = 0x11,
    UFUNC = 0x12,
    USER_DEFINED = 0xFF
};

#endif /* _CLACC_H_ */
