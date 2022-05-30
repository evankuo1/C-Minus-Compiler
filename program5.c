// Author: Evan Kuo
// Class: CSC 453
// Description: This is the compiler that we are writing for CSC 453. This compiler is being written piece by piece
// in milestones across the entire semester

	

// -------------------------------------------------------------------------------------------------------------------------
// Import Libraries

// Import standard libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Import libraries written by myself or Dr.Debray
#include "scanner.h"		// Scanner
#include "ast.h"			// AST
#include "program5.h"		// AST
#include "driver.h"			// Dr.Debray's base 


// --------------------------------------------------------------------------------------------------------------------------
// Global variables 

// Scanner globals
char *lexeme;					// Current lexeme
char ch;						// Current char
int lineNum = 1;				// Current line number
Token lookaheadToken = -100;	// Lookahead token
int currentlyLookahead = 0;		// Flag for whether or not we're currently in lookahead
int tempLineCount = 0;			// Line count for lookahead error
char currID[1024];				// Current ID as a string

// Parser globals
Token curr_tok;				// The current token.
int fixLineNumError = 0;	// Used to fix the line num error when lookahead is used

// Semantic Checking  globals
struct symbolTableStack *symbolTable;			// The current symbol table
struct individualFuncTable *globalFuncTable;	// The global symbol table 
int currNumParameters = 0;						// Current num parameters for function definition
int currFuncCallParamNum = 0;					// Current num parameters for function call
char *currFuncID;								// Name of func being called

// AST globals
struct astNode *firstASTNode;					// Root for current function. This assumes no local function declarations.
struct astNode *currNodeAdd;					// The current node
struct astNode *boolExp;						// A boolean expression
struct innerHolder *innerStmtListReturn;		// Where to return to in the AST when we exit an if/else/while
int inInner = 0;								// Flag for if we're in an if/else/while
int inElse = 0;									// Flag for if we're in an else
int inBrackets = 0;								// Flag for if we're in an if/else/while


// ----------------------------------------------------------------------------------------------------------------------
// Scanner functions
// Return line number of token
int getLineNum() {
	return lineNum;
}

int isNumeric(char ch) {
	if (ch == '1' || ch == '2' || ch == '3' || ch == '4' || ch == '5' || ch == '6'|| ch == '7' || ch == '8' || ch == '9' || ch == '0') {
		return 1;
	}

	return 0;
}

int lookahead() {
	if (lookaheadToken == -100) {
		currentlyLookahead = 1;
		lookaheadToken = get_token();
		return lookaheadToken;
	}

	else {
		return lookaheadToken;
	}
}

char * getCurrID() {
	return currID;
}

int get_token() {

	if (lookaheadToken != -100) {
		Token temp = lookaheadToken;
		lookaheadToken = -100;
		currentlyLookahead = 0;
		lineNum = lineNum + tempLineCount;
		tempLineCount = 0;
		return temp;
	}

	// Give lots of space for lexeme and create a ptr
	lexeme = calloc(1024, 1);
	char *ptr = lexeme;

	// Get character
	ch = getchar();

	// While there are still tokens to read
	while(ch != EOF) {

		// If the token is a "nothing character" then skip it
		if (ch == '\n' || ch == ' ' || ch == '\t') {
			if (ch == '\n') {
				if (currentlyLookahead == 0) {
					lineNum = lineNum + 1;
				}
				else {
					tempLineCount = tempLineCount + 1;
				}
			}
			ch = getchar();
			continue;
		}

		// If the token is a comment.
		else if (ch == '/') {

			ch = getchar();

			if (ch == '*') {

				ch = getchar();

				returnWhile: 
				while (ch != '*') {

					if (ch == EOF) {
						return -1;
					}

					if (ch == '\n') {
						if (currentlyLookahead == 0) {
							lineNum = lineNum + 1;
						}
					
						else {
						tempLineCount = tempLineCount + 1;
						}
					}

					ch = getchar();
				}

				returnStarCheck:
				ch = getchar();

				if (ch == '/') {
					ch = getchar();
					continue;
				}

				else if (ch == '*') {
					goto returnStarCheck;
				}

				else {
					ch = getchar();
					goto returnWhile;
				}
			}

			else  {
				ungetc(ch, stdin);
				*ptr++ = '/';
				*ptr = '\0';
				return 18;
			}
		}


		// If the token is a word
		else if (isalpha(ch)) {

			// Loop through until we find a non-letter character
			while (isalpha(ch) || isNumeric(ch) || ch == '_') {
				*ptr++ = ch;
				ch = getchar();
			}

			ungetc(ch, stdin);

			// Null terminator at the end
			*ptr = '\0';

			// Check if this word is one of the keywords
			// If it's int
			if (strcmp(lexeme, "int") == 0) {
				return 9;
			}

			else if (strcmp(lexeme, "if") == 0) {
				return 10;
			}

			else if (strcmp(lexeme, "else") == 0) {
				return 11;
			}

			else if (strcmp(lexeme, "while") == 0) {
				return 12;
			}

			else if (strcmp(lexeme, "return") == 0) {
				return 13;
			}

			strcpy(currID, lexeme);

			// Return identifier
			return 1;
		}

		// Ints
		else if (isNumeric(ch) == 1) {
			*ptr++ = ch;
			ch = getchar();
			while (isNumeric(ch) == 1) {
				*ptr++ = ch;
				ch = getchar();
			}
			ungetc(ch, stdin);
			*ptr = '\0';
			strcpy(currID, lexeme);
			return 2;
		}

		// Lparen
		else if (ch == '(') {
			*ptr++ = ch;
			*ptr = '\0';
			return 3;
		}

		// Rparen
		else if (ch == ')') {
			*ptr++ = ch;
			*ptr = '\0';
			return 4;
		}

		// Left bracket
		else if (ch == '{') {
			*ptr++ = ch;
			*ptr = '\0';
			return 5;
		}

		// Right bracket
		else if (ch == '}') {
			*ptr++ = ch;
			*ptr = '\0';
			return 6;
		}

		// comma
		else if (ch == ',') {
			*ptr++ = ch;
			*ptr = '\0';
			return 7;
		}

		// semicolon
		else if (ch == ';') {
			*ptr++ = ch;
			*ptr = '\0';
			return 8;
		}

		// addtion
		else if (ch == '+') {
			*ptr++ = ch;
			*ptr = '\0';
			return 15;
		}

		// subtraction
		else if (ch == '-') {
			*ptr++ = ch;
			*ptr = '\0';
			return 16;
		}

		// multiplication
		else if (ch == '*') {
			*ptr++ = ch;
			*ptr = '\0';
			return 17;
		}

		// If it's an equal
		else if (ch == '=') {

			*ptr++ = ch;

			ch = getchar();
			if (ch == '=') {
				*ptr++ = ch;
				*ptr = '\0';
				return 19;
			}
			ungetc(ch, stdin);
			*ptr = '\0';
			return 14;	
		}

		// !
		else if (ch == '!') {
			*ptr++ = ch;
			ch = getchar();
			if (ch == '=') {
				*ptr++ = ch;
				*ptr = '\0';
				return 20;
			}
			else {
				ungetc(ch, stdin);
				*ptr = '\0';
				return 27;
			}
		}

		// If it's a >
		else if (ch == '>') {

			*ptr++ = ch;
			ch = getchar();

			if (ch == '=') {
				*ptr++ = ch;
				*ptr = '\0';
				return 22;
			}
			ungetc(ch, stdin);
			*ptr = '\0';
			return 21;
		}

		// If it's a <
		else if (ch == '<') {

			*ptr++ = ch;
			ch = getchar();

			if (ch == '=') {
				*ptr++ = ch;
				*ptr = '\0';
				return 24;
			}
			ungetc(ch, stdin);
			*ptr = '\0';
			return 23;	
		}

		// And
		else if (ch == '&') {
			*ptr++ = ch;
			ch = getchar();
			if (ch == '&') {
				*ptr++ = ch;
				*ptr = '\0';
				return 25;
			}
			else {
				ungetc(ch, stdin);
				return 0;
			}
		}

		// Or
		else if (ch == '|') {
			*ptr++ = ch;
			ch = getchar();
			if (ch == '|') {
				*ptr++ = ch;
				*ptr = '\0';
				return 26;
			}
			else {
				ungetc(ch, stdin);
				return 0;
			}
		}

		// Else, return 0 for undefined
		else {
			*ptr++ = ch;
			*ptr = '\0';
			return 0;
		}

		ch = getchar();
	}
	
	return -1;
}


// -------------------------------------------------------------------------------------------------------------------------
// AST functions. Sorted alphabetically

void convertStmtToSolo(struct astNode *inner) {

	if (inner->child2 == NULL) {

	}

	else if (inner->child2->child1 == NULL|| inner->child2->child1->child3 == NULL) {
		inner->child2 = inner->child2->child1;
	}
}



void convertStmtToSoloElse(struct astNode *inner) {

	if (inner->ifElse == NULL) {

	}

	else if (inner->ifElse->child1 == NULL|| inner->ifElse->child1->child3 == NULL) {
		inner->ifElse = inner->ifElse->child1;
	}
}



void * exprListNext(struct astNode *ast) {
	int currNum = ast->numParam;
	struct astNode *stmt = ast->child1;
	int i = 1;
	while (i < currNum) {
		if (stmt != NULL) {
			stmt = stmt->child3;
		}
		i = i + 1;
	}
	return stmt;
}



void pushCurrNodeAdd() {

	if (strcmp(currNodeAdd->type, "FUNC_DEF") == 0) {

		struct astNode *stmtList = malloc(sizeof(struct astNode));
		stmtList->type = "STMT_LIST";
		stmtList->numParam = 1;
		currNodeAdd->child2 = stmtList;

		struct astNode *newNode = malloc(sizeof(struct astNode));
		newNode->child3 = NULL;
		stmtList->child1 = newNode;
		currNodeAdd = newNode;
	}

	else if (strcmp(currNodeAdd->type, "STMT_LIST") == 0) {
		struct astNode *newNode = malloc(sizeof(struct astNode));
		newNode->child3 = NULL;
		currNodeAdd->child1 = newNode;
		currNodeAdd = newNode;
	}

	else if ((strcmp(currNodeAdd->type, "WHILE") == 0 || strcmp(currNodeAdd->type, "IF") == 0) && inInner == 1) {

		struct astNode *stmtListNode = malloc(sizeof(struct astNode));
		stmtListNode->type = "STMT_LIST";
		stmtListNode->numParam = 1;
		currNodeAdd->child2 = stmtListNode;

		struct astNode *newNode = malloc(sizeof(struct astNode));
		newNode->child3 = NULL;
		currNodeAdd->child2->child1 = newNode;
		currNodeAdd = newNode;

	}

	else if (strcmp(currNodeAdd->type, "IF") == 0 && inElse == 1) {

		struct astNode *stmtListNode = malloc(sizeof(struct astNode));
		stmtListNode->type = "STMT_LIST";
		stmtListNode->numParam = 1;
		currNodeAdd->ifElse = stmtListNode;

		struct astNode *newNode = malloc(sizeof(struct astNode));
		newNode->child3 = NULL;
		currNodeAdd->ifElse->child1 = newNode;
		currNodeAdd = newNode;

	}

	else  {
		struct astNode *newNode = malloc(sizeof(struct astNode));
		newNode->child3 = NULL;
		currNodeAdd->child3 = newNode;
		currNodeAdd = newNode;
	}
}



void pushCurrNodeAddStmtList() {

	if (strcmp(currNodeAdd->type, "FUNC_DEF") == 0 && curr_tok == 5 && lookahead() != 6) {
		struct astNode *stmtList = malloc(sizeof(struct astNode));
		stmtList->type = "STMT_LIST";
		stmtList->numParam = 1;
		currNodeAdd->child2 = stmtList;
		currNodeAdd = currNodeAdd->child2;

		struct astNode *stmtList2 = malloc(sizeof(struct astNode));
		stmtList2->type = "STMT_LIST";
		stmtList2->numParam = 1;
		currNodeAdd->child1 = stmtList2;
		currNodeAdd = currNodeAdd->child1;
	}

	else if (strcmp(currNodeAdd->type, "FUNC_DEF") == 0) {
		struct astNode *stmtList = malloc(sizeof(struct astNode));
		stmtList->type = "STMT_LIST";
		stmtList->numParam = 1;
		currNodeAdd->child2 = stmtList;
		currNodeAdd = currNodeAdd->child2;
	}

	else if (strcmp(currNodeAdd->type, "STMT_LIST") == 0 && curr_tok == 5 && lookahead() != 6) {
		struct astNode *stmtList = malloc(sizeof(struct astNode));
		stmtList->type = "STMT_LIST";
		stmtList->numParam = 1;
		currNodeAdd->child1 = stmtList;
		currNodeAdd = currNodeAdd->child1;
	}

	else if (strcmp(currNodeAdd->type, "IF") == 0 && inElse == 0 && curr_tok == 5 && lookahead() != 6) {
		inBrackets = inBrackets + 1;
		struct astNode *stmtList = malloc(sizeof(struct astNode));
		stmtList->type = "STMT_LIST";
		stmtList->numParam = 1;
		currNodeAdd->child2 = stmtList;
		currNodeAdd = currNodeAdd->child2;
	}

	else if (strcmp(currNodeAdd->type, "IF") == 0 && inElse == 1 && curr_tok == 5 && lookahead() != 6) {
		inBrackets = inBrackets + 1;
		struct astNode *stmtList = malloc(sizeof(struct astNode));
		stmtList->type = "STMT_LIST";
		stmtList->numParam = 1;
		currNodeAdd->ifElse = stmtList;
		currNodeAdd = currNodeAdd->ifElse;
	}

	else if (strcmp(currNodeAdd->type, "WHILE") == 0 && curr_tok == 5 && lookahead() != 6) {
		inBrackets = inBrackets + 1;
		struct astNode *stmtList = malloc(sizeof(struct astNode));
		stmtList->type = "STMT_LIST";
		stmtList->numParam = 1;
		currNodeAdd->child2 = stmtList;
		currNodeAdd = currNodeAdd->child2;
	}

	else if (curr_tok == 5 && lookahead() != 6){
		struct astNode *stmtList = malloc(sizeof(struct astNode));
		stmtList->type = "STMT_LIST";
		stmtList->numParam = 1;
		stmtList->child3 = NULL;
		currNodeAdd->child3 = stmtList;
		currNodeAdd = stmtList;
	}
}



void pushFuncCallArg(char * paramName) {
	struct astNode *exprList = currNodeAdd->child1;

	if (exprList->child1 == NULL) {
		struct astNode *newNode = malloc(sizeof(struct astNode));
		newNode->name = malloc(strlen(paramName) + 1);
		strcpy(newNode->name, paramName);

		char * name = newNode->name;
		char firstChar = name[0];
		if (isNumeric(firstChar) == 1) {
			newNode->type = "INTCONST";
			newNode->numParam = atoi(newNode->name);
		}
		else {
			newNode->type = "IDENTIFIER";
		}
		exprList->child1 = newNode;
	}
	else {
		struct astNode *node = exprList->child1;
		while (node->child3 != NULL) {
			node = node->child3;
		}
		struct astNode *newNode = malloc(sizeof(struct astNode));
		newNode->name = malloc(strlen(paramName) + 1);
		strcpy(newNode->name, paramName);

		char * name = newNode->name;
		char firstChar = name[0];
		if (isNumeric(firstChar) == 1) {
			newNode->type = "INTCONST";
			newNode->numParam = atoi(newNode->name);
		}
		else {
			newNode->type = "IDENTIFIER";
		}

		node->child3 = newNode;
	}
}



void pushParamName(char *str) {
	struct astNode *param = firstASTNode->child1;

	if (strcmp(param->name, "none") == 0) {
		param->name = malloc(strlen(str) + 1);
		strcpy(param->name, str);

		char * name = param->name;
		char firstChar = name[0];
		if (isNumeric(firstChar) == 1) {
			param->type = "INTCONST";
			param->numParam = atoi(param->name);
		}
		else {
			param->type = "IDENTIFIER";
		}

		return;
	}

	while (param->child3 != NULL) {
		param = param->child3;
	}


	struct astNode *newParam = malloc(sizeof(struct astNode));
	newParam->name = malloc(strlen(str) + 1);
	strcpy(newParam->name, str);

	char * name = newParam->name;
	char firstChar = name[0];
	if (isNumeric(firstChar) == 1) {
		newParam->type = "INTCONST";
		newParam->numParam = atoi(newParam->name);
	}
	else {
		newParam->type = "IDENTIFIER";
	}
	param->child3 = newParam;
	newParam->child3 = NULL;
}



void * stmtListNext(struct astNode *ast) {
	int currNum = ast->numParam;
	struct astNode *stmt = ast->child1;
	int i = 1;
	while (i < currNum) {
		if (stmt != NULL) {
			stmt = stmt->child3;
		}
		i = i + 1;
	}
	return stmt;
}



void switchToInner() {
	//innerStmtListReturn = currNodeAdd;
	if (innerStmtListReturn == NULL) {
		innerStmtListReturn = malloc(sizeof(struct innerHolder));
		innerStmtListReturn->ast = currNodeAdd;
	}

	else {
		struct innerHolder *newHolder = malloc(sizeof(struct innerHolder));
		newHolder->ast = currNodeAdd;
		newHolder->next = innerStmtListReturn;
		innerStmtListReturn = newHolder;
	}
}



void switchToOuter() {
	//currNodeAdd = innerStmtListReturn;
	currNodeAdd = innerStmtListReturn->ast;
	innerStmtListReturn = innerStmtListReturn->next;
}

// --------------------------------------------------------------------------------------------------------------------------
// AST functions specified by Dr.Debray. Ordered by their ordering in ast.h

/* 
 * ptr: an arbitrary non-NULL AST pointer. 
 * ast_node_type() returns the node type for the AST node ptr points to.
 */
NodeType ast_node_type(void *ptr) {

	struct astNode *ast = ptr;

	if (strcmp(ast->type, "DUMMY") == 0) {
		return 0;
	}

	else if (strcmp(ast->type, "FUNC_DEF") == 0) {
		return 1;
	}

	else if (strcmp(ast->type, "FUNC_CALL") == 0) {
		return 2;
	}

	else if ((strcmp(ast->type, "IF") == 0)) {
		return 3;
	}

	else if ((strcmp(ast->type, "WHILE") == 0)) {
		return 4;
	}

	else if ((strcmp(ast->type, "ASSG") == 0)){
		return 5;
	}

	else if ((strcmp(ast->type, "RETURN") == 0)){
		return 6;
	}

	else if ((strcmp(ast->type, "STMT_LIST") == 0)){
		return 7;
	}

	else if ((strcmp(ast->type, "EXPR_LIST") == 0)){
		return 8;
	}

	else if ((strcmp(ast->type, "IDENTIFIER") == 0)){
		return 9;
	}

	else if ((strcmp(ast->type, "INTCONST") == 0)){
		return 10;
	}

	else if ((strcmp(ast->type, "EQ") == 0)){
		return 11;
	}

	else if ((strcmp(ast->type, "NE") == 0)){
		return 12;
	}

	else if ((strcmp(ast->type, "LE") == 0)){
		return 13;
	}

	else if ((strcmp(ast->type, "LT") == 0)){
		return 14;
	}

	else if ((strcmp(ast->type, "GE") == 0)){
		return 15;
	}

	else if ((strcmp(ast->type, "GT") == 0)){
		return 16;
	}
	return 0;
}



/* 
 * ptr: pointer to an AST for a function definition; 
 * func_def_name() returns a pointer to the function name (a string) of the function definition AST that ptr points to.
 */
char * func_def_name(void *ptr) {
	struct astNode *ast = ptr;
	return ast->name;
}



/*
 * ptr: pointer to an AST for a function definition
 * func_def_nargs() returns the number of formal parameters for that function.
 */
int func_def_nargs(void *ptr) {
	struct astNode *ast = ptr;
	return ast->numParam;
}



/*
 * ptr: pointer to an AST for a function definition, 
 * n: an integer.
 * If n > 0 and n <= no. of arguments for the function, then func_def_argname() returns 
 * a pointer to the name (a string) of the nth formal parameter for that function;
 * the first formal parameter corresponds to n == 1.  If the value of n is outside
 * these parameters, the behavior of this function is undefined.
 */
char *func_def_argname(void *ptr, int n) {
	struct astNode *ast = ptr;
	struct astNode *param = ast->child1;
	while (n > 1) {
		param = param->child3;
		n = n - 1;
	}
	return param->name;
}



/* 
 * ptr: pointer to an AST for a function definition
 * func_def_body() returns a pointer to the AST that is the function body of the function that ptr points to.
 */
void * func_def_body(void *ptr) {
	struct astNode *ast = ptr;
	return ast->child2;
}



/*
 * ptr: pointer to an AST node for a function call
 * func_call_callee() returns a pointer to a string that is the name of the function being called.
 */
char * func_call_callee(void *ptr) {
	struct astNode *ast = ptr;
	return ast->name;
}



/*
 * ptr: pointer to an AST node for a function call
 * func_call_args() returns a pointer to the AST that is the list of arguments to the call.
 */
void * func_call_args(void *ptr) {
	struct astNode *ast = ptr;
	return ast->child1;
}



/*
 * ptr: pointer to an AST node for a statement list
 * stmt_list_head() returns a pointer to the AST of the statement at the beginning of this list.
 */
void * stmt_list_head(void *ptr) {
	struct astNode *ast = ptr;
	return stmtListNext(ast);
}



/*
 * ptr: pointer to an AST node for a statement list
 * stmt_list_rest() returns a pointer to the AST of the rest of this list (i.e., the pointer to the
 * next node in the list).
 */
void * stmt_list_rest(void *ptr) {
	struct astNode *ast = ptr;
	ast->numParam = ast->numParam + 1;
	if (stmtListNext(ast) == NULL) {
		return NULL;
	}

	return ast;
}



/*
 * ptr: pointer to an AST node for an expression list
 * expr_list_head() returns a pointer to the AST of the expression at the beginning of this list.
 */
void * expr_list_head(void *ptr) {
	struct astNode *ast = ptr;
	return exprListNext(ast);
}



/*
 * ptr: pointer to an AST node for an expression list
 * expr_list_rest() returns a pointer to the AST of the rest of this list (i.e., the pointer to the next node in the list).
 */
void * expr_list_rest(void *ptr) {
	struct astNode *ast = ptr;
	ast->numParam = ast->numParam + 1;
	if (exprListNext(ast) == NULL) {
		return NULL;
	}

	struct astNode *next = exprListNext(ast);
	next = next->child3;

	if (next == NULL) {
		return NULL;
	}

	return ast;
}



/*
 * ptr: pointer to an AST node for an IDENTIFIER
 * expr_id_name() returns a pointer to the name of the identifier (a string).
 */
char *expr_id_name(void *ptr) {
	struct astNode *ast = ptr;
	return ast->name;
}



/*
 * ptr: pointer to an AST node for an INTCONST
 * expr_intconst_val() returns the integer value of the constant.
 */
int expr_intconst_val(void *ptr) {
	struct astNode *ast = ptr;
	return ast->numParam;
}



/*
 * ptr: pointer to an AST node for an arithmetic or boolean expression.
 * expr_operand_1() returns a pointer to the AST of the first operand.
 */
void * expr_operand_1(void *ptr) {
	struct astNode *ast = ptr;
	return ast->child1;
}



/*
 * ptr: pointer to an AST node for an arithmetic or boolean expression.
 * expr_operand_2() returns a pointer to the AST of the second operand.
 */
void * expr_operand_2(void *ptr) {
	struct astNode *ast = ptr;
	return ast->child2;
}



/*
 * ptr: pointer to an AST node for an IF statement.  
 * stmt_if_expr() returns a pointer to the AST for the expression tested by the if statement.
 */
void * stmt_if_expr(void *ptr) {
	struct astNode *ast = ptr;
	return ast->child1;
}



/*
 * ptr: pointer to an AST node for an IF statement. 
 * stmt_if_then() returns a pointer to the AST for the then-part of the if statement, i.e., the
 * statement to be executed if the condition is true.
 */
void * stmt_if_then(void *ptr) {
	struct astNode *ast = ptr;
	return ast->child2;
}



/*
 * ptr: pointer to an AST node for an IF statement.  
 * stmt_if_else() returns a pointer to the AST for the else-part of the if statement, i.e., the
 * statement to be executed if the condition is false.
 */
void * stmt_if_else(void *ptr) {
	struct astNode *ast = ptr;
	return ast->ifElse;
}



/*
 * ptr: pointer to an AST node for an assignment statement.  
 * stmt_assg_lhs() returns a pointer to the name of the identifier on the LHS of the assignment.
 */
char *stmt_assg_lhs(void *ptr) {
	struct astNode *ast = ptr;
	return ast->child1->name;
}



/*
 * ptr: pointer to an AST node for an assignment statement.  
 * stmt_assg_rhs() returns a pointer to the AST of the expression on the RHS of the assignment.
 */
void *stmt_assg_rhs(void *ptr) {
	struct astNode *ast = ptr;
	return ast->child2;
}



/*
 * ptr: pointer to an AST node for a while statement.  
 * stmt_while_expr() returns a pointer to the AST of the expression tested by the while statement.
 */
void *stmt_while_expr(void *ptr) {
	struct astNode *ast = ptr;
	return ast->child1;
}



/*
 * ptr: pointer to an AST node for a while statement.  
 * stmt_while_body() returns a pointer to the AST of the body of the while statement.
 */
void *stmt_while_body(void *ptr) {
	struct astNode *ast = ptr;
	return ast->child2;
}



/*
 * ptr: pointer to an AST node for a return statement.  
 * stmt_return_expr() returns a pointer to the AST of the expression whose value is returned.
 */
void *stmt_return_expr(void *ptr) {
	struct astNode *ast = ptr;
	return ast->child1;
}

// ------------------------------------------------------------------------------------------------------------------------
// Symbol Table Helper Functions. Ordered alphabetically

int checkIfCurrentlyGlobal() {
	if (symbolTable->head->next == NULL) {
		return 1;
	}

	return 0;
}



int checkIfFuncDeclared(char *str) {

	struct individualFunc *elements = globalFuncTable->head;

	while (elements != NULL) {

		if (strcmp(str, elements->name) == 0) {
			return 1;
		}

		elements = elements->next;

	}

	return 0;
}



void checkIfFuncValid(char *funcName) {

	struct individualFunc *elements = globalFuncTable->head;

	while (elements != NULL ) {
		if (strcmp(funcName, elements->name) == 0) {
			fprintf(stderr, "Error occurred on line %d\n", getLineNum());
			exit(1);
		}

		elements = elements->next;

	}
}



void checkIfFuncParamNumValid(char *funcName) {

	struct individualFunc *func = globalFuncTable->head;

	while (func != NULL) {

		if (strcmp(funcName, func->name) == 0) {

			if (func->numParameters == currFuncCallParamNum) {
				return;
			}

			else {
				fprintf(stderr, "Error occurred on line %d\n", getLineNum());
				exit(1);
			}

		}

		func = func->next;
	}

	fprintf(stderr, "Error occurred on line %d\n", getLineNum());
	exit(1);
}



void checkIfVarDeclared(char *str) {

		struct individualSymbolTable *topTable = symbolTable->head;

		while (topTable != NULL) {

		struct individualElement *elements = topTable->head;

			while (elements != NULL) {
				if (strcmp(str, elements->name) == 0) {
					return;
				}
				elements = elements->next;
			}

		topTable = topTable->next;
	}

	fprintf(stderr, "Error occurred on line %d\n", getLineNum());
	exit(1);
}



void checkIfVarValid(char *str) {

	struct individualSymbolTable *topTable = symbolTable->head;
	struct individualElement *elements = topTable->head;

	while (elements != NULL) {

		if (strcmp(str, elements->name) == 0) {
			fprintf(stderr, "Error occurred on line %d\n", getLineNum());
			exit(1);
		}


		elements = elements->next;

	}
}



void popTopTable() {

	if (symbolTable->head != NULL) {
		symbolTable->head = symbolTable->head->next;
	}

	else {
		symbolTable->head = NULL;
	}
}



void pushNewGlobalFunc(char *funcName) {

	struct individualFunc *newFunc = malloc(sizeof(struct individualFunc));

	char *funcNameHolder = malloc(strlen(funcName) + 1);
	strcpy(funcNameHolder, funcName);

	newFunc->name = funcNameHolder;
	newFunc->next = globalFuncTable->head;
	globalFuncTable->head = newFunc;

}



void pushNewTable() {

	struct individualSymbolTable *newTable = malloc(sizeof(struct individualSymbolTable));
	newTable->head = NULL;
	newTable->next = symbolTable->head;
	symbolTable->head = newTable;

}



void pushVarToTable(char *str) {

	struct individualElement *element = malloc(sizeof(struct individualElement));
	char *theName = malloc(strlen(str) + 1);
	strcpy(theName, str);
	element->name = theName;
	element->next = NULL;

	struct individualSymbolTable *topTable = symbolTable->head;

	element->next = topTable->head;
	topTable->head = element;
}



void updateFuncNumParameters() {
	struct individualFunc *func = globalFuncTable->head;
	func->numParameters = currNumParameters;
}

// -------------------------------------------------------------------------------------------------------------------------
// Parser Helper Functions. Sorted by the ordering seen in the syntax rules of 
// https://www2.cs.arizona.edu/classes/cs453/spring20/PROJECT/SPEC/G2.html

void match(Token expected) {

	if (curr_tok == expected) {
		// continue
	}

	else {
		fprintf(stderr, "Error occurred on line %d\n", getLineNum());
		exit(1);
	}

}



void matchProg() {
	//matchFuncDefn();
}



void matchVarDeclMinusOne() {

	matchID_List(); // ID

	curr_tok = get_token();

	match(8);
}



void matchID_List() {

	match(1); // ID

	if (chk_decl_flag != 0) {
		checkIfVarValid(getCurrID());
		pushVarToTable(getCurrID());
		
		// This is here because only each identifier needs to be unique within scope
		if (checkIfCurrentlyGlobal() != 0) {
			checkIfFuncValid(getCurrID());
		}
	}

	if (lookahead() == 7) {
		curr_tok = get_token();
		match(7);
		curr_tok = get_token();
		matchID_List();
	}
}



void matchType() {
	match(9);
}



void matchFuncDefnMinusOne() {

	// ID
	match(1);

	// Do semantic checking
	if (chk_decl_flag != 0) {
		checkIfFuncValid(getCurrID());
		pushNewGlobalFunc(getCurrID());
		// This is here because only each identifier needs to be unique within scope
		checkIfVarValid(getCurrID());
	}

	// do ast
	if (print_ast_flag != 0) {

		// Create first node
		firstASTNode = malloc(sizeof(struct astNode));
		firstASTNode->type = "FUNC_DEF";

		// Set the name
		firstASTNode->name = malloc(strlen(globalFuncTable->head->name) + 1);
		strcpy(firstASTNode->name, globalFuncTable->head->name);

		// Set child 1, the func param names, to be none
		struct astNode *firstParam = malloc(sizeof(struct astNode));
		firstParam->name = "none";
		firstASTNode->child1 = firstParam;

		// make second child null
		firstASTNode->child2 = NULL;

		// for adding body
		currNodeAdd = firstASTNode;
	}

	curr_tok = get_token();

	// LPAREN
	match(3);
	curr_tok = get_token();

	// Semantic checking
	if (chk_decl_flag != 0) {
		pushNewTable();
	}

	//opt formals
	matchOptFormals();

	// update parameter num
	if (chk_decl_flag != 0) {
		updateFuncNumParameters();
		currNumParameters = 0;
	}

	// do ast params
	if (print_ast_flag != 0) {
		firstASTNode->numParam = globalFuncTable->head->numParameters;
	}


	// RPAREN
	match(4);
	curr_tok = get_token();

	// LBRACE
	match(5); 
	curr_tok = get_token();

	// opt var decals
	matchOptVarDecals();

	// opt_stmt_list
	matchOptStmtList();

	// RBRACE
	match(6);

	// Print ast if flag is set
	if (print_ast_flag != 0) {	
		print_ast(firstASTNode);
	}

	// Semantic checking
	if (chk_decl_flag != 0) {
		popTopTable();
	}
}



void matchOptFormals() {

	if (curr_tok == 9) {
		matchFormals();	
		curr_tok = get_token();	
	}

	else {
		// epsilon
	}
}



void matchFormals() {

	// type
	match(9);
	curr_tok = get_token();

	// id
	match(1);

	if (chk_decl_flag != 0) {
		checkIfVarValid(getCurrID());
		pushVarToTable(getCurrID());
		currNumParameters += 1;
	}

	// add param to func
	if (print_ast_flag != 0) {
		pushParamName(getCurrID());
	}

	// If comma, repeat
	if (lookahead() == 7) {
		curr_tok = get_token();
		match(7);
		curr_tok = get_token();
		matchFormals();
	}
	

}



void matchOptVarDecals() {

	if (curr_tok == 9) {
		match(9);
		curr_tok = get_token();
		matchVarDeclMinusOne();	
		curr_tok = get_token();
		if (curr_tok == 9) {
			matchOptVarDecals();
		}	
	}

	else {
		// epsilon
	}
}



void matchOptStmtList() {
	while (curr_tok == 1 || curr_tok == 12 || curr_tok == 10 || curr_tok == 13 || curr_tok == 5 || curr_tok == 8) {
		matchStmt();
		curr_tok = get_token();
	}
}



void matchStmt() {

	// ID
	if (curr_tok == 1) {
		if (lookahead() == 14) {
			matchAssgStmt();
		}

		// fncall
		else {
			matchFnCall();
		}
	}

	// while
	else if (curr_tok == 12) {
		matchWhile();
	}

	// if
	else if (curr_tok == 10) {
		matchIf();
	}

	// return
	else if (curr_tok == 13) {
		matchReturn();
	}

	// LBRACE
	else if (curr_tok == 5) {
		inBrackets = inBrackets + 1;
		// LBRACE
		match(5);
		if (print_ast_flag != 0) {
			pushCurrNodeAddStmtList();
		}
		curr_tok = get_token();

		// opt_stmt
		matchOptStmtList();

		// RBRACE
		match(6);
	}

	// SEMI
	else if (curr_tok == 8) {
		match(8);
		if (print_ast_flag != 0) {
			pushCurrNodeAddStmtList();
		}
	}

	// throw error
	else {
		match(10000);
	}
}



void matchIf() {

	// If
	match(10);
	curr_tok = get_token();

	// LPAREN
	match(3);
	curr_tok = get_token();

	// bool_exp
	matchBoolExp();

	if (print_ast_flag != 0) {
		pushCurrNodeAdd();
		currNodeAdd->type = "IF";
		currNodeAdd->child1 = boolExp;
		inInner = 1;
		switchToInner();
	}

	curr_tok = get_token();

	// RPAREN
	match(4);
	curr_tok = get_token();

	// stmt
	matchStmt();


	if (print_ast_flag != 0) {
		inInner = 0;
		switchToOuter();
		if (inBrackets <= 0) {
			convertStmtToSolo(currNodeAdd);
		}
		if (inBrackets > 0) {
			inBrackets = inBrackets - 1;
		}
	}

	// If else
	if (lookahead() == 11) {

		curr_tok = get_token();

		// else
		match(11);
		curr_tok = get_token();

		if (print_ast_flag != 0) {
			inElse = 1;
			switchToInner();
		}

		matchStmt();

		if (print_ast_flag != 0) {
			inElse = 0;
			switchToOuter();
			if (inBrackets <= 0) {
				convertStmtToSoloElse(currNodeAdd);
			}
			if (inBrackets > 0) {
				inBrackets = inBrackets - 1;
			}
		}

	}
}



void matchWhile() {
	// While
	match(12);
	curr_tok = get_token();

	// LPAREN
	match(3);
	curr_tok = get_token();

	// bool_exp
	matchBoolExp();

	if (print_ast_flag != 0) {
		pushCurrNodeAdd();
		currNodeAdd->type = "WHILE";
		currNodeAdd->child1 = boolExp;
		inInner = 1;
		switchToInner();
	}


	curr_tok = get_token();

	//RPAREN
	match(4);
	curr_tok = get_token();

	// stmt
	matchStmt();

	if (print_ast_flag != 0) {
		switchToOuter();
		inInner = 0;
		if (inBrackets <= 0) {
			convertStmtToSolo(currNodeAdd);
		}
		if (inBrackets > 0) {
			inBrackets = inBrackets - 1;
		}
	}
}



void matchReturn() {

	// return
	match(13);
	curr_tok = get_token();

	if (print_ast_flag != 0) {
		pushCurrNodeAdd();
		currNodeAdd->type = "RETURN";
		currNodeAdd->child1 = NULL;
	}

	// semi
	if (curr_tok == 8) {
		match(8);
	}

	// arith expression
	else if (curr_tok == 1 || curr_tok == 2) {

		// arith expression
		matchArithExp();
		if (print_ast_flag != 0) {
			struct astNode *returnVal = malloc(sizeof(struct astNode));
			returnVal->type = "IDENTIFIER";
			returnVal->name = malloc(sizeof(getCurrID()) + 1);
			strcpy(returnVal->name, getCurrID());
			currNodeAdd->child1 = returnVal;
		}
		curr_tok = get_token();

		// semi
		match(8);

	}

	else {
		fprintf(stderr, "Error occurred on line %d\n", getLineNum());
		exit(1);
	}
}



void matchAssgStmt() {

	// ID
	match(1);

	if (chk_decl_flag != 0) {
		checkIfVarDeclared(getCurrID());
	}

	if (print_ast_flag != 0) {
		pushCurrNodeAdd();
		currNodeAdd->type = "ASSG";
		currNodeAdd->child1 = malloc(sizeof(struct astNode));
		currNodeAdd->child1->name = malloc(strlen(getCurrID()) + 1);
		strcpy(currNodeAdd->child1->name, getCurrID());
		currNodeAdd->child1->type = "IDENTIFIER";
	}


	curr_tok = get_token();

	// opASSG
	match(14);
	curr_tok = get_token();

	// arith_exp
	matchArithExp();

	if (print_ast_flag != 0) {
		currNodeAdd->child2 = malloc(sizeof(struct astNode));
		currNodeAdd->child2->name = malloc(strlen(getCurrID()) + 1);
		strcpy(currNodeAdd->child2->name, getCurrID());
		currNodeAdd->child2->type = "IDENTIFIER";
	}

	curr_tok = get_token();

	match(8);

}



void matchFnCall() {

	match(1); // ID

	if (chk_decl_flag != 0) {

		// remember name of func
		currFuncID = malloc(sizeof(getCurrID()) + 1);
		strcpy(currFuncID, getCurrID());

		// Check if there's a local var that has the same name
		checkIfVarValid(getCurrID());

		if(checkIfFuncDeclared(getCurrID()) == 0) {
			fprintf(stderr, "Error occurred on line %d\n", getLineNum());
			exit(1);
		}
	}

	if (print_ast_flag != 0) {
		pushCurrNodeAdd();
		currNodeAdd->type = "FUNC_CALL";
		currNodeAdd->name = malloc(strlen(getCurrID()) + 1);
		strcpy(currNodeAdd->name, getCurrID());
		struct astNode *exprList = malloc(sizeof(struct astNode));
		exprList->type = "EXPR_LIST";
		exprList->numParam = 0;
		currNodeAdd->child1 = exprList;
	}

	curr_tok = get_token();

	match(3); // LPAREN
	curr_tok = get_token();

	matchOptExprList();

	if (chk_decl_flag != 0) {
		checkIfFuncParamNumValid(currFuncID);
		currFuncCallParamNum = 0;
	}

	match(4); // RPAREN

	if (lookahead() == 5) {
		fprintf(stderr, "Error occurred on line %d\n", fixLineNumError);
		exit(1);
	}

	curr_tok = get_token();

	match(8); // semi

}



void matchOptExprList() {

	if (curr_tok == 1 || curr_tok == 2) {
		matchExprList();
	}

	else {
		// epsilon
	}
}



void matchExprList() {

	// arith
	matchArithExp();


	if (print_ast_flag != 0) {
		pushFuncCallArg(getCurrID());
	}

	curr_tok = get_token();

	if (chk_decl_flag != 0) {
		currFuncCallParamNum += 1;
	}

	// if comma
	while (curr_tok == 7) {

		// comma
		match(7);
		curr_tok = get_token();

		// recurse
		matchExprList();

	}
}



void matchBoolExp() {

	struct astNode *child1 = malloc(sizeof(struct astNode));

	// arith exp
	matchArithExp();

	if (print_ast_flag != 0) {
		child1 = malloc(sizeof(struct astNode));
		child1->name = malloc(strlen(getCurrID()) + 1);
		strcpy(child1->name, getCurrID());
		child1->type = "IDENTIFIER";
	}

	curr_tok = get_token();

	// relop
	matchRelop();

	if (print_ast_flag != 0) {
		boolExp->child1 = child1;
	}

	curr_tok = get_token();

	// arith exp
	matchArithExp();

	if (print_ast_flag != 0) {
		struct astNode *child2 = malloc(sizeof(struct astNode));
		child2->name = malloc(strlen(getCurrID()) + 1);
		strcpy(child2->name, getCurrID());
		child2->type = "IDENTIFIER";
		boolExp->child2 = child2;
	}
}



void matchArithExp() {

	// ID
	if (curr_tok == 1) {

		if (chk_decl_flag != 0) {
			checkIfVarDeclared(getCurrID());
		}

		match(1);
	}

	// INTCON
	else if (curr_tok == 2) {
		match(2);
	}

	// Neither, so error
	else {
		match(1100000);
	}
}



void matchRelop() {

	// opeq
	if (curr_tok == 19) {
		if (print_ast_flag != 0) {
			boolExp = malloc(sizeof(struct astNode));
			boolExp->type = "EQ";
		}
		match(19);
	}

	// opne
	else if (curr_tok == 20) {
		if (print_ast_flag != 0) {
			boolExp = malloc(sizeof(struct astNode));
			boolExp->type = "NE";
		}
		match(20);
	}

	// ople
	else if (curr_tok == 24) {
		if (print_ast_flag != 0) {
			boolExp = malloc(sizeof(struct astNode));
			boolExp->type = "LE";
		}
		match(24);
	}

	// oplt
	else if (curr_tok == 23) {
		if (print_ast_flag != 0) {
			boolExp = malloc(sizeof(struct astNode));
			boolExp->type = "LT";
		}
		match(23);
	}

	// opgt
	else if (curr_tok == 21) {
		if (print_ast_flag != 0) {
			boolExp = malloc(sizeof(struct astNode));
			boolExp->type = "GT";
		}
		match(21);
	}

	// opge
	else if (curr_tok == 22) {
		if (print_ast_flag != 0) {
			boolExp = malloc(sizeof(struct astNode));
			boolExp->type = "GE";
		}
		match(22);
	}

	// error
	else {
		match(-1000);
	}

}



int parse() {

	// Create the global symbol table
	symbolTable = malloc(sizeof(struct symbolTableStack));
	symbolTable->head = malloc(sizeof(struct individualSymbolTable));
	symbolTable->head->head = NULL;
	symbolTable->head->next = NULL;

	// Create the global function table
	globalFuncTable = malloc(sizeof(struct individualFuncTable));
	globalFuncTable->head = NULL;
	globalFuncTable->next = NULL;


	// While there is file to read
	while((curr_tok = get_token()) != EOF) {

		// if int: type, func defn, var_decl
		if (curr_tok == 9) {

			// if next is type
			if (lookahead() == 1) {

				// move to next
				curr_tok = get_token();

				// If left paren: then it is func_defn
				if (lookahead() == 3) {
					matchFuncDefnMinusOne();
				}

				// If not left paren: then it is var decl
				else {
					matchVarDeclMinusOne();
				}	
			}	

			// else, it is type
			else {
				matchType();
			}			
		}

		// ID
		else if (curr_tok == 1) {

			// Assg stmt
			if (lookahead() == 14) {
				matchAssgStmt();
			}

			// Fn Call
			else if (lookahead() == 3)  {
				fixLineNumError = getLineNum();
				matchFnCall();
			}

			// else error
			else {
				match(1000);
			}
		}

		// If
		else if (curr_tok == 10) {
			matchIf();
		}

		// else
		else if (curr_tok == 11) {
			
		}

		// While
		else if (curr_tok == 12) {
			matchWhile();
		}

		// Return
		else if (curr_tok == 13) {
			matchReturn();
		}

		else {
			fprintf(stderr, "Error occurred on line %d\n", getLineNum());
			exit(1);
		}

	}

	return 0;
}