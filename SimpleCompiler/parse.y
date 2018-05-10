%{
#include <stdio.h>
#include "attr.h"
#include "instrutil.h"
int yylex();
void yyerror(char * s);
#include "symtab.h"

FILE *outfile;
char *CommentBuffer;
 
%}

%union {tokentype token; regInfo targetReg; labelInfo label;}

%token PROG PERIOD VAR 
%token INT BOOL PRINT THEN IF DO  
%token ARRAY OF 
%token BEG END ASG  
%token EQ NEQ LT LEQ GT GEQ AND OR TRUE FALSE
%token ELSE
%token WHILE 
%token <token> ID ICONST 

%type <targetReg> exp 
%type <targetReg> lhs 
%type <targetReg> idlist
%type <targetReg> type
%type <targetReg> stype
%type <label>     WHILE
%type <label>     ifhead
%type <label>     condexp

%start program

%nonassoc EQ NEQ LT LEQ GT GEQ 
%left '+' '-' AND
%left '*' OR

%nonassoc THEN
%nonassoc ELSE

%%
program:
            {
                emitComment("Assign STATIC_AREA_ADDRESS to register \"r0\"");
                emit(NOLABEL, LOADI, STATIC_AREA_ADDRESS, 0, EMPTY);
            }
            PROG ID ';' block PERIOD {  }
            ;

block:      variables cmpdstmt      {  }
            ;

variables:  /* empty */
            | VAR vardcls           {  }
            ;

vardcls:    vardcls vardcl ';'      {  }
            | vardcl ';'
            {
            }
            | error ';'
            { 
                yyerror("***Error: illegal variable declaration\n");
            }
            ;

vardcl:     idlist ':' type
            { /* insert declared variables into symbol table */
                int offset = 0;
                char *id_name;
                Node *ptr = $1.head;

                while (ptr) {
                    id_name = ptr->name;
                    if ($3.isArray) {
                        offset = NextOffset($3.size);
                    }
                    else {
                        offset = NextOffset(1);
                    }
                    insert(id_name, $3.type, offset, $3.isArray);
                    ptr = ptr->next;
                }
                destroy($1.head);
            }
            ;

idlist:     idlist ',' ID
            { 
                Node *new_node = malloc(sizeof(Node));
                if (!new_node) {
                    printf("\n***ERROR***: Malloc failed to allocate\n");
                }
                new_node->name = $3.str;
                new_node->next = NULL;

                $$.head = push($$.head, new_node);

            }
            | ID
            {
                Node *new_node = malloc(sizeof(Node));
                if (!new_node) {
                    printf("\n***ERROR***: Malloc failed to allocate\n");
                }
                new_node->name = $1.str;
                new_node->next = NULL;
                $$.head = push($$.head, new_node);
            }
            ;

type:       ARRAY '[' ICONST ']' OF stype
            {
                $$.type = $6.type;
                $$.isArray = 1;
                $$.size = $3.num;
            }
            | stype
            { 
                $$.type = $1.type;
                $$.isArray = $1.isArray;
            }
            ;

stype:      INT
            {
                $$.type = TYPE_INT;
                $$.isArray = 0;
            }
            | BOOL 
            { 
                $$.type = TYPE_BOOL; 
                $$.isArray = 0;
            }
            ;

stmtlist:   stmtlist ';' stmt { }
            | stmt            { }
            | error { yyerror("***Error: ';' expected or illegal statement \n"); }
            ;

stmt:       ifstmt      { }
            | wstmt     { }
            | astmt     { }
            | writestmt { }
            | cmpdstmt  { }
            ;

cmpdstmt:   BEG stmtlist END { }
            ;

ifstmt:     ifhead
            {
                int label1 = NextLabel();
                int label2 = NextLabel();
                int label3 = NextLabel();

                $<label>$.label1 = label1;
                $<label>$.label2 = label2;
                $<label>$.label3 = label3;

                emit(NOLABEL, CBR, $<label>1.targetRegister, label1, label2);
                emit(label1, NOP, EMPTY, EMPTY, EMPTY);
                emitComment("This is the \"true\" branch");
            }
            THEN stmt
            {
                emitComment("Branch to statement following the \"else\" statement list");
                emit(NOLABEL, BR, $<label>2.label3, EMPTY, EMPTY);
                emit($<label>2.label2, NOP, EMPTY, EMPTY, EMPTY);
                emitComment("This is the \"false\" branch");
            }
            ELSE {}
            stmt
            {
                emit($<label>2.label3, NOP, EMPTY, EMPTY, EMPTY);
            }
            ;

ifhead:     IF condexp
            {
                $$.targetRegister = $2.targetRegister;
            }
            ;

writestmt:  PRINT '(' exp ')' 
            { 
                int printOffset = -4; /* default location for printing */

                sprintf(CommentBuffer, "Code for \"PRINT\" from offset %d", printOffset);
                emitComment(CommentBuffer);
                emit(NOLABEL, STOREAI, $3.targetRegister, 0, printOffset);
                emit(NOLABEL, OUTPUTAI, 0, printOffset, EMPTY);
            }
            ;

wstmt:      WHILE 
            {
                int label1 = NextLabel();
                int label2 = NextLabel();
                int label3 = NextLabel();

                $<label>$.label1 = label1;
                $<label>$.label2 = label2;
                $<label>$.label3 = label3;
                emit(label1, NOP, EMPTY, EMPTY, EMPTY);
                emitComment("Control code for \"WHILE DO\"");
            }
            condexp
            {
                emit(NOLABEL, CBR, $<label>3.targetRegister, $<label>2.label2, $<label>2.label3);
                emit($<label>2.label2, NOP, EMPTY, EMPTY, EMPTY);
                emitComment("Body of \"WHILE\" construct starts here");
            } 
            DO stmt
            {
                emit(NOLABEL, BR, $<label>2.label1, EMPTY, EMPTY);
                emit($<label>2.label3, NOP, EMPTY, EMPTY, EMPTY);
            } 
            ;

astmt:      lhs ASG exp 
            { 
                if ($1.type != $3.type) {
                    printf("*** ERROR ***: Assignment types do not match.\n");
                }
                emit(NOLABEL, STORE, $3.targetRegister, $1.targetRegister, EMPTY);
            }
            ;

lhs:        ID 
            {
                int newReg1 = NextRegister();
                int newReg2 = NextRegister();
                int offset = 0;
                SymTabEntry *entry = lookup($1.str);

                if (entry) {
                    if (entry->isArray) {
                        printf("\n*** ERROR ***: Variable %s is not a scalar variable.\n", $1.str); 
                    }
                    $$.targetRegister = newReg2;
                    $$.type = entry->type;
                    offset = entry->offset;

                    sprintf(CommentBuffer, "Compute address of variable \"%s\" at offset %d in register %d", $1.str, offset, newReg2);
                    emitComment(CommentBuffer);
                    emit(NOLABEL, LOADI, offset, newReg1, EMPTY);
                    emit(NOLABEL, ADD, 0, newReg1, newReg2);
                }
                else {
                    printf("\n*** ERROR ***: Variable %s not declared.\n", $1.str);
                }
            }
            | ID '[' exp ']'
            {
                int targetRegister = NextRegister();
                int newReg1 = NextRegister();
                int newReg2 = NextRegister();
                int newReg3 = NextRegister();
                int newReg4 = NextRegister();
                int baseAddr= 0;
                SymTabEntry *entry = lookup($1.str);

                if (entry) {
                    if (!entry->isArray) {
                        printf("\n*** ERROR ***: Variable %s is not an array variable.\n", $1.str); 
                    }
                    if ($3.type != TYPE_INT) {
                        printf("\n*** ERROR ***: Array variable %s index type must be integer.\n", $1.str);  
                    }

                    baseAddr = entry->offset;
                    $$.targetRegister = targetRegister;
                    $$.type = entry->type;

                    sprintf(CommentBuffer, "Compute address of array variable \"%s\" with base address %d", $1.str, baseAddr);
                    emitComment(CommentBuffer);
                    emit(NOLABEL, LOADI, 4, newReg1, EMPTY); // each array entry is 4 bytes
                    emit(NOLABEL, MULT,  $3.targetRegister, newReg1, newReg2); // compute address of index: (4 * index)
                    emit(NOLABEL, LOADI, baseAddr, newReg3, EMPTY); // load baseAddr
                    emit(NOLABEL, ADD,  newReg3, newReg2, newReg4); // compute offset of array index: (4*index) + baseAddr
                    emit(NOLABEL, ADD,  0, newReg4, targetRegister); // compute location of index, store in targetRegister
                }
                else {
                    printf("\n*** ERROR ***: Variable %s not declared.\n", $1.str);
                }
            }
            ;

exp:        exp '+' exp 
            {
                int newReg = NextRegister();

                if (!(($1.type == TYPE_INT) && ($3.type == TYPE_INT))) {
                    printf("\n*** ERROR ***: Operand type must be integer.\n"); 
                }
                $$.type = $1.type;
                $$.targetRegister = newReg;
                emit(NOLABEL, ADD, $1.targetRegister, $3.targetRegister, newReg);
            }
            | exp '-' exp 
            { 
                int newReg = NextRegister();

                if (!(($1.type == TYPE_INT) && ($3.type == TYPE_INT))) {
                    printf("\n*** ERROR ***: Operand type must be integer.\n"); 
                }
                $$.type = $1.type;
                $$.targetRegister = newReg;
                emit(NOLABEL, SUB, $1.targetRegister, $3.targetRegister, newReg);
            }
            | exp '*' exp 
            {  
                int newReg = NextRegister();

                if (!(($1.type == TYPE_INT) && ($3.type == TYPE_INT))) {
                    printf("\n*** ERROR ***: Operand type must be integer.\n"); 
                }
                $$.type = $1.type;
                $$.targetRegister = newReg;
                emit(NOLABEL, MULT, $1.targetRegister, $3.targetRegister, newReg);
            }
            | exp AND exp
            {
                int newReg = NextRegister();

                if (!(($1.type == TYPE_BOOL) && ($3.type == TYPE_BOOL))) {
                    printf("\n*** ERROR ***: Operand type must be boolean.\n"); 
                }
                $$.type = $1.type;
                $$.targetRegister = newReg;
                emit(NOLABEL, AND_INSTR, $1.targetRegister, $3.targetRegister, newReg);
            } 
            | exp OR exp 
            {  
                int newReg = NextRegister();

                if (!(($1.type == TYPE_BOOL) && ($3.type == TYPE_BOOL))) {
                    printf("\n*** ERROR ***: Operand type must be boolean.\n"); 
                }
                $$.type = $1.type;
                $$.targetRegister = newReg;
                emit(NOLABEL, OR_INSTR, $1.targetRegister, $3.targetRegister, newReg);
            }
            | ID 
            { 
                int newReg = NextRegister();
                int offset = 0;
                SymTabEntry *entry = lookup($1.str);

                if (entry) {
                    if (entry->isArray) {
                        printf("\n*** ERROR ***: Variable %s is not a scalar variable.\n", $1.str); 
                    }
                    offset = entry->offset;
                    $$.targetRegister = newReg;
                    $$.type = entry->type;
                    sprintf(CommentBuffer, "Load RHS value of variable \"%s\" at offset %d", $1.str, offset);
                    emitComment(CommentBuffer);
                    emit(NOLABEL, LOADAI, 0, offset, newReg);
                }
                else {
                    printf("\n*** ERROR ***: Variable %s not declared.\n", $1.str); 
                }
            }
            | ID '[' exp ']'
            { 
                int targetRegister = NextRegister();
                int newReg1 = NextRegister();
                int newReg2 = NextRegister();
                int newReg3 = NextRegister();
                int newReg4 = NextRegister();
                int baseAddr = 0;
                SymTabEntry *entry = lookup($1.str);

                if (entry) {
                    if (!entry->isArray) {
                        printf("\n*** ERROR ***: Variable %s is not an array variable.\n", $1.str);
                    }
                    if ($3.type != TYPE_INT) {
                        printf("\n*** ERROR ***: Array variable %s index type must be integer.\n", $1.str);  
                    }
                    baseAddr = entry->offset;
                    $$.targetRegister = targetRegister;
                    $$.type = entry->type;

                    sprintf(CommentBuffer, "Load RHS value of array variable \"%s\" with base address %d", $1.str, baseAddr);
                    emitComment(CommentBuffer);
                    emit(NOLABEL, LOADI, 4, newReg1, EMPTY); // each array entry is 4 bytes
                    emit(NOLABEL, MULT,  $3.targetRegister, newReg1, newReg2); // compute address of index: (4 * index)
                    emit(NOLABEL, LOADI, baseAddr, newReg3, EMPTY); // load baseAddr
                    emit(NOLABEL, ADD,  newReg3, newReg2, newReg4); // compute offset of array index: (4*index) + baseAddr
                    emit(NOLABEL, LOADAO, 0, newReg4, targetRegister);
                }
                else {
                    printf("\n*** ERROR ***: Variable %s not declared.\n", $1.str); 
                }
            }
            | ICONST 
            {
                int newReg = NextRegister();

                $$.targetRegister = newReg;
                $$.type = TYPE_INT;
                emit(NOLABEL, LOADI, $1.num, newReg, EMPTY); 
            }
            | TRUE 
            {
                int newReg = NextRegister(); /* TRUE is encoded as value '1' */

                $$.targetRegister = newReg;
                $$.type = TYPE_BOOL;
                emit(NOLABEL, LOADI, 1, newReg, EMPTY);
            }
            | FALSE 
            {
                int newReg = NextRegister(); /* TRUE is encoded as value '0' */

                $$.targetRegister = newReg;
                $$.type = TYPE_BOOL;
                emit(NOLABEL, LOADI, 0, newReg, EMPTY);
            }
            | error
            {
                yyerror("***Error: illegal expression\n");
            }  
            ;


condexp:    exp NEQ exp
            {  
                int newReg = NextRegister();
                if ($1.type != $3.type) {
                    printf("\n*** ERROR ***: == or != operator with different types.\n"); 
                }
                $$.targetRegister = newReg;
                emit(NOLABEL, CMPNE, $1.targetRegister, $3.targetRegister, newReg);
            } 
            | exp EQ exp
            {
                int newReg = NextRegister();
                if ($1.type != $3.type) {
                    printf("\n*** ERROR ***: == or != operator with different types.\n"); 
                }
                $$.targetRegister = newReg;
                emit(NOLABEL, CMPEQ, $1.targetRegister, $3.targetRegister, newReg);
            }
            | exp LT exp
            {
                int newReg = NextRegister();
                if (!($1.type == TYPE_INT && $3.type == TYPE_INT)) {
                    printf("\n*** ERROR ***: Relational operator with illegal type.\n");
                }
                $$.targetRegister = newReg;
                emit(NOLABEL, CMPLT, $1.targetRegister, $3.targetRegister, newReg);
            }
            | exp LEQ exp
            {
                int newReg = NextRegister();
                if (!($1.type == TYPE_INT && $3.type == TYPE_INT)) {
                    printf("\n*** ERROR ***: Relational operator with illegal type.\n");
                }
                $$.targetRegister = newReg;
                emit(NOLABEL, CMPLE, $1.targetRegister, $3.targetRegister, newReg);
            }
            | exp GT exp
            {
                int newReg = NextRegister();
                if (!($1.type == TYPE_INT && $3.type == TYPE_INT)) {
                    printf("\n*** ERROR ***: Relational operator with illegal type.\n");
                }
                $$.targetRegister = newReg;
                emit(NOLABEL, CMPGT, $1.targetRegister, $3.targetRegister, newReg);
            }
            | exp GEQ exp
            {
                int newReg = NextRegister();
                if (!($1.type == TYPE_INT && $3.type == TYPE_INT)) {
                    printf("\n*** ERROR ***: Relational operator with illegal type.\n");
                }
                $$.targetRegister = newReg;
                emit(NOLABEL, CMPGE, $1.targetRegister, $3.targetRegister, newReg);
            }
            | error
            {
                yyerror("***Error: illegal conditional expression\n");
            }
            ;

%%

void yyerror(char* s) {
    fprintf(stderr,"%s\n",s);
}


int main(int argc, char* argv[]) {
    printf("\n     CS415 Spring 2018 Compiler\n\n");

    outfile = fopen("iloc.out", "w");
    if (outfile == NULL) { 
        printf("ERROR: cannot open output file \"iloc.out\".\n");
        return -1;
    }

    CommentBuffer = (char *) malloc(650);  
    InitSymbolTable();

    printf("1\t");
    yyparse();
    printf("\n");

    PrintSymbolTable();
  
    fclose(outfile);
  
    return 1;
}
