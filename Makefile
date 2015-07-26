#
# Makefile --
#
# Author -- Pedro Aguilar <paguilar@junkerhq.net>
#
# Copyright (c) 2004 - 2007 Pedro Aguilar
#
# Description: 	Makefile for the whole project
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


include Makefile.defs


all:
	@rm -f $(CLI_DIR)/$(TARGET)
	@$(MAKE) -C $(UTILS_DIR) $@
	@$(MAKE) -C $(CMDS_DIR) $@
	@$(MAKE) -C $(RHAL_DIR) $@
	@$(MAKE) -C $(CLI_DIR) $@
	@echo Executable \"$(TARGET)\" created in \"$(CLI_DIR)/\"

clean_lex_yacc:
	@$(MAKE) -C $(RHAL_DIR) $@

clean:
	@$(MAKE) -C $(UTILS_DIR) $@
	@$(MAKE) -C $(RHAL_DIR) $@
	@$(MAKE) -C $(CMDS_DIR) $@
	@$(MAKE) -C $(CLI_DIR) $@


