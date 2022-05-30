#ifndef PROGRAM5_H
#define PROGRAM5_H

int get_token();
int getLineNum();
int lookahead();
int isNumeric(char ch);
char * getCurrID();

int parse();
void matchFnCall();

void matchProg();
void matchVarDeclMinusOne();
void matchID_List();
void matchType();
void matchFuncDefnMinusOne();
void matchOptFormals();
void matchFormals();
void matchOptVarDecals();
void matchOptStmtList();
void matchStmt();
void matchIf();
void matchWhile();
void matchReturn();
void matchAssgStmt();
void matchFnCall();
void matchOptExprList();
void matchExprList();
void matchBoolExp();
void matchArithExp();
void matchRelop();

struct astNode {
	char *type;
	char *name;
	int numParam;
	struct astNode *child1;
	struct astNode *child2;
	struct astNode *child3;
	struct astNode *ifElse;
};

struct innerHolder {
	struct astNode *ast;
	struct innerHolder *next;
};


struct symbolTableStack {
	struct individualSymbolTable *head;
};

struct individualSymbolTable {
	struct individualElement *head;
	struct individualSymbolTable *next;
};

struct individualElement {
	char *name;
	struct individualElement *next;
};

struct individualFuncTable {
	struct individualFunc *head;
	struct funcTable *next;
};

struct individualFunc {
	char *name;
	int numParameters;
	struct individualFunc *next;
};

#endif