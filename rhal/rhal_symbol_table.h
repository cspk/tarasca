/*
 * rhal_symbol_table.h --
 *
 * Author -- Pedro Aguilar <paguilar@junkerhq.net>
 *
 * Copyright (c) 2004 - 2006 Pedro Aguilar
 *
 * Description: Defines for rhal_symbol_table.h
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


#ifndef _RHAL_SYMTABLE_H
#define _RHAL_SYMTABLE_H

/*****************************
 * DATA TYPES
 *****************************/

typedef struct symtable_st *	SymTable;

/* Symbol table struct used for storing 
 * the reserved words and their values */
struct symtable_st {
    char	*name;
    char	*type;
    SymTable	next;
};

/*****************************
 * FUNCTION DECLARATION
 *****************************/

int		SymInit();
SymTable 	SymLook(char *);


#endif /* _RHAL_SYMTABLE_H */

