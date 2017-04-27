#ifndef _COMP_H_
#define _COMP_H_

#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>
#include <inttypes.h>
#include <stdlib.h>
#include "clacc.h"

enum c0_byte_sizes {
    SPRINT = 11,
    SQUIT  = 3,
    SPLUS  = 1,
    SMINUS = 1,
    SMULT  = 1,
    SDIV   = 1,
    SMOD   = 1,
    SPOW   = 60, /* much big such large wow */
    SLT    = 13,
    SDROP  = 1,
    SSWAP  = 1,
    SROT   = 0x0C,
    SIF    = 0x0D,
    SPICK  = 0x0E,
    SSKIP  = 0x0F,

    SINT   = 3,
    SUNK   = 0x11,
    SUFUNC = 3,
};

int16_t get_bytecode_size(tok *token);
/* returns an integer indicating the number of operands a
 * given clac token will translate to
 * */

int16_t get_arg_amount(tokenList *tokens);
/* returns expected number of stack elements used by a function
 * */

#endif /* _COMP_H_ */
