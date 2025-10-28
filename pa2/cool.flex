/*
 *  The scanner definition for COOL.
 */

/*
 *  Stuff enclosed in %{ %} in the first section is copied verbatim to the
 *  output, so headers and global definitions are placed here to be visible
 * to the code in the file.  Don't remove anything that was here initially
 */
%{
#include <cool-parse.h>
#include <stringtab.h>
#include <utilities.h>

/* The compiler assumes these identifiers. */
#define yylval cool_yylval
#define yylex  cool_yylex

/* Max size of string constants */
#define MAX_STR_CONST 1025
#define YY_NO_UNPUT   /* keep g++ happy */

extern FILE *fin; /* we read from this file */

/* define YY_INPUT so we read from the FILE fin:
 * This change makes it possible to use this scanner in
 * the Cool compiler.
 */
#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) \
	if ( (result = fread( (char*)buf, sizeof(char), max_size, fin)) < 0) \
		YY_FATAL_ERROR( "read() in flex scanner failed");

extern int curr_lineno;
extern int verbose_flag;

extern YYSTYPE cool_yylval;

%}

%option noyywrap

%x STRING
%{
char string_buf[MAX_STR_CONST]; /* to assemble string constants */
char *string_buf_ptr;
%}

%x COMMENT
%{
static int comment_level;
%}

/*
 * Define names for regular expressions here.
 */
DARROW		=>
ASSIGN		<-
LE		<=
UPPER		[A-Z]
LOWER		[a-z]
DIGIT		[0-9]

%%
 /*
  *  Blank characters and line numbers.
  */

^[ \t]*\#[ \t]*[0-9]+[ \t]+\"[^\n"]+\"[^\n]*\n   {
    int ln=0; char fname[1024]={0};
    (void)sscanf(yytext, " #%d \"%1023[^\"]\"", &ln, fname);
    if (ln > 0) curr_lineno = ln;
}

[ \f\r\t\v]+        {}
\n                  { curr_lineno++; }

 /*
  * Keywords are case-insensitive except for the values true and false,
  * which must begin with a lower-case letter.
  */
[cC][lL][aA][sS][sS]			{ return CLASS; }
[iI][nN][hH][eE][rR][iI][tT][sS]	{ return INHERITS; }
[nN][eE][wW]				{ return NEW; }
[iI][sS][vV][oO][iI][dD]		{ return ISVOID; }
[iI][fF]				{ return IF; }
[tT][hH][eE][nN]			{ return THEN; }
[eE][lL][sS][eE]			{ return ELSE; }
[fF][iI]				{ return FI; }
[wW][hH][iI][lL][eE]			{ return WHILE; }
[lL][oO][oO][pP]			{ return LOOP; }
[pP][oO][oO][lL]			{ return POOL; }
[lL][eE][tT]				{ return LET; }
[iI][nN]				{ return IN; }
[cC][aA][sS][eE]			{ return CASE; }
[oO][fF]				{ return OF; }
[eE][sS][aA][cC]			{ return ESAC; }
[nN][oO][tT]				{ return NOT; }

t[rR][uU][eE]				{ cool_yylval.boolean = 1; return BOOL_CONST; }
f[aA][lL][sS][eE]			{ cool_yylval.boolean = 0; return BOOL_CONST; }

{UPPER}({UPPER}|{LOWER}|{DIGIT}|_)*	{ 
                                        	cool_yylval.symbol = idtable.add_string(yytext); 
                                        	return TYPEID; 
					}
{LOWER}({UPPER}|{LOWER}|{DIGIT}|_)*   	{ 
                                        	cool_yylval.symbol = idtable.add_string(yytext); 
                                        	return OBJECTID; 
                                      	}

{DIGIT}+            			{ 
                        			cool_yylval.symbol = inttable.add_string(yytext); 
                        			return INT_CONST; 
                    			}

 /*
  *  Operators.
  */
{DARROW}		{ return DARROW; }
{ASSIGN}		{ return ASSIGN; }
{LE}			{ return LE; }
[+\-*/~<.=@,:;(){}]	{ return yytext[0]; }

 /*
  *  Comments
  */
"--".*              { }

"(*"			{ BEGIN(COMMENT); comment_level = 1; }
<COMMENT>"(*"		{ comment_level++; }
<COMMENT>"*)"		{ 
                		comment_level--; 
                		if (comment_level == 0) BEGIN(INITIAL); 
                	}
<COMMENT><<EOF>>	{ 
				cool_yylval.error_msg = "File ended before comment was closed";
				BEGIN(INITIAL);
				return ERROR;
                	}
<COMMENT>\n		{ curr_lineno++; }
<COMMENT>.		{ }

"*)"			{ 
				cool_yylval.error_msg = "Dangling comment terminator '*)' found"; 
				return ERROR; 
			}

 /*
  *  String constants (C syntax)
  *  Escape sequence \c is accepted for all characters c. Except for 
  *  \n \t \b \f, the result is c.
  */

\"                  { BEGIN(STRING); string_buf_ptr = string_buf; }

<STRING>\"		{ 
				BEGIN(INITIAL);
				*string_buf_ptr = '\0';
				if (string_buf_ptr - string_buf >= MAX_STR_CONST - 1) {
					cool_yylval.error_msg = "String literal exceeds maximum length";
					return ERROR;
				}
				cool_yylval.symbol = stringtable.add_string(string_buf);
				return STR_CONST;
                    	}

<STRING>\n		{
          			curr_lineno++;
          			BEGIN(INITIAL);
          			cool_yylval.error_msg = "Unclosed string literal detected";
          			return ERROR;
          		}

<STRING><<EOF>> 	{
				BEGIN(INITIAL);
				cool_yylval.error_msg = "Unexpected EOF while scanning string";
				return ERROR;
			}

<STRING>\\n		{ *string_buf_ptr++ = '\n'; }
<STRING>\\t		{ *string_buf_ptr++ = '\t'; }
<STRING>\\b		{ *string_buf_ptr++ = '\b'; }
<STRING>\\f		{ *string_buf_ptr++ = '\f'; }
<STRING>\\.		{ *string_buf_ptr++ = yytext[1]; }

<STRING>.		{
				if (yytext[0] == '\0') {
					cool_yylval.error_msg = "String literal includes null ('\\0') character";
					BEGIN(INITIAL);
					return ERROR;
				}
				if (string_buf_ptr - string_buf >= MAX_STR_CONST - 1) {
					cool_yylval.error_msg = "String literal exceeds maximum length";
					BEGIN(INITIAL);
					return ERROR;
				}
				*string_buf_ptr++ = yytext[0];
			}

. 			{ 
                		cool_yylval.error_msg = yytext; 
                		return ERROR; 
            		}
%%