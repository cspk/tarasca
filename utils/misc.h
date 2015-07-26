/* vi: set sw=4 ts=4: */
/*
 * misc.h --
 *
 * Author -- Pedro Aguilar <paguilar@junkerhq.net>
 *
 * Copyright (c) 2004 - 2006 Pedro Aguilar
 *
 * Description: Defines for misc.c
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

#ifndef _MISC_H
#define _MISC_H

/*****************************
 * INCLUDES
 *****************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <stdarg.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>

/*****************************
 * MACROS
 *****************************/

#define u_int              unsigned int
#define u_short            unsigned short
#define SOCK               int

#define	LOG_BUFF           1024
#define NAME_SIZE          256

#define ENABLE             1
#define DISABLE            0

#define FD_IN              stdin
#define FD_OUT             stdout

#define TARASCA_CONF_FILE  "tarasca.conf"
#define TARASCA_CMD_FILE   "tarasca_command_files.conf"
#define TARASCA_LOG_FILE   "tarasca.log"

#define ENABLE_STR         "enable"
#define DISABLE_STR        "disable"
#define LEFT_SIDE          "left"
#define RIGHT_SIDE         "right"

#define MAC_STRLEN         18

#define TRS_ERR(msg...)                                 \
    do {                                                \
        log_error(__FUNCTION__, __LINE__, msg);         \
    } while (0)


/*****************************
 * DATA TYPES
 *****************************/

enum logs_enum {
    LOG_ERROR,
    LOG_WARN,
    LOG_MALLOC,
    LOG_RHAL,
    LOG_GRAPH,
    LOG_PARSER,
    LOG_SHELL,
    LOG_COMM,
    LOG_WWW
}log_types;


typedef struct List_st *    List;

struct List_st {
    char ch;
    List next;
};


/*****************************
 * FUNCTION DECLARATIION
 *****************************/

void *	  xmalloc(size_t, int);
void * 	  xrealloc(void *, size_t);
void   	  xfree(void *);
char *    xstrdup(const char *);
//int       log_error(int, char *, ...);
void      log_error(const char *, int, char *, ...);
ssize_t   safe_read(int, void *, size_t);
int       get_line_cmd_file(FILE *, char *);
char *    trs_config_file_param_read(char *, char *);
int       trs_config_file_param_write(char *, char *, char *);
void      reset_buff(char *, int);
char *    int2str(int);
int       str2mac(unsigned char *, char *);
short int check_space(char *);
short int have_double_quotes(char *);

/* Simple linked list of chars */
List   list_new();
int    list_empty(List);
List   list_append(List, char);
int    list_count(List);
char * list_2buff(List);

/* Data type checking funcs */
short int is_bool(char *);
short int is_number(char *);
short int is_string(char *);
short int is_hex(char *);
short int is_ipaddr(char *);
short int is_netmask(char *);
short int is_macaddr(char *);
short int is_range(char *);

/* Debug funcs */
void      debug_printf(const char *, ...);
void      debug_rhal_printf(const char *, ...);
void      debug_shell_printf(const char *, ...);

#endif	/* _MISC_H */
