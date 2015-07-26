/* vi: set sw=4 ts=4: */
/*
 * print.c --
 *
 * Author -- Pedro Aguilar <paguilar@junkerhq.net>
 *
 * Copyright (c) 2006-2007 Pedro Aguilar
 *
 * Description: Printing function for the CLI/GUI
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
#include <stdarg.h>

#include "print.h"
#include "rhal.h"
#include "graph.h"
#include "misc.h"

/*****************************
 * DATA TYPES
 *****************************/

extern CLIMatrix matrix;
PrintTable       ptable;

/*****************************
 * FUNCTION IMPLEMENTATION
 *****************************/

static void print_table_init() {
    int           i;

    ptable.cols = 0;
    ptable.pager = PRINT_PAGER;

    for (i = 0; i < PRINT_MAX_COLS; i++) {
        ptable.type[i] = 'n';
        ptable.size[i] = 0;
    }
}

static void print_table_clean() {
    int i;

    ptable.cols = 0;
    
    for (i = 0; i < PRINT_MAX_COLS; i++) {
        ptable.type[i] = 's';     /* All columns by default are strings */
        ptable.size[i] = 0;
    }
}

/**
 * Configure the output table before printing.
 * NOTE: When calling va_arg(), if there is no next argument, 
 * or if type is not compatible with the type of the actual next argument, 
 * random errors will occur.
 *
 * @param cols: the number of columns
 * @param variable: the type and length of each column
 *
 * @return void
 *
 * @ingroup Printing functions
 */
void trs_print_table_config(short int cols, ...) {
    va_list    ap;
    int        i = 0;
    char       typeN;
    short int  sizeN;

    print_table_init();

    if (cols < 1 || cols > PRINT_MAX_COLS) {
        fprintf(FD_OUT, "Error: Invalid number of columns %d\n", cols);
        return;
    }

    ptable.cols = cols;
    va_start(ap, cols);
    for (i = 0; i < ptable.cols; i++) {
        typeN = (char)va_arg(ap, int);
        if (typeN != 's' &&    /* string */
            typeN != 'd' &&    /* int */
            typeN != 'f' &&    /* double (float) */
            typeN != 'F' &&    /* long double (long float) */
            typeN != 'x' &&    /* hex */
            typeN != 'u'       /* unsigned int */
           ) {
            fprintf(FD_OUT, "Error: Invalid type '%c' for column %d\n", typeN, i);
            print_table_clean();
            return;
        }
        ptable.type[i] = typeN;

        sizeN = va_arg(ap, int);
        if (sizeN < 1) {
            fprintf(FD_OUT, "Error: Invalid size %d for column %d\n", sizeN, i);
            print_table_clean(ptable);
            return;
        }
        ptable.size[i] = sizeN;
    }
    va_end(ap);
}

/**
 * Configure the output table before printing.
 * NOTE: When calling va_arg(), if there is no next argument, 
 * or if type is not compatible with the type of the actual next argument, 
 * random errors will occur.
 *
 * @param table: the table in which the even cols are the data type
 *        and the odd cols are the value
 *
 * @return void
 *
 * @ingroup Printing functions
 */
void trs_print_row(PrintTable table, ...) {
    int        i;
    char       row[LOG_BUFF];
    va_list    ap;

    va_start(ap, table);
    for (i = 0; i < ptable.cols; i++) {
        if (ptable.type[i] != 's' &&    /* string */
            ptable.type[i] != 'd' &&    /* int */
            ptable.type[i] != 'f' &&    /* double (float) */
            ptable.type[i] != 'F' &&    /* long double (long float) */
            ptable.type[i] != 'x' &&    /* hex */
            ptable.type[i] != 'u'       /* unsigned int */
           ) {
            fprintf(FD_OUT, "Error: Invalid type '%c' for column %d\n", ptable.type[i], i);
            print_table_clean();
            return;
        }

        if (ptable.cols < 1 || ptable.cols > PRINT_MAX_COLS) {
            fprintf(FD_OUT, "Error: Invalid number of columns %d\n", ptable.cols);
            return;
        }

        if (ptable.type[i] == 's') {            /* string */
            int j;
            char *s = NULL;
            reset_buff(row, LOG_BUFF);
            s = va_arg(ap, char *);

            if (strlen(s) > LOG_BUFF - 1) {
                strncpy(row, s, LOG_BUFF);
            }
            else {
                sprintf(row, "%s ", s);
                for (j = strlen(row); j < LOG_BUFF; j++) {
                    if (j < ptable.size[i])
                        sprintf(row, "%s ", row);
                    if (row[j] == '\0' && j >= ptable.size[i])
                        break;
                }
            }
            fprintf(FD_OUT, "%s", row);
        }
        else if (ptable.type[i] == 'd') {       /* int */
            int num;
            reset_buff(row, LOG_BUFF);
            num = va_arg(ap, int);
            sprintf(row, "%d", num);
        }
        else if (ptable.type[i] == 'f') {       /* double (float) */
            double num;
            reset_buff(row, LOG_BUFF);
            num = va_arg(ap, double);
            sprintf(row, "%f", num);
        }
        else if (ptable.type[i] == 'F') {       /* long double (long float) */
            long double num;
            reset_buff(row, LOG_BUFF);
            num = va_arg(ap, long double);
            sprintf(row, "%Lg", num);
        }
        else if (ptable.type[i] == 'x') {       /* hex */
            int num;
            reset_buff(row, LOG_BUFF);
            num = va_arg(ap, int);
            sprintf(row, "%#x", num);
        }
        else if (ptable.type[i] == 'u') {       /* unsigned int */
            unsigned int num;
            reset_buff(row, LOG_BUFF);
            num = va_arg(ap, unsigned int);
            sprintf(row, "%u", num);
        }
        else {
            fprintf(FD_OUT, "Error: Invalid type %c for column %d\n", ptable.type[i], i);
            print_table_clean();
            return;
        }

        if (ptable.type[i] != 's') {
            int j;
            if (strlen(row) > LOG_BUFF - 1)
                row[LOG_BUFF - 1] = '\0';
            for (j = strlen(row); j < LOG_BUFF; j++) {
                if (j < ptable.size[i])
                    sprintf(row, "%s ", row);
                if (row[j] == '\0' && j >= ptable.size[i]) {
                    row[j] = ' ';
                    break;
                }
            }
            fprintf(FD_OUT, "%s", row);
        }
    }
    va_end(ap);
    fprintf(FD_OUT, "\n");
}


/*
 * Description: Print a string
 */
void trs_print_str(char *str) {
    fprintf(FD_OUT, "%s", str);
}

/*
 * Description: Print a single text line adding a '\n' at the end
 */
void trs_print_line(char *line) {
    fprintf(FD_OUT, "%s\n", line);
}

/*
 * Description: Print one or more '\n'
 */
void trs_print_nl(int num) {
    int i;

    if (num < 0)
        return;
    else
        for (i = 0; i < num; i++)
            fprintf(FD_OUT, "\n");
}

/**************************************************
 * Printing funcs
 *************************************************/

/*
 * Get recursively the syntax of the optional args
 */
static char * get_optional_args(SList arg_list, char *syntax, int prev_node_id, int first_time, int prev_parent_id, int bracket_cnt) {
    CLIArgNode arg_node = NULL;
    int i;

    if (!arg_list) {
        if (!bracket_cnt)
            sprintf(syntax, "%s]", syntax);
        else {
            for (i = 0; i < bracket_cnt; i++)
                sprintf(syntax, "%s]", syntax);
        }
        return syntax;
    }

    arg_node = (CLIArgNode)arg_list->data;
    /* Optional param that does not depend of another optional param */
    if (arg_node->parent == -1) {
        if (arg_node->parent == prev_node_id) {
            if (first_time)
                sprintf(syntax, "%s [%s", syntax, arg_node->name);
            else
                sprintf(syntax, "%s] [%s", syntax, arg_node->name);
            syntax = get_optional_args(arg_list->next, syntax, arg_node->id, 0, arg_node->parent, bracket_cnt);
            bracket_cnt++;
        }
        else {
            if (arg_node->parent != prev_parent_id) {
                sprintf(syntax, "%s]", syntax);
            }
            sprintf(syntax, "%s] [%s", syntax, arg_node->name);
            syntax = get_optional_args(arg_list->next, syntax, arg_node->id, 0, arg_node->parent, bracket_cnt);
        }
    }
    /* Optional param that depends of another optional param */
    else {
        if (arg_node->parent == prev_node_id) {
            sprintf(syntax, "%s [%s", syntax, arg_node->name);
            bracket_cnt++;
        }
        else {
            if (arg_node->parent != prev_parent_id) {
                for (i = 0; i < bracket_cnt; i++)
                    sprintf(syntax, "%s]", syntax);
                bracket_cnt = 0;
            }
            else
                bracket_cnt++;
            sprintf(syntax, "%s] [%s", syntax, arg_node->name);
        }
        syntax = get_optional_args(arg_list->next, syntax, arg_node->id, 0, arg_node->parent, bracket_cnt);
    }
 
    return syntax;
}

void trs_print_init_msg(char *msg) {
#ifdef ENABLE_SHELL_MSG
    if (!msg)
        return;

    time_t        timeval;

    timeval = time(0);
    printf("\n%s v%s, %s. %s\n", msg, VERSION, PROJECT_NAME, ctime(&timeval));
#endif /*ENABLE_SHELL_MSG */
}

void trs_print_end_msg(char *msg) {
#ifdef ENABLE_END_MSG
    if (!msg)
        return;

    printf("%s\n", msg);
#endif /*ENABLE_END_MSG */
}

/**
 * Print the correct command's syntax.
 * This function is called when the cmd has a syntax error
 * and from the 'help' command
 *
 * @input curr_cmd: The command that has a syntax error
 * @input help_cmd: 1 if we come from the help command, 0 otherwise
 *
 * @return void
 *
 * @ingroup Printing functions
 */
void trs_print_cmd_syntax(CLICmdNode curr_cmd, int help_cmd) {
    short int    cmd_found = 0;
    char         syntax_buff[BUFSIZ / 8],
                 arg_desc[BUFSIZ / 8];
    SList        arg_list = NULL,
                 avail_list = NULL,
                 cmd_list = NULL,
                 list = NULL;
    CLIArgNode   arg_node = NULL;
    CLIModeNode  mode = NULL;
    CLICmdNode   cmd_node = NULL;
    CLIAvailNode avail_node = NULL;

    if (!curr_cmd) {
        trs_print_line("Syntax: help <cmd>");
        return;
    }

    if (help_cmd < 0 || help_cmd > 1) {
        trs_print_line("Syntax: help <cmd>");
        return;
    }

    /* If the given cmd's syntax was wrong, search the cmd and 
     * display its correct syntax. If we come from the 'help' cmd, 
     * we already have the cmd */
    if (!help_cmd) {
        /* If the cmd does not exist in the current mode,
         * search it in the mode's availability list */
        if (!graph_cmd_exist(matrix->mode, curr_cmd->name)) {
            mode = (CLIModeNode)matrix->mode->data;
            avail_list = mode->avail;
            while (avail_list) {
                avail_node = (CLIAvailNode)avail_list->data;
                if ((cmd_list = graph_mode_cmd_id_exist(avail_node->mode_id, avail_node->cmd_id)) != NULL) {
                    cmd_node = (CLICmdNode)cmd_list->data;
                    if (!strcmp(curr_cmd->name, cmd_node->name)) {
                        cmd_found = 1;
                        break;
                    }
                }
                avail_list = avail_list->next;
            }
            if (cmd_found)
                curr_cmd = cmd_node;
            else {
                trs_print_line("Type <TAB> or 'show' for a list of available commands");
                return;
            }
        }
    }

    reset_buff(syntax_buff, BUFSIZ / 8);
    reset_buff(arg_desc, BUFSIZ / 8);

    if (curr_cmd->desc) {
        sprintf(syntax_buff, "Description: %s", curr_cmd->desc);
        trs_print_line(syntax_buff);
    }
    else {
        sprintf(syntax_buff, "Description: No description available");
        trs_print_line(syntax_buff);
    }
    sprintf(syntax_buff, "Syntax: %s", curr_cmd->name);

    arg_list = (SList)curr_cmd->arg_list;
    list = arg_list;
    while (list) {
        arg_node = (CLIArgNode)list->data;
        /* If we have a mandatory arg print it with '< >',
         * else its optional, call get_optional_args() that 
         * will print them with '[ ]' */
        if (!strcmp(arg_node->prio, MANDATORY_ARG)) {
            if (arg_node->name != NULL)
                sprintf(syntax_buff, "%s <%s>", syntax_buff, arg_node->name);
            else
                sprintf(syntax_buff, "%s <%s>", syntax_buff, arg_node->sid);
        }
        else {
            sprintf(syntax_buff, "%s", get_optional_args(list, syntax_buff, -1, 1, -1, 0));
            break;
        }
        list = list->next;
    }

    trs_print_line(syntax_buff);
    trs_print_nl(1);

    trs_print_table_config(2, 's', 22, 's', 50);
    list = arg_list;
    while (list) {
        arg_node = (CLIArgNode)list->data;
        if (!strcmp(arg_node->prio, MANDATORY_ARG))
            if (!arg_node->name)
                sprintf(syntax_buff, " <%s>", arg_node->sid);
            else
                sprintf(syntax_buff, " <%s>", arg_node->name);
        else
            sprintf(syntax_buff, " [%s]", arg_node->name);

        if (arg_node->desc)
            sprintf(arg_desc, "%s", arg_node->desc);
        else
            sprintf(arg_desc, "No description available");

        trs_print_row(ptable, syntax_buff, arg_desc);
        list = list->next;
    }
}

/**
 * Display in ascending order the list of available cmds 
 * and their description
 *
 * @input mode: The mode that contains the commands
 *
 * @return TRS_OK
 *
 * @ingroup Printing functions
 */
void trs_print_cmd_list(SList mode) {
    int          head;
    SList        base = NULL,
                 curr  = NULL,
                 aux  = NULL,
                 tail1  = NULL,
                 tail2  = NULL,
                 smallest = NULL,
                 list_head = NULL,
                 cmd_list = NULL,
                 avail_list = NULL;
    SortCmds     sort_cmds = NULL,
                 node1 = NULL,
                 node2 = NULL;
    CLICmdNode   cmd_node = NULL;
    CLIModeNode  mode_node = NULL;
    CLIAvailNode avail_node = NULL;

    list_head = slist_new();

    /* Search the cmds in the current mode list */
    mode_node = (CLIModeNode)mode->data;
    cmd_list = (SList)mode_node->cmd_list;
    while (cmd_list) {
        if ((sort_cmds = (SortCmds)xmalloc(sizeof(struct sort_cmds_st), 1)) == NULL) {
            trs_print_line("Could not generate command syntax help");
            return;
        }
        cmd_node = (CLICmdNode)cmd_list->data;
        sort_cmds->name = cmd_node->name;
        if (!cmd_node->desc)
            sort_cmds->desc = "No description available";
        else
            sort_cmds->desc = cmd_node->desc;
        list_head = slist_append(list_head, sort_cmds);
        cmd_list = cmd_list->next;
    }

    /* Search cmds in the avail list of the current mode because
     * they could be declared in another mode and made available to this one */
    avail_list = mode_node->avail; 
    while (avail_list) {
        avail_node = (CLIAvailNode)avail_list->data;
        if ((cmd_list = graph_mode_cmd_id_exist(avail_node->mode_id, avail_node->cmd_id)) != NULL) {
            if ((sort_cmds = (SortCmds)xmalloc(sizeof(struct sort_cmds_st), 1)) == NULL) {
                trs_print_line("Could not generate command syntax help");
                return;
            }
            cmd_node = (CLICmdNode)cmd_list->data;
            sort_cmds->name = cmd_node->name;
            if (!cmd_node->desc)
                sort_cmds->desc = "No description available";
            else
                sort_cmds->desc = cmd_node->desc;
            list_head = slist_append(list_head, sort_cmds);
        }   
        avail_list = avail_list->next;
    }

    /* Ugly implementation of the selection sort algorithm
     * due that I'm using a single linked list */
    base = tail1 = list_head;
    while (base) {
        if (!base->next)
            break;

        head = base == list_head ? 1 : 0;

        smallest = base;
        curr = base->next;
        while (curr) {
            node1 = (SortCmds)smallest->data;
            node2 = (SortCmds)curr->data;
            if (strcmp(node1->name, node2->name) > 0)
                smallest = curr;
            curr = curr->next;
        }

        /* A smaller element than the base was found? */
        if (smallest != base) {
            tail2 = base;
            while (tail2->next != smallest)
                tail2 = tail2->next;

            /* Head of the list? */
            if (head) {
                /* Swap adyacent elements: simple swap */
                if (base->next == smallest) {
                    base->next = smallest->next;
                    smallest->next = base;
                    base = tail1 = smallest;
                }
                /* Swap non-adyacent elements */
                else {
                    aux = base->next;
                    base->next = smallest->next;
                    tail2->next = base;
                    smallest->next = aux;
                    base = tail1 = smallest;
                }
            }
            else {
                /* Swap last 2 elements */
                if (!base->next->next) {
                    aux = base;
                    tail1->next = smallest;
                    smallest->next = aux;
                    base->next = NULL;
                }
                else {
                    /* Swap adyacent elements: simple swap */
                    if (base->next == smallest) {
                        aux = smallest;
                        tail1->next->next = smallest->next;
                        smallest->next = base;
                        tail1->next = aux;
                        base = tail1 = smallest;
                    }
                    /* Swap non-adyacent elements */
                    else {
                        aux = tail1->next->next;
                        tail1->next->next = smallest->next;
                        tail2->next = tail1->next;
                        smallest->next = aux;
                        tail1->next = smallest;
                        base = tail1 = smallest;
                    }
                }
            }
        }
        /* The smallest element is the base */
        else
            tail1 = base;

        if (head)
            list_head = smallest;

        base = base->next;
    }

    trs_print_table_config(2, 's', 18, 's', 50);
    aux = list_head;
    while (aux) {
        node1 = aux->data;
        printf("%s:%s\r\n", node1->name, node1->desc);
        trs_print_row(ptable, node1->name, node1->desc);
        aux = aux->next;
    }

    free(list_head);
}


