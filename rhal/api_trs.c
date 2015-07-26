/*
 * api_trs.c --
 *
 * Author -- Pedro Aguilar <paguilar@junkerhq.net>
 *
 * Copyright (c) 2007 Pedro Aguilar
 *
 * Description: Defines the API for the CLI/GUI users
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

#include "config.h"
#include "rhal.h"
#include "graph.h"
#include "print.h"
#include "misc.h"
#include "slist.h"

/*****************************
 * MACROS
 *****************************/

/*****************************
 * DATA TYPES
 *****************************/
#define trs_error(char *msg, ...)     TRS_ERR(msg...)

/*****************************
 * FUNCTION IMPLEMENTATION
 *****************************/

void trs_error(char *s, ...) {
    TRS_ERR(s);
}
