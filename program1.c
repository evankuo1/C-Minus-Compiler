#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "scanner.h"
#include "program1.h"

// Declare lexeme and ch
char *lexeme;
char ch;

// Line number 
int lineNum = 1;

// lookahead 
Token lookaheadToken = -100;
int currentlyLookahead = 0;
int tempLineCount = 0;

// curr id
char currID[1024];

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
