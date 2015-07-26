/*
 * rhal_symbol_table.c --
 *
 * Author -- Pedro Aguilar <paguilar@junkerhq.net>
 *
 * Copyright (c) 2004 - 2006 Pedro Aguilar
 *
 * Description: Handle the symbol table used by the parser
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
#include <string.h>

#include "rhal_symbol_table.h"
#include "misc.h"

/*****************************
 * DATA TYPES
 *****************************/

SymTable		symrec;

/*****************************
 * FUNCTION IMPLEMENTATION
 *****************************/

/*
 * Description: Insert a pair keyword-value into the symbol table
 */
static SymTable SymInsert(SymTable ptr, char *keyword, char *value) {
    SymTable	p = NULL,
		q = NULL;

    if ((p = (SymTable)xmalloc(sizeof(struct symtable_st), 1)) == NULL) {
	TRS_ERR("Could not allocate memory for SymTable!");
	return 0;
    }

    p->name = keyword;
    p->type = value;
    p->next = NULL;

    if (!ptr) {
	return p;
    }

    q = ptr;
    while (q->next) {
	q = q->next;
    }
    q->next = p;

    return ptr;
}


/*
 * Description: Search a pair keyword-value in the symbol table
 *
 * Returns: 	A ptr to the struct matching the given keyword
 *		or NULL otherwise
 */
SymTable SymLook(char *keyword) {
    SymTable	ptr = NULL;

    ptr = symrec;
    while (ptr) {
	if (!strcmp(ptr->type, keyword))
	    return ptr;
	ptr = ptr->next;
    }

    return (SymTable)NULL;
}


/*
 * Description: Init the symbol table of reserved words
 */
int SymInit() {
    SymTable	ptr = NULL;

    ptr = SymInsert(ptr, "MODE", "mode");
    ptr = SymInsert(ptr, "PROMPT", "prompt");
    ptr = SymInsert(ptr, "COMMAND", "command");
    ptr = SymInsert(ptr, "FUNC", "func");
    ptr = SymInsert(ptr, "AVAIL", "avail");
    ptr = SymInsert(ptr, "ARG", "arg");
    ptr = SymInsert(ptr, "NAME", "name");
    ptr = SymInsert(ptr, "VALUE", "value");
    ptr = SymInsert(ptr, "PRIORITY", "prio");
    ptr = SymInsert(ptr, "PARENT", "parent");
    ptr = SymInsert(ptr, "DESC", "desc");
    ptr = SymInsert(ptr, "BOOL", "bool");
    ptr = SymInsert(ptr, "STRING", "string");
    ptr = SymInsert(ptr, "NUMBER", "number");
    ptr = SymInsert(ptr, "HEX", "hex");
    ptr = SymInsert(ptr, "IPADDR", "ipaddr");
    ptr = SymInsert(ptr, "NETWORKMASK", "networkmask");
    ptr = SymInsert(ptr, "MACADDR", "macaddr");
    ptr = SymInsert(ptr, "RANGE", "range");
    ptr = SymInsert(ptr, "OPTIONAL", "optional");
    ptr = SymInsert(ptr, "MANDATORY", "mandatory");

    symrec = ptr;
    return 1;
}

