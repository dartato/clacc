#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include "clacc.h"

bool operator2int(char *token, tok *parsedToken) {
    parsedToken->raw = token;
    if (strcmp(token, "print") == 0) {
        parsedToken->operator = PRINT;
    } else if (strcmp(token, "quit") == 0) {
        parsedToken->operator = QUIT;
    } else if (strcmp(token, "+") == 0) {
        parsedToken->operator = PLUS;
    } else if (strcmp(token, "-") == 0) {
        parsedToken->operator = MINUS;
    } else if (strcmp(token, "*") == 0) {
        parsedToken->operator = MULT;
    } else if (strcmp(token, "/") == 0) {
        parsedToken->operator = DIV;
    } else if (strcmp(token, "%") == 0) {
        parsedToken->operator = MOD;
    } else if (strcmp(token, "**") == 0) {
        parsedToken->operator = POW;
    } else if (strcmp(token, "<") == 0) {
        parsedToken->operator = LT;
    } else if (strcmp(token, "drop") == 0) {
        parsedToken->operator = DROP;
    } else if (strcmp(token, "swap") == 0) {
        parsedToken->operator = SWAP;
    } else if (strcmp(token, "rot") == 0) {
        parsedToken->operator = ROT;
    } else if (strcmp(token, "if") == 0) {
        parsedToken->operator = IF;
    } else if (strcmp(token, "pick") == 0) {
        parsedToken->operator = PICK;
    } else if (strcmp(token, "skip") == 0) {
        parsedToken->operator = SKIP;
    } else if (strcmp(token, ":") == 0) {
        parsedToken->operator = USER_DEFINED;
    } else {
        fprintf(stderr, "Token not recognized: %s\n", token);
        /*return false; */
    }
    return true;
}
bool tokenizeFunction(char *function, tokenList *tokens) {
    char *token = strtok(function, " \n");
    tokenList *currentToken = tokens;
    while (token) {
        char *next;
        long value = strtol(token, &next, 10);
        tok *parsedToken = malloc(sizeof(tok));

        if ((next == token) || (*next != '\0')) {
            if (!operator2int(token, parsedToken)) {
                fprintf(stderr, "Error parsing token %s", token);
                return false;
            }
            currentToken->next = malloc(sizeof(tokenList));
            currentToken->next->token = parsedToken;
            currentToken = currentToken->next;
        } else {
            parsedToken->operator = INT;
            parsedToken->i = (int32_t)value;
            currentToken->next = malloc(sizeof(tokenList));
            currentToken->next->token = parsedToken;
            currentToken = currentToken->next;
        }
        token = strtok(NULL, " \n");
    }
    return true;
}

bool splitFile(char *buffer, clac_file *output) {
    char *token = strtok(buffer, ";");
    list *currentFunction = output->functions;
    while (token) {
        currentFunction->next = malloc(sizeof(list));
        currentFunction = currentFunction->next;
        currentFunction->raw = token;
        output->functionCount++;
        token = strtok(NULL, ";");
    }
    currentFunction = output->functions;
    while (currentFunction->next != NULL) {
        tokenList *functionTokens = malloc(sizeof(tokenList));
        if (tokenizeFunction(currentFunction->next->raw, functionTokens)) {
            currentFunction->next->tokens = functionTokens;
            currentFunction = currentFunction->next;
        } else {
            fprintf(stderr, "Error tokenizing %s", token);
            return false;
        }
    }
    return true;
}
bool parse(char *path, clac_file *output) {
    FILE *F = fopen(path, "r");
    char *buffer = 0;
    long length;
    if (F) {
        fseek(F, 0, SEEK_END);
        length = ftell(F);
        fseek(F, 0, SEEK_SET);
        buffer = malloc(length);
        if (buffer)
        {
            fread(buffer, 1, length, F);
        }
        fclose(F);
    } else {
        fprintf(stderr, "Error opening file %s", path);
        return false;
    }

    if (buffer) {
        if (splitFile(buffer, output)) {
            return true;
        } else {
            fprintf(stderr, "Error parsing file.");
            return false;
        }
    } else {
        fprintf(stderr, "Error opening file %s", path);
        return false;
    }
}


int main(int argc, char **argv) {
    clac_file cfile;
    list *currentFunction;
    if (argc < 2) {
        fprintf(stderr, "usage: %s <clac_file> [args...]\n", argv[0]);
        exit(1);
    }
    cfile.functions = malloc(sizeof(list));
    cfile.functionCount = 0;
    if (!parse(argv[1], &cfile)) {
        return 1;
    }
    fprintf(stderr, "Parse successful, read %d functions\n", cfile.functionCount);
    currentFunction = cfile.functions->next;
    for (int i = 0; i < cfile.functionCount; i++) {
        tokenList *currentToken = currentFunction->tokens;
        fprintf(stderr, "Function %d : ", i);
        while (currentToken->next != NULL) {
            fprintf(stderr, "%02x ", currentToken->token->operator);
            currentToken = currentToken->next;
        }
        fprintf(stderr, "\n");
    }
    return 0;
}
