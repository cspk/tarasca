/*
 * graph_static_generator.c -- 
 *
 * Author -- Pedro Aguilar <paguilar@junkerhq.net>
 *
 * Copyright (c) 2006 Pedro Aguilar
 *
 * Description: This program is intended to be executed only by the
 *     Makefile when building the whole project.
 *     This program creates the 'dynamic' graph from the .cli configuration
 *     files that are validated (by the lexer and parser), 
 *     Each node is created according to the obtained data that can be 
 *     changed without recompiling the source code.
 *     The output of this program, even if it's called from a shell/script, 
 *     is C source code that contains the necessary code for generating the 
 *     'static' graph.
 *     It's is called 'static' because it's nodes are created from the C 
 *     source code that this program produces, it's nodes cannot be changed 
 *     unless these source files are modified and recompiled.
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
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <glob.h>
#include <errno.h>

#include "misc.h"
#include "graph.h"
#include "rhal.h"
#include "config.h"
#include "graph_static_generator.h"

/*****************************
 * DATA TYPES
 *****************************/

extern CLIMatrix  matrix;

/*****************************
 * FUNCTION IMPLEMENTATION
 *****************************/

/*
 * Description: Writes the *.h *.c files that contain 
 *     the source code that creates the static graph.
 */
static int create_graph_generator() {
    int          i,
                 ret = -1;
    char         path[NAME_SIZE],
                 source_h_file[NAME_SIZE],
                 source_c_file[NAME_SIZE];
    FILE         *dst = NULL;
    glob_t       filelist;
    SList        mode_list = NULL,
                 cmd_list = NULL,
                 arg_list = NULL,
                 arg_parent_list = NULL,
                 avail_list = NULL;
    CLIModeNode  mode_node = NULL;
    CLICmdNode   cmd_node = NULL;
    CLIArgNode   arg_node = NULL,
                 arg_parent_node = NULL;
    CLIAvailNode avail_node = NULL;

    sprintf(source_h_file, "graph_static.h");
    sprintf(source_c_file, "graph_static.c");

    /* Generate the .h file */
    if ((dst = fopen(source_h_file, "w+")) == NULL) {
        fprintf(stderr, "%s\n", strerror(errno));
        return 0;
    }

    fputs(warning, dst);
    fputs(author, dst);
    fputs(license, dst);
    fputs(comments[3], dst);
    fprintf(dst, "int create_static_graph(void);\n");
    fprintf(dst, "int create_cmd_func_link(void);\n\n");

    fclose(dst);
 
    /* Generate the .c file */
    if ((dst = fopen(source_c_file, "w+")) == NULL) {
        fprintf(stderr, "%s\n", strerror(errno));
        return 0;
    }

    /* Write comments */
    fputs(warning, dst);
    fputs(author, dst);
    fputs(license, dst);
    fputs("\n", dst);

    /* Write project includes */
    fputs(comments[0], dst);
    fprintf(dst, "\
#include \"misc.h\"\n\
#include \"graph.h\"\n\n"
    );

    /* Write user includes */
    sprintf(path, "%s/*.h", TARASCA_CMD_PATH);
#if 0
    sprintf(path, "%s/%s/*.h", TARASCA_PATH, TARASCA_CMD_PATH);
#endif
    ret = glob(path, 0, NULL, &filelist);
    if (!ret) {
        for (i = 0; i < filelist.gl_pathc; i++)
            fprintf(dst, "#include \"%s\"\n", filelist.gl_pathv[i]);
        globfree(&filelist);
    }
    fputs("\n", dst);

    fputs(comments[2], dst);
    fprintf(dst, "\
extern CLIMatrix matrix;\n\
extern char **cmd_tbl;\n\
extern void (**fn_tbl)(SList);\n\n"
    );

    fputs(comments[4], dst);

    fprintf(dst, "\
int create_static_graph() {\n\n\
    SList       mode_list;\n\
    CLIModeNode mode_node;\n\n\
\
    matrix = graph_matrix_init();\n\n"
    );

    /* Create graph */
    mode_list = matrix->graph;
    while (mode_list) {
        /* Insert mode */
        mode_node = (CLIModeNode)mode_list->data;
        fprintf(dst, "    graph_mode_insert(\"%s\");\n", mode_node->name);

        if (mode_node->prompt)
            fprintf(dst, "    graph_mode_insert_prompt(\" %s \");\n", mode_node->prompt);

        fputs("\n", dst);

        /* Insert cmd */
        cmd_list = (SList)mode_node->cmd_list;
        while (cmd_list) {
            cmd_node = (CLICmdNode)cmd_list->data;

            fprintf(dst, "    graph_cmd_insert(\"%s\");\n", cmd_node->name);
            fprintf(dst, "    graph_insert_desc(\" %s \");\n", cmd_node->desc);

            /* Insert arg */
            arg_list = (SList)cmd_node->arg_list;
            while (arg_list) {
                arg_node = (CLIArgNode)arg_list->data;

                fprintf(dst, "    graph_arg_insert(\"%s\");\n", arg_node->sid);

                if (arg_node->name)
                    fprintf(dst, "    graph_arg_insert_name(\" %s \");\n", arg_node->name);

                fprintf(dst, "    graph_arg_insert_value(\"%s\");\n", arg_node->value);
                fprintf(dst, "    graph_arg_insert_prio(\"%s\");\n", arg_node->prio);
                fprintf(dst, "    graph_insert_desc(\" %s \");\n", arg_node->desc);
                
                // This optional arg depends of another one
                if (arg_node->parent != -1) {
                    arg_parent_list = (SList)cmd_node->arg_list;
                    while (arg_parent_list) {
                        arg_parent_node = (CLIArgNode)arg_parent_list->data;

                        if (arg_node->parent == arg_parent_node->id)
                            fprintf(dst, "    graph_arg_insert_parent(\"%s\");\n", 
                                arg_parent_node->name);
                        arg_parent_list = arg_parent_list->next; 
                    }
                }

                fputs("\n", dst);
                arg_list = arg_list->next;
            }

            fputs("\n", dst);
            cmd_list = cmd_list->next;
        }

        fputs("\n\n", dst);
        mode_list = mode_list->next;
    }

    /* Once we have the whole graph with its modes and cmds, */
    /* insert to each mode the available list of cmds */
    fprintf(dst, "\n\
    mode_list = matrix->graph;\n\
    mode_node = (CLIModeNode)mode_list->data;\n\n");

    mode_list = matrix->graph;
    while (mode_list) {
        mode_node = (CLIModeNode)mode_list->data;

        fprintf(dst, "    /* Exporting modes and cmds of mode '%s' (%d) */", 
            mode_node->name, mode_node->id);

        avail_list = (SList)mode_node->avail;
        while (avail_list) {
            int         mode_id,
                        cmd_id;

            avail_node = (CLIAvailNode)avail_list->data;
            mode_id = avail_node->mode_id;
            cmd_id = avail_node->cmd_id;

            if (mode_id != -1 && cmd_id != -1) {
                    fprintf(dst, "\n\
    graph_mode_insert_avail(mode_node, %d, %d);",
                        mode_id, cmd_id
                    );
            }
            avail_list = avail_list->next;
        }
        if (mode_list->next) {
            fprintf(dst, "\n\
    mode_list = mode_list->next;\n\
    mode_node = (CLIModeNode)mode_list->data;\n\n");
        }

        mode_list = mode_list->next;
    }

    fputs("\n\n    return 1;\n}\n\n", dst);

    /* Static graph creation end... */

    /* Write the code that creates the link between the given */
    /* cmd and its function */
    fprintf(dst, "\
int create_cmd_func_link() {\n\
    int         cmd_cnt = 0;\n\
\n\
    /* Both arrays end up with a NULL element */\n\
    cmd_cnt = graph_mode_cmd_count(matrix->graph) + 1;\n\
\n\
    /* Create table (array) of functions */\n\
    if ((fn_tbl = (void (**)(SList))xmalloc(sizeof(void (*)(SList)), cmd_cnt)) == NULL)\n\
        return 0;\n\
\n\
    /* Create table (array) of cmds */\n\
    if ((cmd_tbl = (char **)xmalloc(sizeof(char *), cmd_cnt)) == NULL)\n\
        return 0;\n\n"
    );

    i = 0;
    mode_list = matrix->graph;
    while (mode_list) {
        mode_node = (CLIModeNode)mode_list->data;

        cmd_list = (SList)mode_node->cmd_list;
        while (cmd_list) {
            cmd_node = (CLICmdNode)cmd_list->data;

            fprintf(dst, "    cmd_tbl[%d] = \"%s\";\n", i, cmd_node->name);
            fprintf(dst, "    fn_tbl[%d] = %s;\n\n", i, cmd_node->func_name);

            i++;
            cmd_list = cmd_list->next;
        }
        mode_list = mode_list->next;
    }

    fprintf(dst, "    cmd_tbl[%d] = (char *)NULL;\n", i);
    fprintf(dst, "    fn_tbl[%d] = (void (*)(SList))NULL;\n\n", i);
    fputs("    return 1;\n}\n", dst);

    fclose(dst);
    return 1;
}


/*
 * Description: Usage
 */
static void use() {
    fprintf(stderr, "\nThis program must not be used from a shell or script\n");
    fprintf(stderr, "It's called only at compile time\n\n");
}


/*
 * Description: Create the 'dynamic' graph and
 *     call the 'static' graph generator function
 */
int main(int argc, char **argv) {

    if (argc != 1) {
        use();
        return -1;
    }

    /* Init symbol table and graph */
    if (!rhal_parse_init()) {
        fprintf(stderr, "%s: FATAL: Could not initialize symbols table\n", argv[0]);
        return -1;
    }

    /* Parse config files and create graph */
    if (!rhal_parse_files()) {
        fprintf(stderr, "%s: FATAL: Could not create dynamic graph\n", argv[0]);
        return -1;
    }

    debug_rhal_printf("Total number of cmds: %d\n", graph_mode_cmd_count(matrix->graph));

    graph_reset_ptrs();

    /* Write the files with the static graph */
    if (!create_graph_generator()) {
        fprintf(stderr, "%s: FATAL: Could not generate the files containing the source code of the static graph\n", argv[0]);
        return -1;
    }

    return 0;
}
