#ifndef YYERRCODE
#define YYERRCODE 256
#endif

#define IDENTIFIER 257
#define QSTRING 258
#define MODE 259
#define PROMPT 260
#define COMMAND 261
#define FUNC 262
#define AVAIL 263
#define ARG 264
#define NAME 265
#define PRIORITY 266
#define VALUE 267
#define PARENT 268
#define DESC 269
#define BOOL 270
#define NUMBER 271
#define STRING 272
#define HEX 273
#define IPADDR 274
#define NETWORKMASK 275
#define MACADDR 276
#define RANGE 277
#define OPTIONAL 278
#define MANDATORY 279
typedef union 
{
    char	*string;
    SymTable	stptr;
} YYSTYPE;
extern YYSTYPE yylval;
