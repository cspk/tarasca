/*
 * rhal_lexel.l --
 *
 * Author -- Pedro Aguilar <paguilar@junkerhq.net>
 *
 * Copyright (c) 2004 - 2006 Pedro Aguilar
 *
 * Description: This is a Flex file in which the lexical analysis (scan) 
 *     for the command generation is done.
 *     The scanner returns a token in the yytext variable.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

%option nounput
%option noyywrap
 */

%{

#include <ctype.h>
#include <stdio.h>

#include "rhal.h"
#include "rhal_symbol_table.h"
#include "y.tab.h"
#include "misc.h"

extern int line_count;

%}

identifier_start	[A-Za-z_]
identifier_end		[\055]*[A-Za-z_0-9\$]
identifier		{identifier_start}{identifier_end}*|[\?]
qstr			\"[^\"\n]*[\"\r\n]
comment			#.*
space			[ \t\r]+
nl			[\n]

%%

{space}			/* Eat up spaces */
{comment}		/* Eat up comments */

{qstr}			{ 
			    if(yytext[yyleng - 1] != '"') {
                                TRS_ERR("flex: Error while scanning, string not quoted: %s", yytext);
                                fprintf(FD_OUT, "flex: Error while parsing, string not quoted: %s\n", yytext);
			    }
			    else {
				yylval.string = yytext;
				return QSTRING;
			    }
			}

mode			{ yylval.stptr = SymLook(yytext); 
                          return MODE; 
                        }
prompt			{ yylval.stptr = SymLook(yytext); 
                          return PROMPT; 
                        }

command			{ yylval.stptr = SymLook(yytext); 
                          return COMMAND; 
                        }
func			{ yylval.stptr = SymLook(yytext); 
                          return FUNC; 
                        }
avail			{ yylval.stptr = SymLook(yytext); 
                          return AVAIL; 
                        }
arg			{ yylval.stptr = SymLook(yytext); 
                          return ARG; 
                        }
name			{ yylval.stptr = SymLook(yytext); 
                          return NAME; 
                        }
value			{ yylval.stptr = SymLook(yytext); 
                          return VALUE; 
                        }
prio			{ yylval.stptr = SymLook(yytext); 
                          return PRIORITY; 
                        }
parent			{ yylval.stptr = SymLook(yytext); 
                          return PARENT; 
                        }
desc			{ yylval.stptr = SymLook(yytext); 
                          return DESC; 
                        }

bool			{ yylval.stptr = SymLook(yytext); 
                          return BOOL; 
                        }
number			{ yylval.stptr = SymLook(yytext); 
                          return NUMBER; 
                        }
string			{ yylval.stptr = SymLook(yytext); 
                          return STRING; 
                        }
hex			{ yylval.stptr = SymLook(yytext); 
                          return HEX; 
                        }
ipaddr			{ yylval.stptr = SymLook(yytext); 
                          return IPADDR; 
                        }
networkmask		{ yylval.stptr = SymLook(yytext); 
                          return NETWORKMASK; 
                        }
macaddr			{ yylval.stptr = SymLook(yytext); 
                          return MACADDR; 
                        }
range			{ yylval.stptr = SymLook(yytext); 
                          return RANGE; 
                        }

optional		{ yylval.stptr = SymLook(yytext); 
                          return OPTIONAL; 
                        }
mandatory		{ yylval.stptr = SymLook(yytext); 
                          return MANDATORY; 
                        }

{identifier}		{ 
			  yylval.string = yytext;
                          return IDENTIFIER; 
			}

{nl}			{ line_count++; }

[={};]			{ return yytext[0]; }

.			{ return yytext[0]; }

%%

