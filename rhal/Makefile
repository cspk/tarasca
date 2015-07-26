#
# Makefile --
#
# Author -- Pedro Aguilar <paguilar@junkerhq.net>
#
# Copyright (c) 2004 - 2006 Pedro Aguilar
#
# Description: 	Makefile for the Request Handler Abstraction Layer 
#               (rhal) files
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


RHAL_SRCS = rhal.c graph.c rhal_symbol_table.c print.c graph_static_generator.c
RHAL_OBJS = y.tab.o lex.yy.o
RHAL_OBJS += $(patsubst %.c, %.o, $(RHAL_SRCS))

STATIC_GRAPH = graph_static_gen


all: $(RHAL_OBJS)


# Generates y.tab.c and y.tab.h from rhal_parser.y
y.tab.c:
	@echo YACC $@
	@$(YACC) -d rhal_parser.y

# Generates lex.yy.c from rhal_lexer.l
lex.yy.c:
	@echo LEX $@
	@$(LEX) rhal_lexer.l


y.tab.o: y.tab.c
	@echo CC $^
	@$(CC)  $(YACC_FLAGS) -I../$(UTILS_DIR) -c y.tab.c -DYYDEBUG=0

lex.yy.o: lex.yy.c
	@echo CC $^
	@$(CC) -O2 $(DEBUG_FLAGS) -I../$(UTILS_DIR) -c $^

graph.o: graph.c
	@echo CC $^
	@$(CC) $(CFLAGS) $(DEBUG_FLAGS) -I../$(UTILS_DIR) -c $^

rhal_symbol_table.o: rhal_symbol_table.c
	@echo CC $^
	@$(CC) $(CFLAGS) $(DEBUG_FLAGS) -I../$(UTILS_DIR) -c $^

print.o: print.c
	@echo CC $^
	@$(CC) $(CFLAGS) $(CONFIG_FLAGS) $(DEBUG_FLAGS) -I../$(UTILS_DIR) -c $^

HOST_CC = gcc
generate_graph: graph_static_generator.c
	@echo HOST_CC $^
	@$(HOST_CC) $(CFLAGS) $(DEBUG_FLAGS) -I../$(UTILS_DIR) -c $^
	@$(HOST_CC) -o $(STATIC_GRAPH) $(RHAL_OBJS) \
		$(wildcard $(ROOT_DIR)/$(UTILS_DIR)/*.o)
	@echo Generating static graph...
	./$(STATIC_GRAPH)
	@echo Static graph generated!
	@echo Cleaning temporary files...
	@rm -f $(STATIC_GRAPH) graph_static_generator.o

graph_static_generator.o: graph_static_generator.c
	@echo CC graph_static.c
	@@$(CC) $(CFLAGS) $(DEBUG_FLAGS) -I../$(RHAL_DIR) -I../$(CLI_DIR) \
		-I../$(UTILS_DIR) -c graph_static.c

rhal.o: rhal.c	
	@echo CC $^
	@$(CC) $(CFLAGS) $(DEBUG_FLAGS) -I../$(UTILS_DIR) \
		-I../$(CLI_DIR) -c $^


clean_lex_yacc:
	@echo Removing generated files from lex/yacc 
	@rm -f y.tab.* lex.yy.*


clean:
	@echo Cleaning $(RHAL_DIR)...
	@rm -f $(STATIC_GRAPH) $(RHAL_OBJS) *.o
