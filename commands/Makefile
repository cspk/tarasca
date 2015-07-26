#
# Makefile --
#
# Author -- Pedro Aguilar <paguilar@junkerhq.net>
#
# Copyright (c) 2004, 2005 Pedro Aguilar
#
# Description: 	Makefile for the user-implemented commands
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

ROOT_DIR = ../
include /home/arkady/Downloads/tarasca-0.3.2/Makefile.defs

CONFIG_FLAGS += -I"/home/arkady/Downloads/tarasca-0.3.2/rhal" -I"/home/arkady/Downloads/tarasca-0.3.2/cli" -I"/home/arkady/Downloads/tarasca-0.3.2/utils"

CMDS_SRCS += $(wildcard *.c)
CMDS_OBJS += $(patsubst %.c, %.o, $(CMDS_SRCS))


all: $(CMDS_OBJS)


$(CMDS_OBJS): $(CMDS_SRCS)
	@echo CC $^
	@$(CC) $(CFLAGS) $(CONFIG_FLAGS) $(DEBUG_FLAGS) -c $^


clean:
	@echo Cleaning $(CMDS_DIR)...
	@rm -f $(CMDS_OBJS) *.o


