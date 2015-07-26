#
# Makefile --
#
# Author -- Pedro Aguilar <paguilar@junkerhq.net>
#
# Copyright (c) 2004 - 2006 Pedro Aguilar
#
# Description: 	Makefile for the utils: Lists, dynamic memory allocation, logs
#
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
#


ROOT_DIR = ..
include $(ROOT_DIR)/Makefile.defs


ifeq ($(ENABLE_AUTH), 1)
    UTILS_SRCS += slist.c misc.c md5.c 
else
    UTILS_SRCS += slist.c misc.c
endif

UTILS_OBJS += $(patsubst %.c, %.o, $(UTILS_SRCS))

all: $(UTILS_OBJS)

slist.o: slist.c
	@echo CC $^
	@$(CC) $(CFLAGS) $(DEBUG_FLAGS) -c $^

misc.o: misc.c
	@echo CC $^
	@$(CC) $(CFLAGS) $(CONFIG_FLAGS) $(DEBUG_FLAGS) -I$(ROOT_DIR)/$(RHAL_DIR) -c $^

md5.o: md5.c
	@echo CC $^
	@$(CC) $(CFLAGS) -c $^

clean:
	@echo Cleaning $(UTILS_DIR)...
	@rm -f $(UTILS_OBJS) *.o

