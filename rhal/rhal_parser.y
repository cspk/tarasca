/*
 * rhal_parser.y --
 *
 * Author -- Pedro Aguilar <paguilar@junkerhq.net>
 *
 * Copyright (c) 2004 - 2006 Pedro Aguilar
 *
 * Description: This is a Bison file in which the syntax analysis (parse) is done.
 *     It parses the command definition files used by the CLI.
 *     Once a token is ready, it calls a func of the graph in order
 *     to put the token inside a specific node.
 *     The parser is called using the func rhal_parse()
 *
 * Notes: CAPITALS are used to represent terminal symbols.
 *     Non-capitals are used to represent non-terminals.
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
 */

%{

#include <stdio.h>
#include <string.h>

#include "rhal.h"
#include "rhal_symbol_table.h"
#include "graph.h"
#include "misc.h"

extern CLIMatrix	matrix;

%}

%union 
{
    char	*string;
    SymTable	stptr;
}

%token <string> IDENTIFIER
%token <string> QSTRING

%type <string> cstring
%type <string> name
%type <stptr> value_expr
%type <stptr> prio_expr

%token <stptr> MODE PROMPT COMMAND FUNC AVAIL ARG NAME PRIORITY VALUE PARENT DESC
%token <stptr> BOOL NUMBER STRING HEX IPADDR NETWORKMASK MACADDR RANGE OPTIONAL MANDATORY

%%

mode_sections: mode_section
	| mode_sections mode_section
;

mode_section: MODE name { matrix = graph_mode_insert($2); }
			'{' mode_attrs '}'
;

mode_attrs: prompt_section
	| cmd_sections
	| prompt_section cmd_sections
;

prompt_section: PROMPT '=' cstring { matrix = graph_mode_insert_prompt($3); } ';'
;


cmd_sections: cmd_section
	| cmd_sections cmd_section
;

cmd_section: COMMAND name { matrix = graph_cmd_insert($2); } 
			  '{' cmd_attrs '}'
;

/* FIXME: Ugly printf. If there is a cmd without desc, the parser crashes */
cmd_attrs: func_section avail_sections { printf(""); } desc_section arg_sections
;

func_section: FUNC '=' name { matrix = graph_cmd_insert_func($3); } ';'
;

avail_sections: avail_section
	| avail_sections avail_section
;

avail_section: /* empty */
	| AVAIL '=' name { matrix = graph_cmd_insert_avail($3); } ';'
;

arg_sections: arg_section
	| arg_sections arg_section
;

arg_section: /* empty */
	| ARG name { matrix = graph_arg_insert($2); }
		   '{' arg_attrs '}'
;

arg_attrs: name_section value_section prio_section parent_section desc_section
;

name_section: /* empty */
        | NAME '=' cstring { matrix = graph_arg_insert_name($3); } ';'
;

value_section:  VALUE '=' value_expr { 
                                 if ($3 == (SymTable)NULL) {
                                     TRS_ERR("bison: Error while parsing value_section");
                                     fprintf(FD_OUT, "bison: Error while parsing value_section\n");
                                 }
                                 else
                                     matrix = graph_arg_insert_value($3->type);
                               } ';'
;

value_expr: BOOL
	| STRING 
	| NUMBER	
	| HEX 
	| IPADDR 
	| NETWORKMASK 
	| MACADDR 
	| RANGE		{ $$ = $1; }
;

prio_section:  PRIORITY '=' prio_expr { 
				if ($3 == (SymTable)NULL) {
                                     TRS_ERR("bison: Error while parsing prio_section");
                                     fprintf(FD_OUT, "bison: Error while parsing prio_section\n");
                                 }
				else
				    matrix = graph_arg_insert_prio($3->type); 
			     } ';'
;

prio_expr: OPTIONAL
	| MANDATORY	{ $$ = $1; }
;


parent_section: /* empty */
	| PARENT '=' name { matrix = graph_arg_insert_parent($3); } ';'
;

name: IDENTIFIER { $$ = $1; }
;

cstring: QSTRING { $$ = $1; }
;

desc_section: /* empty */
	| DESC '=' cstring { matrix = graph_insert_desc($3); } ';'
;
%%


void enable_yacc_debug() {
    #if YYDEBUG != 0
        yydebug = 1;
    #endif
}
