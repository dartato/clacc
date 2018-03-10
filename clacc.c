#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lib/xalloc.h"
#include "lib/hdict.h"
#include "clacc.h"
#include "parse.h"
#include "comp.h"

#include "lib/c0vm.h"
#define LARGEST_OPERATION_SIZE 180
#define BYTECODE_VERSION 9
#define NATIVE_COUNT 2
#define NATIVE_PRINTINT 0x00010009
#define NATIVE_PRINTLN 0x0001000A

int32_t functionLength(tokenList *function) {
    /* gives a worst case maximum size, in bytes, of associated bytecode of a clac function */

    tokenList *currentNode = function;
    int32_t i = 0;

    while (currentNode != NULL) {
        i++;
        currentNode = currentNode->next;
    }

    return i * LARGEST_OPERATION_SIZE;
}

int32_t totalsize(tokenList *functions[], int functionCount) {
    int32_t total = 0;

    for (int i = 1; i <= functionCount; i++) {

        total = total + functions[i]->token->i;
    }

    return total;
}
bool token2c0(tok *token, int *i, size_t intCount, int32_t *ints, bool *prints, char *buffer, tokenList *functions[], char *raw) {
    switch (token->operator) {

    case PRINT: {
        *prints = true;
        sprintf(raw, "%02x 00 00\n%02x\n%02x 00 00\n%02x 00 01\n%02x\n", INVOKENATIVE, POP, ALDC, INVOKENATIVE, POP);
        /* need to somehow note to put '\0' into the string pool */
        strcpy(buffer, raw);
        *i = *i + 3*SPRINT;
        break;
    }

    case QUIT: {
        sprintf(raw, "%02x 00\n%02x\n", ALDC, ATHROW); /* simplest way to exit program
                                                          is to throw an error...
                                                          I'll fix this eventually */
        strcpy(buffer, raw);
        *i = *i + 3*SQUIT;
        break;
    }

    case PLUS: {
        sprintf(raw, "%02x\n", IADD);
        strcpy(buffer, raw);
        *i = *i + 3*SPLUS;
        break;
    }

    case MINUS: {
        sprintf(raw, "%02x\n", ISUB);
        strcpy(buffer, raw);
        *i = *i + 3*SMINUS;
        break;
    }

    case MULT: {
        sprintf(raw, "%02x\n", IMUL);
        strcpy(buffer, raw);
        *i = *i + 3*SMULT;
        break;
    }

    case DIV: {
        sprintf(raw, "%02x\n", IDIV);
        strcpy(buffer, raw);
        *i = *i + 3*SDIV;
        break;
    }

    case MOD: {
        sprintf(raw, "%02x\n", IREM);
        strcpy(buffer, raw);
        *i = *i + 3*SMOD;
        break;
    }

    case POW: { /*this one is a doozy*/
        sprintf(raw,
"%02x %02x\n\
%02x %02x\n\
%02x %02x\n\
%02x %02x\n\
%02x %02x\n\
%02x %02x\n\
%02x %02x %02x\n\
%02x %02x %02x\n\
%02x %02x\n\
%02x %02x\n\
%02x\n\
%02x %02x\n\
%02x %02x %02x\n\
%02x %02x %02x\n\
%02x %02x\n\
%02x %02x\n\
%02x\n\
%02x %02x\n\
%02x %02x %02x\n\
%02x %02x\n\
%02x %02x\n\
%02x\n\
%02x %02x\n\
%02x %02x\n\
%02x %02x\n\
%02x\n\
%02x %02x\n\
%02x %02x %02x\n\
%02x %02x\n",
                VSTORE, 1,
                VSTORE, 0,
                BIPUSH, 1,
                VSTORE, 2,
                VLOAD, 1,
                BIPUSH, 0,
                IF_ICMPGT, 0, 6,
                GOTO, 0, 0x2B,
                VLOAD, 1,
                BIPUSH, 2,
                IREM,
                BIPUSH, 1,
                IF_CMPEQ, 0, 6,
                GOTO, 0, 0x0D,
                VLOAD, 0,
                VLOAD, 2,
                IMUL,
                VSTORE, 2,
                GOTO, 0, 3,
                VLOAD, 0,
                VLOAD, 0,
                IMUL,
                VSTORE, 0,
                VLOAD, 1,
                BIPUSH, 2,
                IDIV,
                VSTORE, 1,
                GOTO, 0xFF, 0xD1,
                VLOAD, 2);
        strcpy(buffer, raw);
        *i = *i + (3*SPOW);
        break;
    }

    case LT: {
        sprintf(raw, "%02x %02x %02x\n%02x %02x %02x\n%02x %02x\n%02x %02x %02x\n%02x %02x\n",
                IF_ICMPLT, 0x00, 0x06,
                GOTO, 0x00, 0x08,
                BIPUSH, 0x01,
                GOTO, 0x00, 0x05,
                BIPUSH, 0x00);
        strcpy(buffer, raw);
        *i = *i + (3*SLT);
        break;
    }

    case DROP: {
        sprintf(raw, "%02x\n", POP);
        strcpy(buffer, raw);
        *i = *i + SDROP;
        break;
    }

    case SWAP: {
        sprintf(raw, "%02x\n", SWAP);
        strcpy(buffer, raw);
        *i = *i + SSWAP;
        break;
    }

    case ROT: break;

    case IF: break;

    case PICK: break;

    case SKIP: {
                   /*
        sprintf(raw, "%02x\n",
        strcpy(buffer, raw);
        *i = *i + 3;*/
    }

    case INT: {
        int32_t newint = token->i;
        if ((newint < 127) && (newint > -127)) {
            sprintf(raw, "%02x %02x %02x\n", BIPUSH, newint, NOP); /* NOP is there to make
                                                                      the line the same length as
                                                                      an ildc call */
            strcpy(buffer, raw);
            *i = *i + 3*SINT;
        } else {
            size_t firstopen = intCount;
            size_t j = 0;
            for (j = 0; j < intCount; j++) {
                if (ints[j] == newint) {
                    firstopen = j;
                    j = intCount + 1;
                    break;
                }
                if ((ints[j] == 0) && (j < firstopen)) {
                    firstopen = j;
                }
            }
            if (j <= intCount) {
                ints[firstopen] = newint;
        }
            sprintf(raw, "%02x 00 %02x\n", ILDC, (int)firstopen);
            strcpy(buffer, raw);
            *i = *i + 3*SINT;
        }
        break;
    }

    case UFUNC: {
        int32_t funcIndex = token->i;
        sprintf(raw, "%02x %02x %02x\n", INVOKENATIVE,
                                        *((ubyte*)(&i)+1),
                                        *((ubyte*)(&i)+0));
        strcpy(buffer, raw);
        *i = *i + 3*SUFUNC;
        functions[funcIndex]->token->operator = USED;
        break;
    }

    default:
        fprintf(stderr, "Opcode not recognized: 0x%02x\n", token->operator);
        return false;
    }

    return true;
}
bool convert2c0(tokenList *functions[], tokenList *mainFunction, size_t int_pool, int32_t *ints, char *buffer) {
    /* the first 4 bytes say where the buffer ends. (the first 5 bytes are reserved) */
    char raw[LARGEST_OPERATION_SIZE];
    bool prints = false;
    int i = *((uint32_t*)buffer);

    tokenList *currentToken = mainFunction->next;
    while (currentToken != NULL) {
        if ((currentToken->token->operator == UFUNC ) && (functions[currentToken->token->i]->token->operator == UNUSED)) {
            fprintf(stderr, "Error compiling: unrecognized token in function %d.\n", currentToken->token->i);
            return false;
        }
        if (!token2c0(currentToken->token, &i, int_pool, ints, &prints, &buffer[i], functions, raw)) {
            return false;
        }
        currentToken = currentToken->next;
    }
    fprintf(stderr, "%d bytes of buffer used\n", i);
    fprintf(stderr, "%d bytes of bytecode generated\n", i / 3);
    sprintf(raw, "%02x\n", RETURN);
    strcpy(buffer+i, raw);
    *((uint32_t*)buffer) = i+3;
    return true;
}

bool generate_file(char *buffer, char *header, size_t int_pool, int32_t *ints, int functionCount, uint32_t bufferSize) {
    strcpy(header   , "C0 C0 FF EE\n"); /* copy 12 bytes (magic number)*/
    strcpy(header+12, "00 13\n"); /* copy 6 bytes (version 9, arch =1) */
    char twbyte[12];
    /* There goes endianness... */
    sprintf(twbyte, "%02x %02x\n", *((char*)&int_pool+1), *((char*)&int_pool)); /* number of ints in int_pool */
    strcpy(header+18, twbyte); /* copy over the 6 bytes (int_pool) */
    size_t i;
    for (i = 0; i < int_pool; i++) {
        sprintf(twbyte, "%02x %02x %02x %02x\n",
                *((ubyte*)(ints+i)+3),
                *((ubyte*)(ints+i)+2),
                *((ubyte*)(ints+i)+1),
                *((ubyte*)(ints+i)+0));
        strcpy(header+24+(12*i), twbyte);
    }
    i = 24+ (12*i);
    strcpy(header + i, "00 01\n00\n\n");
    sprintf(twbyte, "%02x %02x\n",
            *((ubyte*)&functionCount+1),
            *((ubyte*)&functionCount+0));
    strcpy(header + i + 10, twbyte);
    strcpy(header + i + 16, "\n00 00\n00 03\n");
    uint32_t bytelen = (bufferSize - 5) / 3;
    sprintf(twbyte, "%02x %02x\n",
            *((ubyte*)&bytelen+1),
            *((ubyte*)&bytelen+0));
    strcpy(header + i + 29, twbyte);
    strcpy(header + i + 35, buffer+5);
    strcpy(header + i + 30 + bufferSize, "\n00 02\n\n00 01 00 09\n00 01 00 0A\n# Generated by clacc\n");
    return true;
}

bool doneCompiling(tokenList *functions[], int functionCount) {
    for (int i = 1; i < functionCount; i++) {
        if ((functions[i]->token->operator == USED) && (functions[i]->compiledYet == false)) {
            return false;
        }
    }
    return true;
}

bool build_bytecode(clac_file *cfile) {
    tokenList *functions[cfile->functionCount+1];
    list *currentFunction = cfile->functions->next;
    size_t int_pool = 0;
    for (int i = 1; i <= cfile->functionCount; i++) {
        functions[i] = currentFunction->tokens;
        functions[i]->token = xmalloc(sizeof(tok));
        functions[i]->token->operator = USED;
        functions[i]->token->i = functionLength(functions[i]);
        functions[i]->compiledYet = false;
        tokenList *currentToken = functions[i];
        while (currentToken != NULL) {
            if (currentToken->token->operator == UNK) {
                fprintf(stderr, "Unknown token '%s' in function %d\n", currentToken->token->raw, i);
                functions[i]->token->operator = UNUSED;
            }
            if (currentToken->token->operator == INT) {
                int32_t val = currentToken->token->i;
                if (!(-127 < val && val < 127)) {
                    int_pool++;
                }
            }
            currentToken = currentToken->next;
        }

        currentFunction = currentFunction->next;
    }
    int32_t ints[int_pool];
    for (size_t i = 0; i < int_pool; i++) {
        ints[i] = 0;
    }

    fprintf(stderr,"Int pool size: %d\n", (int)int_pool);
    int bufsize = totalsize(functions, cfile->functionCount);
    fprintf(stderr,"buffer size: %d\n", bufsize);
    char *buffer = xmalloc(bufsize);
    *((uint32_t*)buffer) = 5;
    if (convert2c0(functions, cfile->mainFunction, int_pool, ints, buffer)) {
        while (!doneCompiling(functions, cfile->functionCount)) {
            for (int i = 1; i < cfile->functionCount; i++) {
                if ((functions[i]->token->operator == USED) && (functions[i]->compiledYet == false)) {
                    functions[i]->c0_bytecode = xmalloc(functions[i]->token->i);
                    *((uint32_t*)(functions[i]->c0_bytecode)) = 5;
                    convert2c0(functions, functions[i], int_pool, ints, functions[i]->c0_bytecode);
                }
            }
        }
        char *header = xmalloc(91 + (12*int_pool) + *((uint32_t*)buffer));
        /* 91 is constant, 12 bytes for each int, 1 bytes for each byte used in buffer */
        if (generate_file(buffer, header, int_pool, ints, cfile->functionCount, *((uint32_t*)buffer))) {
            fprintf (stderr, "Success.\n");
            printf("%s", header);
        }
        fprintf(stderr, "%s\n",buffer);
    } else {
        return false;
    }
    return true;
}


int main(int argc, char **argv) {
    clac_file cfile;
    list *currentFunction;
    if (argc < 2) {
        fprintf(stderr, "usage: %s <clac_file> [args...]\n", argv[0]);
        exit(1);
    }
    cfile.functions = xmalloc(sizeof(list));
    cfile.functionCount = 0;
    if (!parse(argv[1], &cfile)) {
        return 1;
    }
    fprintf(stderr, "Parse successful.\n");
    currentFunction = cfile.functions->next;
    for (int i = 1; i <= cfile.functionCount; i++) {
        tokenList *currentToken = currentFunction->tokens->next;
        fprintf(stderr, "Function %d : ", i);
        while (currentToken->next != NULL) {
            fprintf(stderr, "'%02x' ", currentToken->token->operator);
            currentToken = currentToken->next;
        }
        currentFunction = currentFunction->next;
        fprintf(stderr, "\n");
    }
    build_bytecode(&cfile);
    return 0;
}
