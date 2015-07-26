/*
 * rhal.c --
 *
 * Author -- Pedro Aguilar <paguilar@junkerhq.net>
 *
 * Copyright (c) 2004 - 2006 Pedro Aguilar
 *
 * Description: Initializes the main structs, symbols tables and parsing 
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

/*****************************
 * INCLUDES
 *****************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <pthread.h>
#include <errno.h>

#include "config.h"
#include "rhal.h"
#include "rhal_symbol_table.h"
//#include "rhal_parser.h"
#include "y.tab.h"
#include "graph.h"
#include "print.h"
#include "misc.h"
#include "slist.h"

/*****************************
 * MACROS
 *****************************/

#ifdef __GNUC__
# ifdef YYPARSE_PARAM
extern int yyparse (void *);
# else
extern int yyparse (void);
# endif
#endif

extern void enable_yacc_debug();

/*****************************
 * DATA TYPES
 *****************************/

void (**fn_tbl)(SList);
char **cmd_tbl;

extern void enable_yacc_debug();

int		 parserr;
extern FILE 	 *yyin;
extern FILE 	 *rhal_out;
extern CLIMatrix matrix;

/*****************************
 * FUNCTION IMPLEMENTATION
 *****************************/

void yyerror(const char *err) {
    TRS_ERR((char *)err);
}

int yywrap(void) {
    return 1;
}


/*
 * Description: Execute the given cmd with its args (if any)
 */
int rhal_exec_cmd(CLICmdNode cmd, SList arg_input_list) {
    int          i = 0;

    /* Search the cmd in the cmds table */
    while (cmd_tbl[i] != (char *)NULL) {
        if (!strncmp(cmd->name, cmd_tbl[i], strlen(cmd->name)))
            break;
        i++;
    }

    if (cmd_tbl[i] == (char *)NULL)
        return 0;

    /* Exec the function */
    fn_tbl[i](arg_input_list);

    return 1;
}

/**************************************************
 * Check the args of a cmd given in the input line
 *************************************************/

/*
 * Description: Check if the given num is inside the limits 'left'
 *     and 'right', inclusive
 */
short int check_limits(int num, int left, int right) {
    if (num < left || num > right)
        return 0;
    return 1;
}


/*
 * Description: 
 */
short int check_arg_parent(CLICmdNode cmd_node, CLIArgNode arg_node, SList input_list) {
    char          *parent_sid = NULL;
    SList         plist,
                  arg_list;
    InputToken    token_node;
    CLIArgNode    tmp_arg;

    /* Arg doesn't have parent */
    if (arg_node->parent == -1)
        return 1;

    arg_list = (SList)cmd_node->arg_list;
    while (arg_list) {
        tmp_arg = (CLIArgNode)arg_list->data;
        if (tmp_arg->id == arg_node->parent) {
            parent_sid = tmp_arg->sid;
            break;
        }
        arg_list = arg_list->next;
    }

    if (!parent_sid)
        return 0;
    debug_shell_printf("Search for arg parent %s in input_list\n", parent_sid);

    plist = input_list->next;
    while (plist) {
        token_node = (InputToken)plist->data;
        if (!token_node->sid)
            return 0;
        if (!strcmp(token_node->sid, parent_sid))
            return 1;

        plist = plist->next;
    }

    return 0;
}


/*
 * Description: 
 */
short int check_arg_type(CLIArgNode arg_node, InputToken token_node) {
    char          *graph_type;

    graph_type = arg_node->value;
    debug_shell_printf("Arg type: %s, input: %s\n", graph_type, token_node->value);

    if (!strcmp(graph_type, "bool"))
        return is_bool(token_node->value);
    else if (!strcmp(graph_type, "number"))
        return is_number(token_node->value);
    else if (!strcmp(graph_type, "string"))
        return is_string(token_node->value);
    else if (!strcmp(graph_type, "hex"))
        return is_hex(token_node->value);
    else if (!strcmp(graph_type, "ipaddr"))
        return is_ipaddr(token_node->value);
    else if (!strcmp(graph_type, "networkmask"))
        return is_netmask(token_node->value);
    else if (!strcmp(graph_type, "macaddr"))
        return is_macaddr(token_node->value);
    else if (!strcmp(graph_type, "range"))
        return is_range(token_node->value);
    else
        return 0;
}


/*
 * Description: If priorities are equal, return 1; else return 0
 */
short int check_arg_prio(CLIArgNode arg_node, char *priority) {
    return !strcmp(arg_node->prio, priority) ? 1 : 0;
}


/*
 * Description: If names are equal, return 1; else return 0
 */
short int check_arg_name(CLIArgNode arg_node, InputToken token_node) {
    debug_shell_printf("Checking arg name. Graph: %s. Input: %s\n", 
        arg_node->name, token_node->value);
    if (!strcmp(arg_node->name, token_node->value))
        return 1;
    else
        return 0;
}


/*
 * Description: Validate the given args in the input_list
 */
static short int match_args(CLICmdNode cmd, SList input_list) {
    CLIArgNode    arg_node;
    InputToken    token_node;
    SList         arg_list,
                  plist;
    int           cnt = 0;

    cnt = slist_count(input_list) - 1;

    /* Input list empty; */
    if (cnt < 0)
        return 0;

    /* The cmd doesn't have args */
    if (!cmd->arg_list && cnt == 0)
        return 1;

    /* The cmd doesn't have args but we receive dummy ones */
    if (!cmd->arg_list && cnt > 0) {
        fprintf(FD_OUT, "Too many parameters!\n");
        return 0;
    }

    plist = input_list->next;
    arg_list = (SList)cmd->arg_list;

    /* We do not receive args. If the cmd has only optional args 
     * everything is OK, but if it has at least one mandatory arg 
     * before any optional arg there's an error */
    if (!plist) {
         arg_node = (CLIArgNode)arg_list->data;
         return !strcmp(arg_node->prio, MANDATORY_ARG) ? 0 : 1;
    }

    /* Default case in which we receive one or more args in 
     * the input list and the cmd has one or more mandatory 
     * and/or optional args */
    while (plist && arg_list) {
        token_node = (InputToken)plist->data;
        if (!strcmp(token_node->value, "?"))
            return 0;

        arg_node = (CLIArgNode)arg_list->data;
        if (check_arg_prio(arg_node, MANDATORY_ARG)) { /* Mandatory arg */
            debug_shell_printf("Mandatory arg: %s\n", token_node->value);
            if (!arg_node->name) {                     /* Arg has form: 'value' */
                if (!check_arg_type(arg_node, token_node)) {
                        fprintf(FD_OUT, "Wrong parameter type: %s\n", 
                            token_node->value);
                        return 0;
                }
                token_node->sid = arg_node->sid;
                cnt--;
            }
            else {                                     /* Arg has form: 'name value' */
                token_node = (InputToken)plist->data;
                if (strcmp(arg_node->name, token_node->value))
                    return 0;
                /* Check if we have parent arg */
                if (!check_arg_parent(cmd, arg_node, input_list))
                    return 0;
                token_node->sid = arg_node->sid;
                plist = plist->next;
                if (!plist)
                    return 0;
                token_node = (InputToken)plist->data;
                if (!check_arg_type(arg_node, token_node)) {
                        fprintf(FD_OUT, "Wrong parameter type: %s\n", 
                            token_node->value);
                        return 0;
                }
                token_node->sid = arg_node->sid;
                cnt = cnt - 2;
            }
        }
        else {                                          /* Optional arg */
            debug_shell_printf("Optional arg: %s\n", token_node->value);
            /* Search input arg in the graph's arg list */
            while (arg_list) {                          /* Always have the form: 'name value' */
                arg_node = (CLIArgNode)arg_list->data;
                if (!arg_node->name)
                    return 0;
                if (!strcmp(token_node->value, arg_node->name)) {
                    /* Check if we have parent arg */
                    if (!check_arg_parent(cmd, arg_node, input_list))
                        return 0;
                    token_node = (InputToken)plist->data;
                    token_node->sid = arg_node->sid;
                    plist = plist->next;
                    if (!plist)
                        return 0;
                    token_node = (InputToken)plist->data;
                    if (!check_arg_type(arg_node, token_node)) {
                        fprintf(FD_OUT, "Wrong parameter type: %s\n", 
                            token_node->value);
                        return 0;
                    }
                    token_node->sid = arg_node->sid;
                    cnt = cnt - 2;
                    break;
                }
                arg_list = arg_list->next;
            }
            if (!arg_list && cnt) {
                return 0;
            }
        }
        arg_list = arg_list->next;
        plist = plist->next;
    }

    /* Check if we have all the mandatory args.
     * The mandatory args that depend of an optional arg are not considered */
    while (arg_list) {
        arg_node = (CLIArgNode)arg_list->data;
        if (check_arg_prio(arg_node, MANDATORY_ARG)) {
            if (arg_node->parent == -1)
                return 0;
            else {
                if (check_arg_parent(cmd, arg_node, input_list))
                    return 0;
            }
        }
        arg_list = arg_list->next;
    }

    /* If we matched all args successfully, the counter 'cnt' should be zero */
    if (!cnt) {
        debug_shell_printf("--- Command lex, syntax and semantics correct!\n");
        return 1;
    }
    else {
        fprintf(FD_OUT, "Missed parameter\n");
        return 0;
    }
}


/*
 * Description: Validate the syntax of the cmd and its args
 */
short int rhal_validate_input_list(SList input_list) {
    CLICmdNode     cmd = NULL;
    CLIModeNode    mode = NULL;
    InputToken     token_node = NULL;
    SList          cmd_list,
                   avail_list,
                   plist;
    CLIAvailNode   avail_node;

    plist = input_list;
    cmd_list = (SList)matrix->cmd;
    /* Search if the given cmd token exists in the current mode */
    while (cmd_list) {
        cmd = (CLICmdNode)cmd_list->data;
        token_node = (InputToken)plist->data;
        if (!strcmp(cmd->name, token_node->value)) {
            debug_shell_printf("--- Command found in graph\n");
            /* If cmd was found, check its args */
            if (!match_args(cmd, input_list)) {
                trs_print_cmd_syntax(cmd, 0);
                return CMD_INVALID;
            }
            /* Point global ptr 'cmd' to the cmd node found */
            matrix->cmd = cmd_list;
            return CMD_OK;
        }
        cmd_list = cmd_list->next;
    }
    
    /* Search the cmd in the avail list of the mode because it could be
     * declared in another mode and made available to this one */
    cmd = (CLICmdNode)NULL;
    mode = (CLIModeNode)matrix->mode->data;
    avail_list = mode->avail;
    while (avail_list) {
        avail_node = (CLIAvailNode)avail_list->data;
        if ((cmd_list = graph_mode_cmd_id_exist(avail_node->mode_id, avail_node->cmd_id)) != NULL) {         cmd = (CLICmdNode)cmd_list->data;
            if (!strcmp(cmd->name, token_node->value)) {
                debug_shell_printf("--- Command found in graph\n");
                /* If cmd was found, check its args */
                if (!match_args(cmd, input_list)) {
                    trs_print_cmd_syntax(cmd, 0);
                    return CMD_INVALID;
                }
                /* Point global ptr 'cmd' to the cmd node found */
                matrix->cmd = cmd_list;
                return CMD_OK;
            }
        }
        avail_list = avail_list->next;
    }

    debug_shell_printf("--- Command not found in graph!\n");
    return CMD_NOT_FOUND;
}


/*
 * Description: This func is called when an error occurs in the 
 *    lexer and parser
 */
void rhal_error(const char *errstr) {
    TRS_ERR("Configuration file, line %d: %s\n", line_count, errstr);
    fprintf(stderr, "Configuration file, line %d: %s\n", line_count, errstr);
}


/*
 * Description: Initialize the symbols table and graph
 */
int rhal_parse_init() {
    /* Init symbol table */
    if (!SymInit())
        return 0;

    parserr = 0;

    /* Init graph */
    matrix = graph_matrix_init();

    /* If YYDEBUG=1 prints out debug info from yacc */
    enable_yacc_debug();

    if (parserr)
        return 0;

    return 1;
}


/*
 * Description: 
 *     If retval > 0, a stream was successfully read
 *     else if retval == 0, there was an error
 *     else if retval == -1, EOF was reach
 *     else if retval == -2, file name size exceeded allowed size
 */
int rhal_parse_files() {
    FILE	*rhalfd = NULL,
		*fd = NULL;
    char	filename[NAME_SIZE];
    char	stream[NAME_SIZE];
    int		retval = 0;

    reset_buff(filename, NAME_SIZE);
    reset_buff(stream, NAME_SIZE);

    sprintf(filename, "%s/%s", TARASCA_CMD_PATH, TARASCA_CMD_FILE);
    /*sprintf(filename, "%s/%s/%s", TARASCA_PATH, TARASCA_CMD_PATH, TARASCA_CMD_FILE);*/
    if (!(rhalfd = fopen(filename, "r"))) {
        TRS_ERR("%s. Could not open CLI config file %s", 
            strerror(errno), filename);
        return 0;
    }

    while ((retval = get_line_cmd_file(rhalfd, stream)) >= -2) {
        /* Error: File name size exceeded allowed size */
	if (retval == -2) {
	    TRS_ERR("File name size exceeded allowed size of %d bytes", 
                NAME_SIZE);
	    fclose(rhalfd); return 0;
	}
        /* Error: There was a parse error */
	else if (retval == 0) {
	    TRS_ERR("Parse error when reading config file %s", 
                TARASCA_CMD_FILE);
	    fclose(rhalfd); return 0;
	}
        /* EOF was reached and no filename in stream */
	else if (retval == -1 && stream[0] == '\0') {
	    fclose(rhalfd); return 1;
	}
        /* A filename in stream was received */
	else if (stream[0] != '\0') {
	    sprintf(filename, "%s/%s", TARASCA_CMD_PATH, stream);
	    /*sprintf(filename, "%s/%s/%s", TARASCA_PATH, TARASCA_CMD_PATH, stream);*/
	    if (!(fd = fopen(filename, "r"))) {
                TRS_ERR("Could not open COMMAND file %s", 
                    filename);
		fclose(rhalfd); return 0;
	    }
	    else {
                /* Parse file using the lex/yacc funcs */
	        debug_rhal_printf("*** Parsing file %s ***\n", filename);

                yyin = fd;
                yyparse();
	        fclose(fd);
	    }
            /* A filename in stream was received with an EOF */
	    if (retval == -1) {
		fclose(rhalfd); return 1;
	    }
	}
	else {
	    TRS_ERR("Unknown error while parsing config file %s", 
                TARASCA_CMD_FILE);
	    fclose(rhalfd); return 0;
	}
	reset_buff(stream, NAME_SIZE);
    }

    fclose(rhalfd);
    return 1;
}


/*
 * Description: Initialize the command and function tables
 */
int rhal_init_tables() {
    CLIModeNode    mode_node;
    SList          graph_list,
                   cmd_list;
    int            cmd_cnt = 0;

    graph_list = matrix->graph;
    while (graph_list) {
        mode_node = (CLIModeNode)graph_list->data;
        cmd_list = (SList)mode_node->cmd_list;
        while (cmd_list) {
            cmd_cnt++;
            cmd_list = cmd_list->next;
        }
        graph_list = graph_list->next;
    }

    /* Both arrays end with a NULL element */
    cmd_cnt++;
    debug_shell_printf("Number of cmds implemented: %d\n", cmd_cnt);
    if ((fn_tbl = (void (**)(SList))xmalloc(sizeof(void (*)(SList)), cmd_cnt)) == NULL)
        return 0;

    if ((cmd_tbl = (char **)xmalloc(sizeof(char *), cmd_cnt)) == NULL)
        return 0;

    return 1;
}

