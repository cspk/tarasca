/*
 * rhal.h --
 *
 * Author -- Pedro Aguilar <paguilar@junkerhq.net>
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

#ifndef _RHAL_H
#define _RHAL_H

/*****************************
 * INCLUDES
 *****************************/

#include "graph.h"
#include "rhal.h"
#include "misc.h"

/*****************************
 * MACROS
 *****************************/

#define PROJECT_NAME "Tarasca project"
#define VERSION      "0.3.2"

/*****************************
 * DATA TYPES
 *****************************/

int line_count;

typedef struct input_token_st *    InputToken;
typedef struct sort_cmds_st *      SortCmds;

enum input_validation {
    CMD_OK = 0,
    CMD_INVALID,
    CMD_NOT_FOUND,
    CMD_UNKNOWN = 100
};

struct sort_cmds_st {
    char      *name;
    char      *desc;
};

struct input_token_st {
    char      *sid;
    char      *value;
};

/*****************************
 * FUNCTION DECLARATION
 *****************************/

void yyerror(const char *);
int yywrap(void);

int rhal_parse_init(void);
int rhal_parse_files(void);
int rhal_init_tables(void);

int rhal_exec_cmd(CLICmdNode, SList);
short int rhal_validate_input_list(SList);
void rhal_error(const char *);

#endif /* _RHAL_H */
