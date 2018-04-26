# Simple Compiler For a Pascal-like Language, Rutgers CS415 Project2

Using Bison and Flex, a parser and code generator (ILOC) is generated for a subset of a Pascal-like language. There are no procedures in this language. Base types are limited to integer and boolean Composit types are limited to single-dimensional arrays of based types, indexed by integers (0-based indexing).

# Supported Statments
Only the following statements are supported:
* while-do,
* if-then-else,
* assignment,
* print, and
* compound

Operators are restricted to:
* arithmetic
* logical
* relational

The parser supports semantic analysis (e.g. type checking) and compiles the input languge into an intermediate representation (ILOC).

# Usage

## Prerequsites
* Bison and Flex

## Compiling
Use the added Makefile to compile by typing:
'''make'''
Clean the directory by typing:
'''make clean'''

## Running
'''./codegen < input_file'''
where "input file" is file containing some program for this language

# BNF Grammar Specification for input language
'''
program:    PROG ID ';' block PERIOD 

block:      variables cmpdstmt 

variables:  /* empty */
            | VAR vardcls 

vardcls:    vardcls vardcl ';' 
            | vardcl ';' 


vardcl:     idlist ':' type 


idlist:     idlist ',' ID 
            | ID        


type:       ARRAY '[' ICONST ']' OF stype 
            | stype 


stype:      INT 
            | BOOL 

stmtlist:   stmtlist ';' stmt 
            | stmt 

stmt:       ifstmt 
            | wstmt 
            | astmt 
            | writestmt 
            | cmpdstmt 

cmpdstmt:   BEG stmtlist END 

ifstmt:     ifhead THEN stmt ELSE stmt 

ifhead:     IF condexp 

writestmt:  PRINT '(' exp ')' 

wstmt:      WHILE condexp DO stmt  

astmt:      lhs ASG exp            

lhs:        ID    
            | ID '[' exp ']' 

exp:        exp '+' exp   
            | exp '-' exp
            | exp '*' exp   
            | exp AND exp
            | exp OR exp 
            | ID    
            | ID '[' exp ']'
            | ICONST   
            | TRUE     
            | FALSE    

condexp:    exp NEQ exp   
            | exp EQ exp    
            | exp LT exp    
            | exp LEQ exp   
            | exp GT exp    
            | exp GEQ exp   

ID := (A-Za-z)(A-Za-z)\*
'''
