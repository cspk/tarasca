/* vi: set sw=4 ts=4: */
/*
 * print.h --
 *
 * Author -- Pedro Aguilar <paguilar@junkerhq.net>
 *
 * Copyright (c) 2006 Pedro Aguilar
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

#ifndef _PRINT_H
#define _PRINT_H

/*****************************
 * INCLUDES
 *****************************/

#include "graph.h"

/*****************************
 * MACROS
 *****************************/

/* The ISO/ANSI standard screen size is 80x24, so
 * we can have a max of 10 columns of 8 'spaces' */
#define PRINT_MAX_COLS 10
#define PRINT_PAGER    24


/*

#define trs_param_check(name, p, type) \
        static inline type *trs_check_##name(void) { return(p); }

#define trs_param_check_byte(name, p) trs_param_check(name, p, unsigned char)
#define trs_param_check_short(name, p) trs_param_check(name, p, short)
#define trs_param_check_ushort(name, p) trs_param_check(name, p, unsigned short)
#define trs_param_check_int(name, p) trs_param_check(name, p, int)
#define trs_param_check_uint(name, p) trs_param_check(name, p, unsigned int)
#define trs_param_check_long(name, p) trs_param_check(name, p, long)
#define trs_param_check_ulong(name, p) trs_param_check(name, p, unsigned long)
#define trs_param_check_charp(name, p) trs_param_check(name, p, char *)
*/

/*****************************
 * DATA TYPES
 *****************************/

typedef struct print_table_st PrintTable;

struct print_table_st {
    short int cols;
    char      type[PRINT_MAX_COLS];
    int       size[PRINT_MAX_COLS];
    int       pager;
};

/*****************************
 * FUNCTION DECLARATION
 *****************************/

void trs_print_table_config(short int, ...);
void trs_print_row(PrintTable, ...);
void trs_print_str(char *);
void trs_print_line(char *);
void trs_print_nl(int);

void trs_print_init_msg(char *);
void trs_print_end_msg(char *);
void trs_print_cmd_syntax(CLICmdNode, int);
void trs_print_cmd_list(SList);

#endif /* _PRINT_H */
