#include <inttypes.h>
#include <stdbool.h>

#include "comp.h"
#include "clacc.h"

int32_t get_bytecode_size(tok *token, tokenList *functions[]) {
    switch (token->operator) {
        case PRINT: return SPRINT;
        case QUIT: return SQUIT;
        case PLUS: return SPLUS;
        case MINUS: return SMINUS;
        case MULT: return SMULT;
        case DIV: return SDIV;
        case MOD: return SMOD;
        case POW: return SPOW;
        case LT: return SLT;
        case DROP: return SDROP;
        case SWAP: return SSWAP;
        case ROT: return SROT;
        case IF: return SIF;
        case PICK: return SPICK;
        case INT: return SINT;
        case UFUNC: return SUFUNC;
        default:
            return 0;
    }
}
