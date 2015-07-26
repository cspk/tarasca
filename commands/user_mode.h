/*
 * user_mode.h --
 *
 * Author -- Pedro Aguilar <paguilar@junkerhq.net>
 *
 * Copyright (c) 2004, 2005 Pedro Aguilar
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

/* Needed for the Tarasca project funcs */
#include "tarasca.h"

/*****************************
 * FUNCTION DECLARATION
 *****************************/

void user_mode_exit(SList);
void user_mode_help(SList);
void user_mode_show(SList);
void user_mode_quit(SList);
void user_mode_list_cmds(SList);
void user_mode_test(SList);
void user_mode_enable(SList);

