/*
 * graph.h --
 *
 * Author -- Pedro Aguilar <paguilar@junkerhe.net>
 *
 * Copyright (c) 2004 - 2006 Pedro Aguilar
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

#ifndef _GRAPH_H
#define _GRAPH_H

/*****************************
 * INCLUDES
 *****************************/

#include "slist.h"

/*****************************
 * MACROS
 *****************************/

#define MANDATORY_ARG    "mandatory"
#define OPTIONAL_ARG     "optional"
#define TEXT_ARG         "text"

/*****************************
 * DATA TYPES
 *****************************/

typedef struct CLIMatrix_st *	 CLIMatrix;
typedef struct CLIModeNode_st *	 CLIModeNode;
typedef struct CLICmdNode_st *   CLICmdNode;
typedef struct CLIArgNode_st *   CLIArgNode;
typedef struct CLIAvailNode_st * CLIAvailNode;


struct CLIMatrix_st {
    SList		graph;
    SList		mode;
    SList		cmd;
    SList		arg;
};


struct CLIModeNode_st {
    short int		id;
    char        	*name;
    char        	*prompt;
    short int           cmd_count;
    short int           parent;
    SList		avail;
    SList       	cmd_list;
};


struct CLICmdNode_st {
    short int   	id;
    char        	*name;
    char        	*func_name;
    char		*desc;
    SList       	arg_list;
};


struct CLIArgNode_st {
    short int           id;
    char                *sid;
    char        	*name;
    char        	*value;
    char   		*prio;
    char        	*desc;
    short int           parent;
};


struct CLIAvailNode_st {
    short int		mode_id;
    short int		cmd_id;
};

/*****************************
 * FUNCTION DECLARATION
 *****************************/

CLIMatrix	graph_matrix_init(void);
void		graph_reset_ptrs(void);

int		graph_mode_id_exist(short int);
CLIMatrix       graph_mode_insert(char *);
CLIMatrix	graph_mode_insert_prompt(char *);
int	        graph_mode_insert_avail(CLIModeNode, int, int);
int		graph_mode_count_avail(CLIAvailNode);
int             graph_mode_cmd_count(SList);
SList           graph_mode_cmd_id_exist(short int, short int);
int             trs_graph_mode_change(char *);

int		graph_cmd_exist(SList, char *);
CLIMatrix	graph_cmd_insert(char *);
CLIMatrix	graph_cmd_insert_func(char *);
CLIMatrix	graph_cmd_insert_avail(char *);

int		graph_arg_exist(SList, char *);
CLIMatrix	graph_arg_insert(char *);
CLIMatrix	graph_arg_insert_name(char *);
CLIMatrix	graph_arg_insert_value(char *);
CLIMatrix	graph_arg_insert_prio(char *);
CLIMatrix	graph_arg_insert_parent(char *);

CLIMatrix	graph_insert_desc(char *);

#endif	/* _GRAPH_H */
