#include "plis.h"


int lex(char* codeFileName, token tokenised[]){
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
        insideChar = (c == '\'') != insideChar;
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
    return programCounter;
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
        global_program_counter = programCounter + 1;
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
            case op_shiftl:
                assert(i < 3 && "shiftl only takes 2 arguments");
                break;
            case op_shiftr:
                assert(i < 3 && "shiftr only takes 2 arguments");
                break;
            case op_shiftla:
                assert(i < 3 && "shiftla only takes 2 arguments");
                break;
            case op_shiftra:
                assert(i < 3 && "bitnot only takes 2 arguments");
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
            case op_div:
                assert(i < 3 && "div only takes 2 arguments");
                break;
            case op_mod:
                assert(i < 3 && "mod only takes 2 arguments");
                break;
            case op_not:
                assert(i < 2 && "not only takes 1 argument");
                break;
            case op_and:
                assert(i < 3 && "and only takes 2 arguments");
                break;
            case op_or:
                assert(i < 3 && "or only takes 2 arguments");
                break;
            case op_eq:
                assert(i < 3 && "== only takes 2 arguments");
                break;
            case op_neq:
                assert(i < 3 && "!= only takes 2 arguments");
                break;
            case op_lt:
                assert(i < 3 && "< only takes 2 arguments");
                break;
            case op_gt:
                assert(i < 3 && "> only takes 2 arguments");
                break;
            case op_le:
                assert(i < 3 && "<= only takes 2 arguments");
                break;
            case op_ge:
                assert(i < 3 && ">= only takes 2 arguments");
                break;
            case op_if:
                assert(i < 4 && "if only takes 3 arguments");
                break;
            case op_while:
                assert(i < 3 && "while only takes 2 arguments");
                break;
            case op_memset:
                assert(i < 3 && "memset only takes 2 arguments");
                break;
            case op_memget:
                assert(i < 2 && "memget only takes 1 arguments");
                break;
            case op_parseint:
                assert(i < 2 && "parseint only takes 1 argument");
                break;
            case op_inttostr:
                assert(i < 2 && "inttostr only takes 1 argument");
                break;
            case op_argc:
                break;
            case op_argv:
                assert(i < 2 && "argv only takes 1 argument");
                break;
            case op_testingop:
                break;
            default:
                assert(0 && "not recognised operation");
        }
        programCounter++; // first argument
        token param = tokenised[programCounter];
        if (param.opnum == op_argend){
            global_program_counter = programCounter + 1;
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
    global_program_counter = programCounter + 1;
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
    fprintf(fpointer, "\tmov rbp, rsp\t\t\t\t; set stack base\n");
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

void printAsmProgram(FILE* fpointer, astNode* node, char* stringVariables[], bool weare_in_ifstatement,
                        bool weare_in_whileloop, int argindex, int statement_depth, int loop_depth){
    if (weare_in_ifstatement){
        if (argindex == 1){
            fprintf(fpointer, "; if block start\n");
        } else if (argindex == 2){
            fprintf(fpointer, "\tjmp endiflabel%d\t\t\t\t; jump to endif\n", statement_depth);
            fprintf(fpointer, "elselabel%d:\t\t\t\t; else block start\n", statement_depth);
        }
    }
    if (weare_in_whileloop && argindex == 1){
        fprintf(fpointer, "; while code block start\n");
    }
    
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
            fprintf(fpointer, "\tpushq stringvar%d\t\t\t\t; string var def\n", string_variable_count);
            stringVariables[string_variable_count++] = node->info;
        }
        return;
    }


    if (node->opnum == op_if){
        weare_in_ifstatement = true;
        global_statement_depth++;
        statement_depth = global_statement_depth;
    } else {
        weare_in_ifstatement = false;
    }
    if (node->opnum == op_while){
        weare_in_whileloop = true;
        global_loop_depth++;
        loop_depth = global_loop_depth;
        fprintf(fpointer, "whilelooplabel%d:\t\t\t\t; while condition\n", loop_depth);
    } else {
        weare_in_whileloop = false;
    }

    for (int i = 0; i < MAX_ARGUMENT_COUNT; i++){
        if (!node->child[i]){
            break;
        }
        printAsmProgram(fpointer, node->child[i], stringVariables, weare_in_ifstatement, weare_in_whileloop, i, statement_depth, loop_depth);

        if (weare_in_ifstatement && i == 0){
            fprintf(fpointer, "\tpopq rax\t\t\t\t; if jump\n"); // if ends here
            fprintf(fpointer, "\tcmp rax, 0\t\t\t\t; |\n");
            fprintf(fpointer, "\tje elselabel%d\t\t\t\t; |\n", statement_depth);
        }
        if (weare_in_whileloop && i == 0){
            fprintf(fpointer, "\tpopq rax\t\t\t\t; while jump\n");
            fprintf(fpointer, "\tcmp rax, 0\t\t\t\t; |\n");
            fprintf(fpointer, "\tje endwhilelabel%d\t\t\t\t; |\n", loop_depth);
        }
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
                fprintf(fpointer, "\tmov qword [bufferpointer], 0\t\t\t\t; |\n"); // reset writebuffer
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
            fprintf(fpointer, "\tmov qword [bufferpointer], 0\t\t\t\t; |\n"); // reset writebuffer
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
        case op_shiftl:
            fprintf(fpointer, "\tpopq rcx\t\t\t\t; shiftl\n");
            fprintf(fpointer, "\tpopq rax\t\t\t\t; |\n");
            fprintf(fpointer, "\tshl rax, cl\t\t\t\t; |\n");
            fprintf(fpointer, "\tpushq rax\t\t\t\t; |\n");
            break;
        case op_shiftr:
            fprintf(fpointer, "\tpopq rcx\t\t\t\t; shiftr\n");
            fprintf(fpointer, "\tpopq rax\t\t\t\t; |\n");
            fprintf(fpointer, "\tshr rax, cl\t\t\t\t; |\n");
            fprintf(fpointer, "\tpushq rax\t\t\t\t; |\n");
            break;
        case op_shiftla:
            fprintf(fpointer, "\tpopq rcx\t\t\t\t; shiftla\n");
            fprintf(fpointer, "\tpopq rax\t\t\t\t; |\n");
            fprintf(fpointer, "\tsal rax, cl\t\t\t\t; |\n");
            fprintf(fpointer, "\tpushq rax\t\t\t\t; |\n");
            break;
        case op_shiftra:
            fprintf(fpointer, "\tpopq rcx\t\t\t\t; shiftra\n");
            fprintf(fpointer, "\tpopq rax\t\t\t\t; |\n");
            fprintf(fpointer, "\tsar rax, cl\t\t\t\t; |\n");
            fprintf(fpointer, "\tpushq rax\t\t\t\t; |\n");
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
        case op_div:
            fprintf(fpointer, "\tpopq rbx\t\t\t\t; div\n");
            fprintf(fpointer, "\tpopq rax\t\t\t\t; |\n");
            fprintf(fpointer, "\tmov rdx, 0\t\t\t\t; |\n");
            fprintf(fpointer, "\tidiv rbx\t\t\t\t; |\n");
            fprintf(fpointer, "\tpushq rax\t\t\t\t; |\n");
            break;
        case op_mod:
            fprintf(fpointer, "\tpopq rbx\t\t\t\t; mod\n");
            fprintf(fpointer, "\tpopq rax\t\t\t\t; |\n");
            fprintf(fpointer, "\tmov rdx, 0\t\t\t\t; |\n");
            fprintf(fpointer, "\tidiv rbx\t\t\t\t; |\n");
            fprintf(fpointer, "\tpushq rdx\t\t\t\t; |\n");
            break;
        case op_not:
            fprintf(fpointer, "\tpopq rax\t\t\t\t; not\n");
            fprintf(fpointer, "\tmov rbx, 0\t\t\t\t; |\n");
            fprintf(fpointer, "\tnot rbx\t\t\t\t; |\n");
            fprintf(fpointer, "\tshl rbx, 1\t\t\t\t; |\n");
            fprintf(fpointer, "\tor rax, rbx\t\t\t\t; |\n");
            fprintf(fpointer, "\tnot rax\t\t\t\t; |\n");
            fprintf(fpointer, "\tpushq rax\t\t\t\t; |\n");
            break;
        case op_and:
            fprintf(fpointer, "\tpopq rbx\t\t\t\t; and\n");
            fprintf(fpointer, "\tpopq rax\t\t\t\t; |\n");
            fprintf(fpointer, "\tand rax, rbx\t\t\t\t; |\n");
            fprintf(fpointer, "\tpushq rax\t\t\t\t; |\n");
            break;
        case op_or:
            fprintf(fpointer, "\tpopq rbx\t\t\t\t; or\n");
            fprintf(fpointer, "\tpopq rax\t\t\t\t; |\n");
            fprintf(fpointer, "\tor rax, rbx\t\t\t\t; |\n");
            fprintf(fpointer, "\tpushq rax\t\t\t\t; |\n");
            break;
        case op_eq:
            fprintf(fpointer, "\tpopq rbx\t\t\t\t; eq\n");
            fprintf(fpointer, "\tpopq rax\t\t\t\t; |\n");
            fprintf(fpointer, "\tcmp rax, rbx\t\t\t\t; |\n");
            fprintf(fpointer, "\tmov rax, 0\t\t\t\t; |\n");
            fprintf(fpointer, "\tmov rbx, 1\t\t\t\t; |\n");
            fprintf(fpointer, "\tcmove rax, rbx\t\t\t\t; |\n");
            fprintf(fpointer, "\tpushq rax\t\t\t\t; |\n");
            break;
        case op_neq:
            fprintf(fpointer, "\tpopq rbx\t\t\t\t; neq\n");
            fprintf(fpointer, "\tpopq rax\t\t\t\t; |\n");
            fprintf(fpointer, "\tcmp rax, rbx\t\t\t\t; |\n");
            fprintf(fpointer, "\tmov rax, 1\t\t\t\t; |\n");
            fprintf(fpointer, "\tmov rbx, 0\t\t\t\t; |\n");
            fprintf(fpointer, "\tcmove rax, rbx\t\t\t\t; |\n");
            fprintf(fpointer, "\tpushq rax\t\t\t\t; |\n");
            break;
        case op_lt:
            fprintf(fpointer, "\tpopq rbx\t\t\t\t; lt\n");
            fprintf(fpointer, "\tpopq rax\t\t\t\t; |\n");
            fprintf(fpointer, "\tcmp rax, rbx\t\t\t\t; |\n");
            fprintf(fpointer, "\tmov rax, 0\t\t\t\t; |\n");
            fprintf(fpointer, "\tmov rbx, 1\t\t\t\t; |\n");
            fprintf(fpointer, "\tcmovl rax, rbx\t\t\t\t; |\n");
            fprintf(fpointer, "\tpushq rax\t\t\t\t; |\n");
            break;
        case op_gt:
            fprintf(fpointer, "\tpopq rbx\t\t\t\t; gt\n");
            fprintf(fpointer, "\tpopq rax\t\t\t\t; |\n");
            fprintf(fpointer, "\tcmp rax, rbx\t\t\t\t; |\n");
            fprintf(fpointer, "\tmov rax, 0\t\t\t\t; |\n");
            fprintf(fpointer, "\tmov rbx, 1\t\t\t\t; |\n");
            fprintf(fpointer, "\tcmovg rax, rbx\t\t\t\t; |\n");
            fprintf(fpointer, "\tpushq rax\t\t\t\t; |\n");
            break;
        case op_le:
            fprintf(fpointer, "\tpopq rbx\t\t\t\t; le\n");
            fprintf(fpointer, "\tpopq rax\t\t\t\t; |\n");
            fprintf(fpointer, "\tcmp rax, rbx\t\t\t\t; |\n");
            fprintf(fpointer, "\tmov rax, 0\t\t\t\t; |\n");
            fprintf(fpointer, "\tmov rbx, 1\t\t\t\t; |\n");
            fprintf(fpointer, "\tcmovle rax, rbx\t\t\t\t; |\n");
            fprintf(fpointer, "\tpushq rax\t\t\t\t; |\n");
            break;
        case op_ge:
            fprintf(fpointer, "\tpopq rbx\t\t\t\t; ge\n");
            fprintf(fpointer, "\tpopq rax\t\t\t\t; |\n");
            fprintf(fpointer, "\tcmp rax, rbx\t\t\t\t; |\n");
            fprintf(fpointer, "\tmov rax, 0\t\t\t\t; |\n");
            fprintf(fpointer, "\tmov rbx, 1\t\t\t\t; |\n");
            fprintf(fpointer, "\tcmovge rax, rbx\t\t\t\t; |\n");
            fprintf(fpointer, "\tpushq rax\t\t\t\t; |\n");
            break;
        case op_if:
            fprintf(fpointer, "endiflabel%d:\t\t\t\t; if statement end\n", statement_depth);
            break;
        case op_while:
            fprintf(fpointer, "\tjmp whilelooplabel%d\t\t\t\t; while loop end\n", loop_depth);
            fprintf(fpointer, "endwhilelabel%d:\t\t\t\t; |\n", loop_depth);
            break;
        case op_memset:
            fprintf(fpointer, "\tpopq rax\t\t\t\t; memset\n");
            fprintf(fpointer, "\tpopq rcx\t\t\t\t; |\n");
            fprintf(fpointer, "\tshl rcx, 3\t\t\t\t; |\n");
            fprintf(fpointer, "\tlea rbx, [gpmemory]\t\t\t\t; |\n");
            fprintf(fpointer, "\tadd rbx, rcx\t\t\t\t; |\n");
            fprintf(fpointer, "\tmov [rbx], rax\t\t\t\t; |\n");
            break;
        case op_memget:
            fprintf(fpointer, "\tpopq rcx\t\t\t\t; memget\n");
            fprintf(fpointer, "\tshl rcx, 3\t\t\t\t; |\n");
            fprintf(fpointer, "\tlea rbx, [gpmemory]\t\t\t\t; |\n");
            fprintf(fpointer, "\tadd rbx, rcx\t\t\t\t; |\n");
            fprintf(fpointer, "\tmov rax, [rbx]\t\t\t\t; |\n");
            fprintf(fpointer, "\tpushq rax\t\t\t\t; |\n");
            break;
        case op_parseint:
            fprintf(fpointer, "\tpopq rsi\t\t\t\t; parseint\n");
            fprintf(fpointer, "\tmov rdi, rsi\t\t\t\t; |\n");
            fprintf(fpointer, "countforward%d:\t\t\t\t; |\n", putc_calls_count);
            fprintf(fpointer, "\tadd rdi, 1\t\t\t\t; |\n");
            fprintf(fpointer, "\tcmp byte [rdi], 0\t\t\t\t; |\n");
            fprintf(fpointer, "\tjne countforward%d\t\t\t\t; |\n", putc_calls_count++);
            fprintf(fpointer, "\tmov rax, 0\t\t\t\t; |\n");
            fprintf(fpointer, "\tmov rbx, 10\t\t\t\t; |\n");
            fprintf(fpointer, "calcnumber%d:\t\t\t\t; |\n", putc_calls_count);
            fprintf(fpointer, "\tmov dl, [rsi]\t\t\t\t; |\n");
            fprintf(fpointer, "\tsub dl, 48\t\t\t\t; |\n");
            fprintf(fpointer, "\tadd rax, rdx\t\t\t\t; |\n");
            fprintf(fpointer, "\tmul rbx\t\t\t\t; |\n");
            fprintf(fpointer, "\tadd rsi, 1\t\t\t\t; |\n");
            fprintf(fpointer, "\tcmp rsi, rdi\t\t\t\t; |\n");
            fprintf(fpointer, "\tjne calcnumber%d\t\t\t\t; |\n", putc_calls_count++);
            fprintf(fpointer, "\tmov rdx, 0\t\t\t\t; |\n");
            fprintf(fpointer, "\tdiv rbx\t\t\t\t; |\n");
            fprintf(fpointer, "\tpushq rax\t\t\t\t; |\n");
            break;
        case op_inttostr:
            fprintf(fpointer, "\tpopq rax\t\t\t\t; inttostr\n");
            fprintf(fpointer, "\tmov r8, rax\t\t\t\t; |\n");
            fprintf(fpointer, "\tmov rbx, 10\t\t\t\t; |\n");
            fprintf(fpointer, "\tmov rcx, 0\t\t\t\t; |\n");
            fprintf(fpointer, "putlabel%d:\t\t\t\t; |\n", putc_calls_count);
            fprintf(fpointer, "\tadd rcx, 1\t\t\t\t; |\n");
            fprintf(fpointer, "\tmov rdx, 0\t\t\t\t; |\n");
            fprintf(fpointer, "\tidiv rbx\t\t\t\t; |\n");
            fprintf(fpointer, "\tcmp rax, 0\t\t\t\t; |\n");
            fprintf(fpointer, "\tjnz putlabel%d\t\t\t\t; |\n", putc_calls_count++);
            // rcx je log(rax)
            fprintf(fpointer, "\tmov rax, r8\t\t\t\t; |\n");
            fprintf(fpointer, "\tlea rsi, [emptynumber%d]\t\t\t\t; |\n", empty_number_count);
            fprintf(fpointer, "\tadd rsi, rcx\t\t\t\t; |\n");
            fprintf(fpointer, "putlabel%d:\t\t\t\t; |\n", putc_calls_count);
            fprintf(fpointer, "\tmov rdx, 0\t\t\t\t; |\n");
            fprintf(fpointer, "\tidiv rbx\t\t\t\t; |\n");
            fprintf(fpointer, "\tadd rdx, 48\t\t\t\t; |\n");
            fprintf(fpointer, "\tmov [rsi], dl\t\t\t\t; |\n");
            fprintf(fpointer, "\tsub rsi, 1\t\t\t\t; |\n");
            fprintf(fpointer, "\tsub rcx, 1\t\t\t\t; |\n");
            fprintf(fpointer, "\tcmp rcx, 0\t\t\t\t; |\n");
            fprintf(fpointer, "\tjg putlabel%d\t\t\t\t; |\n", putc_calls_count++);
            fprintf(fpointer, "\tpushq emptynumber%d\t\t\t\t; |\n", empty_number_count++);
            break;
        case op_argc:
            fprintf(fpointer, "\tpushq [rbp]\t\t\t\t; argc\n");
            break;
        case op_argv:
            fprintf(fpointer, "\tpopq rax\t\t\t\t; argv\n");
            fprintf(fpointer, "\tmov rbx, rbp\t\t\t\t; |\n");
            fprintf(fpointer, "\tadd rbx, 8\t\t\t\t; |\n");
            fprintf(fpointer, "\timul rax, rax, 8\t\t\t\t; |\n");
            fprintf(fpointer, "\tadd rax, rbx\t\t\t\t; |\n");
            fprintf(fpointer, "\tpushq [rax]\t\t\t\t; |\n");
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
    for (int i  = 0; i < empty_number_count; i++){
        fprintf(fpointer, "emptynumber%d db 23 dup (0)\n", i);
    }
    fprintf(fpointer, "gpmemory rq %d\n", ALLOCATED_GP_MEMORY);
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
    char* binaryFlieName = NULL;

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
            if (codeInFilename){
                binaryFlieName = (char*)argv[i];
            } else {
                codeInFilename = (char*)argv[i];
            }
        }
    }

    if (flags.help){
        system("cat help.txt");
        return 0;
    }

    if (!codeInFilename){
        printf("input file missing. use -h flag for help\n");
        return 1;
    }
    if (!binaryFlieName){
        binaryFlieName = "out.elf";
    }

    int tokenCount = lex(codeInFilename, tokenised);
    if (flags.tokens){
        FILE* tokenOutFile = fopen(tokenOutFilename, "w");
        printTokenisedProgram(tokenOutFile, tokenised);
        fclose(tokenOutFile);
    }

    int usedLinesOfCode = 0;
    astNode* linesOfCode[MAX_LINES_OF_CODE];
    for (int i = 0; global_program_counter < tokenCount && i < MAX_LINES_OF_CODE; i++){
        linesOfCode[i] = (astNode*)calloc(1, sizeof(astNode));
        insertASTNode(tokenised, global_program_counter, linesOfCode[i]);
        usedLinesOfCode = i + 1;
    }
    if (flags.tree){
        FILE* treeOutFile = fopen(treeOutFilename, "w");
        for (int i = 0; i < usedLinesOfCode; i++){
            printAST(treeOutFile, linesOfCode[i], 0);
        }
        fclose(treeOutFile);
    }

    if (!flags.assembly){
        char* tempFilename = "temp.asm";
        assemblyOutFilename = tempFilename;
    }
    FILE* assemblyOutFile = fopen(assemblyOutFilename, "w");
    printAsmHead(assemblyOutFile);
    for (int i = 0; i < usedLinesOfCode; i++){
        printAsmProgram(assemblyOutFile, linesOfCode[i], stringVariables, false, false, 0, 0, 0);
    }
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
    strcat(command, binaryFlieName);
    system(command);

    strcpy(command, "chmod +x ");
    strcat(command, binaryFlieName);
    system(command);

    if (!flags.assembly){
        strcpy(command, "rm ");
        strcat(command, assemblyOutFilename);
        system(command);
    }

    return 0;
}
