/*
 * graph_static_generator.h --
 *
 * Author -- Pedro Aguilar <paguilar@junkerhq.net>
 *
 * Copyright (c) 2006 Pedro Aguilar
 *
 * Description: Defines for graph_static_generation.c
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
 * DATA TYPES
 *****************************/

const char *warning = " \
/* \n \
 * DO NOT EDIT!!! \n\n \
 * This file is generated automatically by the program 'graph_static_generator'.\n \
 * Contains the data needed for building the data structures that will contain \n \
 * the info of the configuration files. \n \
 */ \n\n\
";

const char *author = " \
/* \n \
 * graph_static -- \n \
 * \n \
 * Author -- Pedro Aguilar <paguilar@junkerhq.net> \n \
 * \n \
 * Copyright (c) 2006 Pedro Aguilar \n \
 */ \n\n\
";

const char *license = " \
/* \n \
 * This program is free software; you can redistribute it and/or modify \n \
 * it under the terms of the GNU General Public License as published by \n \
 * the Free Software Foundation; either version 2 of the License, or \n \
 * (at your option) any later version. \n \
 * \n \
 * This program is distributed in the hope that it will be useful, \n \
 * but WITHOUT ANY WARRANTY; without even the implied warranty of \n \
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU \n \
 * General Public License for more details. \n \
 * \n \
 * You should have received a copy of the GNU General Public License \n \
 * along with this program; if not, write to the Free Software \n \
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA \n \
 */ \n\n\
";

const char comments[5][128] = {
"/*****************************\n\
 * INCLUDES\n\
 *****************************/\n\n",
"/*****************************\n\
 * MACROS\n\
 *****************************/\n\n",
"/*****************************\n\
 * DATA TYPES\n\
 *****************************/\n\n",
"/*****************************\n\
 * FUNCTION DECLARATION\n\
 *****************************/\n\n",
"/*****************************\n\
 * FUNCTION IMPLEMENTATION\n\
 *****************************/\n\n"
};

