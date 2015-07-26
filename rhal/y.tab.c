#ifndef lint
static char const 
yyrcsid[] = "$FreeBSD: src/usr.bin/yacc/skeleton.c,v 1.28 2000/01/17 02:04:06 bde Exp $";
#endif
#include <stdlib.h>
#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define YYLEX yylex()
#define YYEMPTY -1
#define yyclearin (yychar=(YYEMPTY))
#define yyerrok (yyerrflag=0)
#define YYRECOVERING() (yyerrflag!=0)
static int yygrowstack();
#define YYPREFIX "yy"
#line 34 "rhal_parser.y"

#include <stdio.h>
#include <string.h>

#include "rhal.h"
#include "rhal_symbol_table.h"
#include "graph.h"
#include "misc.h"

extern CLIMatrix	matrix;

#line 47 "rhal_parser.y"
typedef union 
{
    char	*string;
    SymTable	stptr;
} YYSTYPE;
#line 35 "y.tab.c"
#define YYERRCODE 256
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
const short yylhs[] = {                                        -1,
    0,    0,    6,    5,    7,    7,    7,   10,    8,    9,
    9,   12,   11,   17,   13,   19,   14,   15,   15,   20,
   21,   20,   18,   18,   22,   23,   22,   24,   25,   29,
   25,   30,   26,    3,    3,    3,    3,    3,    3,    3,
    3,   31,   27,    4,    4,   28,   32,   28,    2,    1,
   16,   33,   16,
};
const short yylen[] = {                                         2,
    1,    2,    0,    6,    1,    1,    2,    0,    5,    1,
    2,    0,    6,    0,    5,    0,    5,    1,    2,    0,
    0,    5,    1,    2,    0,    0,    6,    5,    0,    0,
    5,    0,    5,    1,    1,    1,    1,    1,    1,    1,
    1,    0,    5,    1,    1,    0,    0,    5,    1,    1,
    0,    0,    5,
};
const short yydefred[] = {                                      0,
    0,    0,    1,   49,    3,    2,    0,    0,    0,    0,
    0,    0,    0,   10,    0,   12,    4,    0,   11,   50,
    8,    0,    0,    0,    9,    0,    0,    0,    0,   13,
    0,    0,   18,   16,    0,    0,   19,    0,   21,    0,
    0,   17,    0,    0,    0,    0,   23,   22,   52,   26,
   24,    0,    0,   53,    0,    0,    0,    0,    0,   27,
    0,    0,   30,    0,    0,    0,    0,   34,   36,   35,
   37,   38,   39,   40,   41,   32,    0,    0,    0,   31,
    0,   44,   45,   42,    0,   28,   33,    0,   47,   43,
    0,   48,
};
const short yydgoto[] = {                                       2,
   21,    5,   76,   84,    3,    7,   11,   12,   13,   23,
   14,   22,   27,   28,   32,   41,   36,   46,   38,   33,
   43,   47,   53,   57,   58,   62,   66,   79,   67,   81,
   88,   91,   52,
};
const short yysindex[] = {                                   -238,
 -235, -238,    0,    0,    0,    0,  -99, -245,  -36, -235,
  -98, -233, -233,    0, -232,    0,    0, -233,    0,    0,
    0,  -93,  -28, -230,    0,  -27,  -92, -228, -235,    0,
  -25, -228,    0,    0, -235, -231,    0,  -22,    0,  -21,
 -223,    0,  -17, -232, -235, -223,    0,    0,    0,    0,
    0,  -16,  -79,    0, -220,  -15,  -78, -219, -232,    0,
  -12, -216,    0, -264,  -10, -215,   -7,    0,    0,    0,
    0,    0,    0,    0,    0,    0, -261,   -5, -231,    0,
   -4,    0,    0,    0, -235,    0,    0,   -2,    0,    0,
   -1,    0,
};
const short yyrindex[] = {                                      0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,  -71,  -66,    0,    0,    0,    0,  -65,    0,    0,
    0,    0,    0,    0,    0,    0,    0, -125,    0,    0,
    0, -124,    0,    0,    0, -122,    0,    0,    0,    0,
  -64,    0,    0,    0,    0,  -63,    0,    0,    0,    0,
    0,    0,    0,    0, -204,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0, -123,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,  -61,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,
};
const short yygindex[] = {                                      0,
  -39,   -6,    0,    0,   63,    0,    0,    0,   54,    0,
    1,    0,    0,    0,    0,  -11,    0,    0,    0,   35,
    0,   23,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,
};
#define YYTABLESIZE 146
const short yytable[] = {                                      20,
   14,   46,   51,   16,   49,   68,   69,   70,   71,   72,
   73,   74,   75,   19,    9,   10,   82,   83,   19,   63,
    1,    4,   34,    8,   15,   20,   17,   10,   39,   24,
   25,   26,   30,   29,   31,   35,   42,   40,   50,   44,
   45,   48,   54,   55,   56,   59,   60,   61,   64,   65,
   77,   80,   78,    5,   87,   85,   90,   92,    6,    7,
   25,   15,   29,   51,    6,   18,   37,   86,   51,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   89,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   20,   14,
    0,   51,    0,   20,   14,   46,
};
const short yycheck[] = {                                     125,
  125,  125,  125,   10,   44,  270,  271,  272,  273,  274,
  275,  276,  277,   13,  260,  261,  278,  279,   18,   59,
  259,  257,   29,  123,   61,  258,  125,  261,   35,  123,
   59,  262,  125,   61,  263,   61,   59,  269,   45,   61,
  264,   59,   59,  123,  265,   61,  125,  267,   61,  266,
   61,   59,  268,  125,   59,   61,   59,   59,  125,  125,
  125,  125,  267,  125,    2,   12,   32,   79,   46,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   85,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  264,  264,
   -1,  264,   -1,  269,  269,  269,
};
#define YYFINAL 2
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 279
#if YYDEBUG
const char * const yyname[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"';'",0,"'='",0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,"'{'",0,"'}'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"IDENTIFIER",
"QSTRING","MODE","PROMPT","COMMAND","FUNC","AVAIL","ARG","NAME","PRIORITY",
"VALUE","PARENT","DESC","BOOL","NUMBER","STRING","HEX","IPADDR","NETWORKMASK",
"MACADDR","RANGE","OPTIONAL","MANDATORY",
};
const char * const yyrule[] = {
"$accept : mode_sections",
"mode_sections : mode_section",
"mode_sections : mode_sections mode_section",
"$$1 :",
"mode_section : MODE name $$1 '{' mode_attrs '}'",
"mode_attrs : prompt_section",
"mode_attrs : cmd_sections",
"mode_attrs : prompt_section cmd_sections",
"$$2 :",
"prompt_section : PROMPT '=' cstring $$2 ';'",
"cmd_sections : cmd_section",
"cmd_sections : cmd_sections cmd_section",
"$$3 :",
"cmd_section : COMMAND name $$3 '{' cmd_attrs '}'",
"$$4 :",
"cmd_attrs : func_section avail_sections $$4 desc_section arg_sections",
"$$5 :",
"func_section : FUNC '=' name $$5 ';'",
"avail_sections : avail_section",
"avail_sections : avail_sections avail_section",
"avail_section :",
"$$6 :",
"avail_section : AVAIL '=' name $$6 ';'",
"arg_sections : arg_section",
"arg_sections : arg_sections arg_section",
"arg_section :",
"$$7 :",
"arg_section : ARG name $$7 '{' arg_attrs '}'",
"arg_attrs : name_section value_section prio_section parent_section desc_section",
"name_section :",
"$$8 :",
"name_section : NAME '=' cstring $$8 ';'",
"$$9 :",
"value_section : VALUE '=' value_expr $$9 ';'",
"value_expr : BOOL",
"value_expr : STRING",
"value_expr : NUMBER",
"value_expr : HEX",
"value_expr : IPADDR",
"value_expr : NETWORKMASK",
"value_expr : MACADDR",
"value_expr : RANGE",
"$$10 :",
"prio_section : PRIORITY '=' prio_expr $$10 ';'",
"prio_expr : OPTIONAL",
"prio_expr : MANDATORY",
"parent_section :",
"$$11 :",
"parent_section : PARENT '=' name $$11 ';'",
"name : IDENTIFIER",
"cstring : QSTRING",
"desc_section :",
"$$12 :",
"desc_section : DESC '=' cstring $$12 ';'",
};
#endif
#if YYDEBUG
#include <stdio.h>
#endif
#ifdef YYSTACKSIZE
#undef YYMAXDEPTH
#define YYMAXDEPTH YYSTACKSIZE
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 10000
#define YYMAXDEPTH 10000
#endif
#endif
#define YYINITSTACKSIZE 200
int yydebug;
int yynerrs;
int yyerrflag;
int yychar;
short *yyssp;
YYSTYPE *yyvsp;
YYSTYPE yyval;
YYSTYPE yylval;
short *yyss;
short *yysslim;
YYSTYPE *yyvs;
int yystacksize;
#line 171 "rhal_parser.y"


void enable_yacc_debug() {
    #if YYDEBUG != 0
        yydebug = 1;
    #endif
}
#line 269 "y.tab.c"
/* allocate initial stack or double stack size, up to YYMAXDEPTH */
static int yygrowstack()
{
    int newsize, i;
    short *newss;
    YYSTYPE *newvs;

    if ((newsize = yystacksize) == 0)
        newsize = YYINITSTACKSIZE;
    else if (newsize >= YYMAXDEPTH)
        return -1;
    else if ((newsize *= 2) > YYMAXDEPTH)
        newsize = YYMAXDEPTH;
    i = yyssp - yyss;
    newss = yyss ? (short *)realloc(yyss, newsize * sizeof *newss) :
      (short *)malloc(newsize * sizeof *newss);
    if (newss == NULL)
        return -1;
    yyss = newss;
    yyssp = newss + i;
    newvs = yyvs ? (YYSTYPE *)realloc(yyvs, newsize * sizeof *newvs) :
      (YYSTYPE *)malloc(newsize * sizeof *newvs);
    if (newvs == NULL)
        return -1;
    yyvs = newvs;
    yyvsp = newvs + i;
    yystacksize = newsize;
    yysslim = yyss + newsize - 1;
    return 0;
}

#define YYABORT goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR goto yyerrlab

#ifndef YYPARSE_PARAM
#if defined(__cplusplus) || __STDC__
#define YYPARSE_PARAM_ARG void
#define YYPARSE_PARAM_DECL
#else	/* ! ANSI-C/C++ */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif	/* ANSI-C/C++ */
#else	/* YYPARSE_PARAM */
#ifndef YYPARSE_PARAM_TYPE
#define YYPARSE_PARAM_TYPE void *
#endif
#if defined(__cplusplus) || __STDC__
#define YYPARSE_PARAM_ARG YYPARSE_PARAM_TYPE YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else	/* ! ANSI-C/C++ */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL YYPARSE_PARAM_TYPE YYPARSE_PARAM;
#endif	/* ANSI-C/C++ */
#endif	/* ! YYPARSE_PARAM */

int
yyparse (YYPARSE_PARAM_ARG)
    YYPARSE_PARAM_DECL
{
    register int yym, yyn, yystate;
#if YYDEBUG
    register const char *yys;

    if ((yys = getenv("YYDEBUG")))
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
#endif

    yynerrs = 0;
    yyerrflag = 0;
    yychar = (-1);

    if (yyss == NULL && yygrowstack()) goto yyoverflow;
    yyssp = yyss;
    yyvsp = yyvs;
    *yyssp = yystate = 0;

yyloop:
    if ((yyn = yydefred[yystate])) goto yyreduce;
    if (yychar < 0)
    {
        if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, reading %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
    }
    if ((yyn = yysindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: state %d, shifting to state %d\n",
                    YYPREFIX, yystate, yytable[yyn]);
#endif
        if (yyssp >= yysslim && yygrowstack())
        {
            goto yyoverflow;
        }
        *++yyssp = yystate = yytable[yyn];
        *++yyvsp = yylval;
        yychar = (-1);
        if (yyerrflag > 0)  --yyerrflag;
        goto yyloop;
    }
    if ((yyn = yyrindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag) goto yyinrecovery;
#if defined(lint) || defined(__GNUC__)
    goto yynewerror;
#endif
yynewerror:
    yyerror("syntax error");
#if defined(lint) || defined(__GNUC__)
    goto yyerrlab;
#endif
yyerrlab:
    ++yynerrs;
yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if ((yyn = yysindex[*yyssp]) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == YYERRCODE)
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: state %d, error recovery shifting\
 to state %d\n", YYPREFIX, *yyssp, yytable[yyn]);
#endif
                if (yyssp >= yysslim && yygrowstack())
                {
                    goto yyoverflow;
                }
                *++yyssp = yystate = yytable[yyn];
                *++yyvsp = yylval;
                goto yyloop;
            }
            else
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: error recovery discarding state %d\n",
                            YYPREFIX, *yyssp);
#endif
                if (yyssp <= yyss) goto yyabort;
                --yyssp;
                --yyvsp;
            }
        }
    }
    else
    {
        if (yychar == 0) goto yyabort;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, error recovery discards token %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
        yychar = (-1);
        goto yyloop;
    }
yyreduce:
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: state %d, reducing by rule %d (%s)\n",
                YYPREFIX, yystate, yyn, yyrule[yyn]);
#endif
    yym = yylen[yyn];
    yyval = yyvsp[1-yym];
    switch (yyn)
    {
case 3:
#line 70 "rhal_parser.y"
{ matrix = graph_mode_insert(yyvsp[0].string); }
break;
case 8:
#line 79 "rhal_parser.y"
{ matrix = graph_mode_insert_prompt(yyvsp[0].string); }
break;
case 12:
#line 87 "rhal_parser.y"
{ matrix = graph_cmd_insert(yyvsp[0].string); }
break;
case 14:
#line 92 "rhal_parser.y"
{ printf(""); }
break;
case 16:
#line 95 "rhal_parser.y"
{ matrix = graph_cmd_insert_func(yyvsp[0].string); }
break;
case 21:
#line 103 "rhal_parser.y"
{ matrix = graph_cmd_insert_avail(yyvsp[0].string); }
break;
case 26:
#line 111 "rhal_parser.y"
{ matrix = graph_arg_insert(yyvsp[0].string); }
break;
case 30:
#line 119 "rhal_parser.y"
{ matrix = graph_arg_insert_name(yyvsp[0].string); }
break;
case 32:
#line 122 "rhal_parser.y"
{ 
                                 if (yyvsp[0].stptr == (SymTable)NULL) {
                                     TRS_ERR("bison: Error while parsing value_section");
                                     fprintf(FD_OUT, "bison: Error while parsing value_section\n");
                                 }
                                 else
                                     matrix = graph_arg_insert_value(yyvsp[0].stptr->type);
                               }
break;
case 41:
#line 139 "rhal_parser.y"
{ yyval.stptr = yyvsp[0].stptr; }
break;
case 42:
#line 142 "rhal_parser.y"
{ 
				if (yyvsp[0].stptr == (SymTable)NULL) {
                                     TRS_ERR("bison: Error while parsing prio_section");
                                     fprintf(FD_OUT, "bison: Error while parsing prio_section\n");
                                 }
				else
				    matrix = graph_arg_insert_prio(yyvsp[0].stptr->type); 
			     }
break;
case 45:
#line 153 "rhal_parser.y"
{ yyval.stptr = yyvsp[0].stptr; }
break;
case 47:
#line 158 "rhal_parser.y"
{ matrix = graph_arg_insert_parent(yyvsp[0].string); }
break;
case 49:
#line 161 "rhal_parser.y"
{ yyval.string = yyvsp[0].string; }
break;
case 50:
#line 164 "rhal_parser.y"
{ yyval.string = yyvsp[0].string; }
break;
case 52:
#line 168 "rhal_parser.y"
{ matrix = graph_insert_desc(yyvsp[0].string); }
break;
#line 542 "y.tab.c"
    }
    yyssp -= yym;
    yystate = *yyssp;
    yyvsp -= yym;
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: after reduction, shifting from state 0 to\
 state %d\n", YYPREFIX, YYFINAL);
#endif
        yystate = YYFINAL;
        *++yyssp = YYFINAL;
        *++yyvsp = yyval;
        if (yychar < 0)
        {
            if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
            if (yydebug)
            {
                yys = 0;
                if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
                if (!yys) yys = "illegal-symbol";
                printf("%sdebug: state %d, reading %d (%s)\n",
                        YYPREFIX, YYFINAL, yychar, yys);
            }
#endif
        }
        if (yychar == 0) goto yyaccept;
        goto yyloop;
    }
    if ((yyn = yygindex[yym]) && (yyn += yystate) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yystate)
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: after reduction, shifting from state %d \
to state %d\n", YYPREFIX, *yyssp, yystate);
#endif
    if (yyssp >= yysslim && yygrowstack())
    {
        goto yyoverflow;
    }
    *++yyssp = yystate;
    *++yyvsp = yyval;
    goto yyloop;
yyoverflow:
    yyerror("yacc stack overflow");
yyabort:
    return (1);
yyaccept:
    return (0);
}
