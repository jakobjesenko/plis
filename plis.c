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
    int bracketsOpen = 0;
    if (tokenised[programCounter].opnum != op_argstart){
        return programCounter;
    }
    do {
        if (tokenised[programCounter].opnum == op_argstart){
            bracketsOpen++;
        } else if (tokenised[programCounter].opnum == op_argend){
            bracketsOpen--;
        }
        programCounter++;
    } while (bracketsOpen);
    return programCounter;
}

bool insertASTNode(token tokenised[], int programCounter, astNode* branch, int depth){

    token t = tokenised[programCounter];
    if (t.opnum == op_argend){
        return true;
    }

    branch->opnum = t.opnum;
    if (t.opnum == not_op){
        branch->info = t.info;
        return false;
    }

    programCounter += 2;

    for (int i = 0; i <= MAX_ARGUMENT_COUNT; i++){
        astNode* node = (astNode*)calloc(1, sizeof(astNode));
        branch->child[i] = node;
        programCounter = jumpPC(tokenised, programCounter + i);
        if (insertASTNode(tokenised, programCounter, branch->child[i], depth + 1)){
            return false;
        }
    }
}

void printAST(FILE* fpointer, astNode* node, int depth){
    for (int i = 0; i < depth; i++){
        fprintf(fpointer, "  ");
    }
    if (node->opnum == op_nop || node->opnum == not_op){
        if (!node->info){
            return;
        }
        fprintf(fpointer, "%s  %s\n", keywords[node->opnum], node->info);
        return;
    }

    fprintf(fpointer, "%s\n", keywords[node->opnum]);

    for (int i = 0; i < MAX_ARGUMENT_COUNT; i++){
        if (!node->child[i]){
            break;
        }
        printAST(fpointer, node->child[i], depth + 1);
    }
}

int main(int argc, char const *argv[]) {
    static token tokenised[TOKEN_ARRAY_LENGHT] = {0};

    flagList flags = {0};
    char* tokenOutFilename;
    char* treeOutFilename;
    char* assemblyOutFilename;
    char* codeInFilename = NULL;

    if (argc < 2){
        printf("not enough arguments. use -h flag for help\n");
        return 1;
    }
    for (int i = 1; i < argc; i++){
        if (argv[i][0] == '-'){
            switch (argv[i][1]){
                case 'a':
                    flags.tree = true;
                    if (i < argc - 1 && argv[i + 1][0] != '-'){
                        treeOutFilename = (char*)argv[++i];
                    }
                    break;
                case 't':
                    flags.tokens = true;
                    if (i < argc - 1 && argv[i + 1][0] != '-'){
                        tokenOutFilename = (char*)argv[++i];
                    }
                    break;
                case 's':
                    flags.assembly = true;
                    if (i < argc - 1 && argv[i + 1][0] != '-'){
                        assemblyOutFilename = (char*)argv[++i];
                    }
                    break;
                case 'h':
                    flags.help = true;
                    break;
                default:
                    printf("not recognised flag. use -h flag for help\n");
                    return 1;
                    break;
            }
        } else {
            codeInFilename = (char*)argv[i];
        }
    }

    if (flags.help){
        printf("help needs implemented\n");
        return 0;
    }

    if (!codeInFilename){
        printf("input file missing. use -h flag for help\n");
        return 1;
    }
    lex(codeInFilename, tokenised);
    if (flags.tokens){
        FILE* tokenOutFile = fopen(tokenOutFilename, "w");
        printTokenisedProgram(tokenOutFile, tokenised);
        fclose(tokenOutFile);
    }

    astNode* ast = (astNode*)calloc(1, sizeof(astNode));
    insertASTNode(tokenised, 0, ast, 0);
    if (flags.tree){
        FILE* treeOutFile = fopen(treeOutFilename, "w");
        printAST(treeOutFile, ast, 0);
        fclose(treeOutFile);
    }

    return 0;
}
