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
    bool insideChar = false;
    bool insideString = false;

    while ((c = fgetc(f)) != EOF){
        if (c == '\'' && insideChar){
            insideChar = false;
        }
        insideString = (c == '\"') != insideString;
        if (insideChar || insideString){
            goto skipstuff;
        }
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
                bool found = false;
                for (int i = 0; i < count_op; i++){
                    if (!strcmp(word, keywords[i])){
                        char* info = (char*)malloc(sizeof(char));
                        info[0] = '1'; // proxy info to differentiate nop from empty token
                        tokenised[programCounter++] = (token){row, col - strlen(word), i, info};
                        found = true;
                        break;
                    }
                }
                assert(found && "unrecognised keyword");
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
            skipstuff:
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
    for (int i = 0; i < TOKEN_ARRAY_LENGTH; i++){
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

// move to next argument of function call
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
        switch (t.opnum){
            case op_nop:
                break;
            case op_exit:
                assert(i < 2 && "exit only takes 1 argument");
                break;
            case op_putc:
                assert(i < 2 && "putc only takes 1 argument");
                break;
            case op_getc:
                break;
            case op_prints:
                assert(i < 2 && "prints only takes 1 argument");
                break;
            case op_reads:
                break;
            case op_chain:
                assert(i < 3 && "chain only takes 2 arguments");
                break;
            case op_bitand:
                assert(i < 3 && "bitand only takes 2 arguments");
                break;
            case op_bitor:
                assert(i < 3 && "bitor only takes 2 arguments");
                break;
            case op_bitnot:
                assert(i < 2 && "bitnot only takes 1 argument");
                break;
            case op_add:
                assert(i < 3 && "add only takes 2 arguments");
                break;
            case op_sub:
                assert(i < 3 && "sub only takes 2 arguments");
                break;
            case op_mul:
                assert(i < 3 && "mul only takes 2 arguments");
                break;
            case op_parseint:
                assert(i < 2 && "parseint only takes 1 argument");
                break;
            case op_testingop:
                break;
            default:
                assert(0 && "not recognised operation");
        }
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

    if (node->opnum == not_op){
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

bool isNumber(char* x){
    while (*x){
        if (!isdigit(*x)){
            return false;
        }
        x++;
    }
    return true;
}

char parseChar(char* str){
    switch (str[2]){
        case 'a':
            return '\a';
        case 'b':
            return '\b';
        case 'e':
            return '\e';
        case 'f':
            return '\f';
        case 'n':
            return '\n';
        case 'r':
            return '\r';
        case 't':
            return '\t';
        case 'v':
            return '\v';
        case '\\':
            return '\\';
        case '\'':
            return '\'';
        case '\"':
            return '\"';
        default:
            return '\?';
    }
}

void printAsmProgram(FILE* fpointer, astNode* node, char* stringVariables[]){
    if (node->opnum == not_op){
        if (isNumber(node->info)){
            fprintf(fpointer, "\tpushq %s\t\t\t\t; params push\n", node->info);
        } else if (node[0].info[0] == '\''){
            if (node->info[1] == '\\' && strlen(node->info) == 4) {
                fprintf(fpointer, "\tpushq %d\t\t\t\t; params push\n", (int)parseChar(node->info));
            } else {
                fprintf(fpointer, "\tpushq %d\t\t\t\t; params push\n", (int)node->info[1]);
            }
        } else {
            //fprintf(fpointer, "\tpushq %d\t\t\t\t; string length\n", (int)strlen(node->info));
            fprintf(fpointer, "\tpushq stringvar%d\t\t\t\t; string var def\n", string_variable_count);
            stringVariables[string_variable_count++] = node->info;
        }
        return;
    }

    for (int i = 0; i < MAX_ARGUMENT_COUNT; i++){
        if (!node->child[i]){
            break;
        }
        printAsmProgram(fpointer, node->child[i], stringVariables);
    }
    switch (node->opnum){
        case op_nop:
            fprintf(fpointer, "\tmov rax, rax\t\t\t\t; nop\n");
            break;
        case op_exit:
            fprintf(fpointer, "\tmov rax, 60\t\t\t\t; exit\n");
            fprintf(fpointer, "\tpopq rdi\t\t\t\t; |\n");
            fprintf(fpointer, "\tsyscall\t\t\t\t; |\n");
            break;
        case op_putc:
            if (buffered_write){
                fprintf(fpointer, "\tpopq rax\t\t\t\t; putc\n");
                fprintf(fpointer, "\tlea rdi, [writebuffer]\t\t\t\t; |\n");
                fprintf(fpointer, "\tadd rdi, [bufferpointer]\t\t\t\t; |\n");
                fprintf(fpointer, "\tadd [bufferpointer], 1\t\t\t\t; |\n");
                fprintf(fpointer, "\tmov [rdi], rax\t\t\t\t; |\n");
                fprintf(fpointer, "\tcmp rax, 10\t\t\t\t; |\n");
                fprintf(fpointer, "\tjne putlabel%d\t\t\t\t; |\n", putc_calls_count);
                fprintf(fpointer, "\tmov rax, 1\t\t\t\t; |\n");
                fprintf(fpointer, "\tmov rdi, 1\t\t\t\t; |\n");
                fprintf(fpointer, "\tlea rsi, [writebuffer]\t\t\t\t; |\n");
                fprintf(fpointer, "\tmov rdx, [bufferpointer]\t\t\t\t; |\n");
                fprintf(fpointer, "\tsyscall\t\t\t\t; |\n");
                fprintf(fpointer, "\tmov [bufferpointer], 0\t\t\t\t; |\n"); // reset writebuffer
                fprintf(fpointer, "putlabel%d:\t\t\t\t; |\n", putc_calls_count++);
            } else {
                fprintf(fpointer, "\tmov rax, 1\t\t\t\t; putc\n");
                fprintf(fpointer, "\tmov rdi, 1\t\t\t\t; |\n");
                fprintf(fpointer, "\tmov rsi, rsp\t\t\t\t; |\n");
                fprintf(fpointer, "\tmov rdx, 1\t\t\t\t; |\n");
                fprintf(fpointer, "\tsyscall\t\t\t\t; |\n");
                fprintf(fpointer, "\tadd rsp, 8\t\t\t\t; |\n");
            }
            break;
        case op_getc:
            fprintf(fpointer, "\tsub rsp, 8\t\t\t\t; getc\n");
            fprintf(fpointer, "\tmov rax, 0\t\t\t\t; |\n");
            fprintf(fpointer, "\tmov rdi, 0\t\t\t\t; |\n");
            fprintf(fpointer, "\tmov rsi, rsp\t\t\t\t; |\n");
            fprintf(fpointer, "\tmov rdx, 1\t\t\t\t; |\n");
            fprintf(fpointer, "\tsyscall\t\t\t\t; |\n");
            break;
        case op_prints:
            fprintf(fpointer, "\tcmp [bufferpointer], 0\t\t\t\t; prints\n"); // flush stdout if needed
            fprintf(fpointer, "\tje putlabel%d\t\t\t\t; |\n", putc_calls_count);
            fprintf(fpointer, "\tmov rax, 1\t\t\t\t; |\n");
            fprintf(fpointer, "\tmov rdi, 1\t\t\t\t; |\n");
            fprintf(fpointer, "\tlea rsi, [writebuffer]\t\t\t\t; |\n");
            fprintf(fpointer, "\tmov rdx, [bufferpointer]\t\t\t\t; |\n");
            fprintf(fpointer, "\tsyscall\t\t\t\t; |\n");
            fprintf(fpointer, "\tmov [bufferpointer], 0\t\t\t\t; |\n"); // reset writebuffer
            fprintf(fpointer, "putlabel%d:\t\t\t\t; |\n", putc_calls_count++);
            fprintf(fpointer, "\tmov rax, 1\t\t\t\t; \n");
            fprintf(fpointer, "\tmov rdi, 1\t\t\t\t; |\n");
            fprintf(fpointer, "\tpopq rsi\t\t\t\t; |\n");
            fprintf(fpointer, "\tmov rdx, 0\t\t\t\t; |\n");
            fprintf(fpointer, "\tmov rcx, rsi\t\t\t\t; |\n");
            fprintf(fpointer, "putlabel%d:\t\t\t\t; |\n", putc_calls_count);
            fprintf(fpointer, "\tadd rdx, 1\t\t\t\t; |\n");
            fprintf(fpointer, "\tadd rcx, 1\t\t\t\t; |\n");
            fprintf(fpointer, "\tcmp byte [rcx], 0\t\t\t\t; |\n");
            fprintf(fpointer, "\tjne putlabel%d\t\t\t\t; |\n", putc_calls_count++);
            fprintf(fpointer, "\tsyscall\t\t\t\t; |\n");
            break;
        case op_reads:
            // TODO read arbitrary number of characters, how?
            break;
        case op_chain:
            break;
        case op_bitand:
            fprintf(fpointer, "\tpopq rax\t\t\t\t; bitand\n");
            fprintf(fpointer, "\tpopq rdi\t\t\t\t; |\n");
            fprintf(fpointer, "\tand rdi, rax\t\t\t\t; |\n");
            fprintf(fpointer, "\tpushq rdi\t\t\t\t; |\n");
            break;
        case op_bitor:
            fprintf(fpointer, "\tpopq rax\t\t\t\t; bitor\n");
            fprintf(fpointer, "\tpopq rdi\t\t\t\t; |\n");
            fprintf(fpointer, "\tor rdi, rax\t\t\t\t; |\n");
            fprintf(fpointer, "\tpushq rdi\t\t\t\t; |\n");
            break;
        case op_bitnot:
            fprintf(fpointer, "\tpopq rax\t\t\t\t; bitnot\n");
            fprintf(fpointer, "\tnot rax\t\t\t\t; |\n");
            fprintf(fpointer, "\tpushq rax\t\t\t\t; |\n");
            break;
        case op_add:
            fprintf(fpointer, "\tpopq rax\t\t\t\t; add\n");
            fprintf(fpointer, "\tpopq rdi\t\t\t\t; |\n");
            fprintf(fpointer, "\tadd rdi, rax\t\t\t\t; |\n");
            fprintf(fpointer, "\tpushq rdi\t\t\t\t; |\n");
            break;
        case op_sub:
            fprintf(fpointer, "\tpopq rax\t\t\t\t; sub\n");
            fprintf(fpointer, "\tpopq rdi\t\t\t\t; |\n");
            fprintf(fpointer, "\tsub rdi, rax\t\t\t\t; |\n");
            fprintf(fpointer, "\tpushq rdi\t\t\t\t; |\n");
            break;
        case op_mul:
            fprintf(fpointer, "\tpopq rax\t\t\t\t; mul\n");
            fprintf(fpointer, "\tpopq rdi\t\t\t\t; |\n");
            fprintf(fpointer, "\timul rdi, rax\t\t\t\t; |\n");
            fprintf(fpointer, "\tpushq rdi\t\t\t\t; |\n");
            break;
        case op_parseint:
            fprintf(fpointer, "\tpopq rsi\t\t\t\t; parseint\n");
            fprintf(fpointer, "\tmov rdi, rsi\t\t\t\t; |\n");
            fprintf(fpointer, "putlabel%d:\t\t\t\t; |\n", putc_calls_count);
            fprintf(fpointer, "\tadd rdi, 1\t\t\t\t; |\n");
            fprintf(fpointer, "\tcmp byte [rdi], 0\t\t\t\t; |\n");
            fprintf(fpointer, "\tjne putlabel%d\t\t\t\t; |\n", putc_calls_count++);
            fprintf(fpointer, "\tmov rax, 0\t\t\t\t; |\n");
            fprintf(fpointer, "\tmov rdx, 0\t\t\t\t; |\n"); // exponent
            fprintf(fpointer, "putlabel%d:\t\t\t\t; |\n", putc_calls_count);
            fprintf(fpointer, "\tsub rdi, 1\t\t\t\t; |\n");
            fprintf(fpointer, "\tmov rbx, [rdi]\t\t\t\t; |\n");
            fprintf(fpointer, "\tsub rbx, 48\t\t\t\t; |\n");
            fprintf(fpointer, "\tadd rax, rbx\t\t\t\t; |\n"); // TODO self evident
            fprintf(fpointer, "\tadd rdx, 1\t\t\t\t; |\n");
            fprintf(fpointer, "\tcmp rsi, rdi\t\t\t\t; |\n");
            fprintf(fpointer, "\tjne putlabel%d\t\t\t\t; |\n", putc_calls_count++);
            fprintf(fpointer, "\tpushq rax\t\t\t\t; |\n");
            break;
        case op_testingop:
            break;
        default:
            assert(0 && "not recognised operation");
            break;
    }
}

void printAsmExit(FILE* fpointer){
    fprintf(fpointer, "\tmov rax, 60\n");
    fprintf(fpointer, "\tmov rdi, 0\n");
    fprintf(fpointer, "\tsyscall\n");
}

void printAsmFooter(FILE* fpointer, char* stringVariables[]){
    fprintf(fpointer, "segment readable writable\n");
    fprintf(fpointer, "bufferpointer dq 0\n");
    fprintf(fpointer, "writebuffer rb %d\n", WRITE_BUFFER_LENGTH);
    for (int i  = 0; i < string_variable_count; i++){
        fprintf(fpointer, "stringvar%d db %s, 0\n", i, stringVariables[i]);
    }
}

int main(int argc, char const *argv[]) {
    assert(sizeof(keywords) / sizeof(char*) == count_op && "keywords counts do not match");

    static token tokenised[TOKEN_ARRAY_LENGTH] = {0};
    static char* stringVariables[MAX_STRING_COUNT];

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
                case 'n':
                    flags.nooutbuffer = true;
                    buffered_write = false;
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
        printf("help needs implemented\n\tflags:[-a, -h, -n, -s, -t]\n");
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
    printAsmProgram(assemblyOutFile, ast, stringVariables);
    printAsmExit(assemblyOutFile);
    printAsmFooter(assemblyOutFile, stringVariables);
    fclose(assemblyOutFile);

    char command[256];
    strcpy(command, "./asmreformat.py ");
    strcat(command, assemblyOutFilename);
    system(command);
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
