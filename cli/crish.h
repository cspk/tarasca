/*
 * crish.h --
 *
 * Author -- Pedro Aguilar <paguilar@junkerhq.net>
 *
 * Copyright (c) 2004 - 2006 Pedro Aguilar
 *
 * Description: Defines used by crish.c
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

#ifndef CRISH_H
#define CRISH_H

/*****************************
 * INCLUDES
 *****************************/

#include "../rhal/graph.h"

/*****************************
 * MACROS
 *****************************/

#define SHELL          "Crish"

#define TRUE           ((int) 1)
#define FALSE          ((int) 0)

#define LOGIN_LEN      32
#define PASSWD_LEN     32
#define PASSWD_MIN     6

/*****************************
 * DATA TYPES
 *****************************/

/*****************************
 * FUNCTION DECLARATION
 *****************************/

/* Terminal and packet control funcs */
short int       get_login_passwd();
char *          get_prompt(char *);

/* Input processing */
char *          trs_get_param_name(SList, const char *);
int             trs_exists_param(SList, const char *);
int             trs_get_param_num(SList, const char *);
double          trs_get_param_range(SList, const char *, const char *);
char *          trs_get_param_str(SList, const char *);

#endif /* CRISH_H */
