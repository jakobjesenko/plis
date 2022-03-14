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

// move to next argument from function call
int jumpPC(token tokenised[], int programCounter){
    int bracketsOpen = 0;
    programCounter++; // always op_argstart
    do {
        if (tokenised[programCounter].opnum == op_argstart){
            bracketsOpen++;
        } else if (tokenised[programCounter].opnum == op_argend){
            bracketsOpen--;
        }
        programCounter++;
    } while (bracketsOpen > 0);
    return programCounter - 1; // move PC back to last op_argend
}

void insertASTNode(token tokenised[], int programCounter, astNode* branch){

    token t = tokenised[programCounter];
    if (t.opnum == op_argend){
        return;
    }

    assert(t.opnum != not_op && "unbelonging constant");

    branch->opnum = t.opnum;
    programCounter++; // always op_argstart
    
    for (int i = 0; i <= MAX_ARGUMENT_COUNT; i++){
        programCounter++; // first argument
        token param = tokenised[programCounter];
        if (param.opnum == op_argend){
            return;
        }
        astNode* node = (astNode*)calloc(1, sizeof(astNode));
        if (param.opnum == not_op){
            node->opnum = not_op;
            node->info = param.info;
            branch->child[i] = node;
            continue;
        }
        branch->child[i] = node;
        insertASTNode(tokenised, programCounter, branch->child[i]);
        programCounter = jumpPC(tokenised, programCounter); // PC points to a function
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

void printAsmHead(FILE* fpointer){
    fprintf(fpointer, "format ELF64 executable 3\n");
    fprintf(fpointer, "segment readable executable\n");
    fprintf(fpointer, "entry start\n");
    fprintf(fpointer, "\nstart:\n");
}

void printAsmProgram(FILE* fpointer, astNode* node){
    // TODO rework this bottom condition
    if (node->opnum == op_nop || node->opnum == not_op){
        if (!node->info){
            return;
        }
        fprintf(fpointer, "\tpushq %s\t\t\t\t; %s\n", node->info, keywords[node->opnum]);
        return;
    }

    switch (node->opnum){
        case op_exit:
            fprintf(fpointer, "\tmov rax, 60\t\t\t\t; exit\n");
            fprintf(fpointer, "\tmov rdi, %s\t\t\t\t; |\n", node->child[0]->info);
            fprintf(fpointer, "\tsyscall\t\t\t\t; |\n");
            break;
        case op_testingop:
            break;
        default:
            assert(0 && "not recognised operation");
            break;
    }

    for (int i = 0; i < MAX_ARGUMENT_COUNT; i++){
        if (!node->child[i]){
            break;
        }
        printAsmProgram(fpointer, node->child[i]);
    }
}

int main(int argc, char const *argv[]) {
    assert(sizeof(keywords) / sizeof(char*) == count_op && "keywords counts do not match");

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
                    } else {
                        assert(0 && "filename missing");
                    }
                    break;
                case 't':
                    flags.tokens = true;
                    if (i < argc - 1 && argv[i + 1][0] != '-'){
                        tokenOutFilename = (char*)argv[++i];
                    } else {
                        assert(0 && "filename missing");
                    }
                    break;
                case 's':
                    flags.assembly = true;
                    if (i < argc - 1 && argv[i + 1][0] != '-'){
                        assemblyOutFilename = (char*)argv[++i];
                    } else {
                        assert(0 && "filename missing");
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
    insertASTNode(tokenised, 0, ast);
    if (flags.tree){
        FILE* treeOutFile = fopen(treeOutFilename, "w");
        printAST(treeOutFile, ast, 0);
        fclose(treeOutFile);
    }

    if (!flags.assembly){
        char* tempFilename = "temp.asm";
        assemblyOutFilename = tempFilename;
    }
    FILE* assemblyOutFile = fopen(assemblyOutFilename, "w");
    printAsmHead(assemblyOutFile);
    printAsmProgram(assemblyOutFile, ast);
    fclose(assemblyOutFile);

    char command[256];
    strcpy(command, "fasm ");
    strcat(command, assemblyOutFilename);
    strcat(command, " ");
    strcat(command, "out.elf");
    system(command);

    system("chmod +x out.elf");

    if (!flags.assembly){
        strcpy(command, "rm ");
        strcat(command, assemblyOutFilename);
        system(command);
    }

    return 0;
}
