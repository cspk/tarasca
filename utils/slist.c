/*
 * slist.c --
 *
 * Author -- Pedro Aguilar <paguilar@junkerhq.net>
 *
 * Copyright (c) 2004 - 2006 Pedro Aguilar
 *
 * Description: Implementation of a Single List
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
 *
 */

#include "slist.h"
#include "misc.h"

SList slist_new() {
    return (SList)NULL;
}


/*
 * Description: Adds a new element to the end of the SList
 */
SList slist_append(SList node, void *data) {
    SList	ptr = NULL,
		aux = NULL;

    if ((ptr = (SList)xmalloc(sizeof(struct SList_st), 1)) == NULL) {
	return NULL;
    }

    ptr->data = data;
    ptr->next = NULL;

    aux = node;
    if (!aux)
	return ptr;

    while (aux->next) {
	aux = aux->next;
    }
    aux->next = ptr;

    return node;
}


/*
 * Description: Inserts a new element into the slist at the given position
 */
SList slist_insert(SList node, void *data, int position) {
    int		counter = 0;
    SList       ptr = NULL,
                aux = NULL;

    if (position < 0 || slist_count(node) < position) {
	node = slist_append(node, data);
	return node;
    }

    aux = node;
    if (position > 0)
        counter++;

    while (aux && counter != position) {
	aux = aux->next;
	counter++;
    }

    if ((ptr = (SList)xmalloc(sizeof(struct SList_st), 1)) == NULL) {
        return NULL;
    }

    ptr->data = data;

    if (!position) {
	ptr->next = aux;
	node = ptr;
    }
    else {
        ptr->next = aux->next;
        aux->next = ptr;
    }

    return node;
}


/*
 * Description:	Removes the element from a slist at the given position. 
 *    If the position is negative or greater than the size of the slist, 
 *    then the slist is unchanged.
 */
SList slist_remove(SList node, int position) {
    int		counter = 0;
    SList	ptr = NULL;

    if (position < 0 || slist_count(node) < position) {
	return node;
    }

    ptr = node;
    if (position > 0)
        counter++;

    while (ptr && counter != position) {
        ptr = ptr->next;
        counter++;
    }

    if (!position) {
	node = node->next;
	free(ptr);
    }
    else {
	ptr--;
	ptr->next = ptr->next->next;
	free(ptr->next);
    }

    return node;
}


/*
 * Description: Count the number of elements in the slist
 */
int slist_count(SList node) {
    int		counter = 0;
    SList	ptr = NULL;

    ptr = node;
    while (ptr) {
	counter++;
	ptr = ptr->next;
    }

    return counter;
}


/*
 * Description: Gets the position of the given element in the 
 *     slist starting from 0
 */
int slist_get_position(SList node, SList dest) {
    int		counter = 0;
    SList	ptr = NULL;

    ptr = node;
    while (ptr) {
	if (ptr == dest)
	    return counter;
	else {
	    ptr = ptr->next;
	    counter++;
	}
    }

    return -1;
}


/*
 * Description: Tells if a slist is empty
 */
int slist_empty(SList node) {
    return node == NULL ? 1 : 0;
}

