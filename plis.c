#include "plis.h"

void lex(char* codeFileName, token tokenised[]){
    FILE* f = fopen(codeFileName, "r");
    char c;
    int col = 1;
    int row = 1;
    char word[MAX_WORD_LENGTH];
    int wordIndex = 0;
    int programCounter = 0;
    char bracketStack[BRACKET_STACK_DEPTH];
    int bracketSP = 0;

    while ((c = fgetc(f)) != EOF){
        switch (c){
            case ' ':
                break;
            case '\t':
                col +=3;
                break;
            case '\n':
                row++;
                col = 0;
                break;
            case '(':
                bracketStack[bracketSP++] = c;
                assert(wordIndex && "keyword missing");
                bool finished = false;
                for (int i = 0; i < count_op; i++){
                    if (!strcmp(word, keywords[i])){
                        tokenised[programCounter++] = (token){row, col - strlen(word), i, NULL};
                        finished = true;
                        break;
                    }
                }
                assert(finished && "unrecognised keyword");
                tokenised[programCounter++] = (token){row, col, op_argstart, NULL};
                wordIndex = 0;
                break;
            case ',':
                assert((wordIndex || tokenised[programCounter - 1].opnum == op_argend) && "illegal keyword: `,`");
                if (!wordIndex){
                    break;
                }
                int toalloc = strlen(word) + 1;
                char* info = (char*)malloc(toalloc * sizeof(char));
                strcpy(info, word);
                tokenised[programCounter++] = (token){row, col - strlen(word), not_op, info};
                wordIndex = 0;
                break;
            case ')':
                assert(bracketSP && bracketStack[bracketSP - 1] == '(' && "brackets do not match");
                bracketStack[bracketSP--] = 0;
                if (wordIndex){
                    int toalloc = strlen(word) + 1;
                    char* info = (char*)malloc(toalloc * sizeof(char));
                    strcpy(info, word);
                    tokenised[programCounter++] = (token){row, col - strlen(word), not_op, info};
                    wordIndex = 0;
                }
                tokenised[programCounter++] = (token){row, col, op_argend, NULL};
                break;
            default:
                word[wordIndex++] = c;
                word[wordIndex] = 0;
                break;
        }
        col++;
    }
    assert(!bracketSP && "not all brackets were closed");
    fclose(f);
}

void printTokenisedProgram(FILE* fpointer, token tokenised[]){
    token t;
    for (int i = 0; i < TOKEN_ARRAY_LENGHT; i++){
        t = tokenised[i];
        if (!t.row){
            return;
        }
        char info[MAX_WORD_LENGTH] = {0};
        if(t.info){
            strcpy(info, t.info);
        }
        fprintf(fpointer, "ln%d:col%d\t%s\t\t%s\n", t.row, t.col, keywords[t.opnum], info);
    }
}

int jumpPC(token tokenised[], int programCounter){
    int bracketsOpen = 1;
    if (programCounter == 0){
        return programCounter;
    }
    do {
        programCounter++;
        if (tokenised[programCounter].opnum == op_argstart){
            bracketsOpen++;
        } else if (tokenised[programCounter].opnum == op_argend){
            bracketsOpen--;
        }
    } while (bracketsOpen);
    return programCounter;
}

// dons ne vem, kako bi to popravu
void insertASTNode(token tokenised[], int programCounter, astNode* branch){
    for (int i = 0; i < MAX_ARGUMENT_COUNT; i++){
        programCounter += i;
        token t = tokenised[programCounter];
        if (!t.row){
            return;
        }
        // astNode* node = (astNode*)malloc(sizeof(astNode));
        astNode* node = (astNode*)calloc(1, sizeof(astNode));
        switch (t.opnum){
            case op_argstart:
                free(node);
                break;
            case op_argend:
                free(node);
                break;
            case op_nop:
                /* assert(tokenised[programCounter + 1].opnum == op_argstart &&
                    tokenised[programCounter + 2].opnum == op_argend &&
                    "wrong use of operation: `nop`"); */
                node->opnum = op_nop;
                branch->child[i] = node;
                break;
            case op_exit:
                /* assert(tokenised[programCounter + 1].opnum == op_argstart &&
                    tokenised[programCounter + 2].opnum == not_op &&
                    tokenised[programCounter + 3].opnum == op_argend &&
                    "wrong use of operation: `exit`"); */
                node->opnum = op_exit;
                branch->child[i] = node;
                insertASTNode(tokenised, programCounter + 2, branch->child[i]);
                break;
            case not_op:
                node->opnum = not_op;
                node->info = t.info;
                branch->child[i] = node;
                programCounter++;
                break;
            default:
                assert(0 && "unknown operation");
                break;
        }
        programCounter = jumpPC(tokenised, programCounter);
    }
}

void printAST(FILE* fpointer, astNode node, int depth){
    for (int i = 0; i < depth; i++){
        fprintf(fpointer, "  ");
    }
    if (node.opnum == op_nop || node.opnum == not_op){
        if (!node.info){
            return;
        }
        fprintf(fpointer, "%s  %s\n", keywords[node.opnum], node.info);
        return;
    }

    fprintf(fpointer, "%s\n", keywords[node.opnum]);

    for (int i = 0; i < MAX_ARGUMENT_COUNT; i++){
        if (!node.child[i]){
            break;
        }
        printAST(fpointer, *node.child[i], depth + 1);
    }
}

int main(int argc, char const *argv[]) {
    static token tokenised[TOKEN_ARRAY_LENGHT] = {0};

    lex("examples/test0.plis", tokenised);
    printTokenisedProgram(stdout, tokenised);

    astNode* ast = (astNode*)calloc(1, sizeof(astNode));
    ast->opnum = op_entry;
    insertASTNode(tokenised, 0, ast);
    
    printf("\n\n\n");

    printAST(stdout, *ast, 0);

    printf("\n\n\n");

    return 0;
}
