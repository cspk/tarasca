 /* 
  * DO NOT EDIT!!! 

  * This file is generated automatically by the program 'graph_static_generator'.
  * Contains the data needed for building the data structures that will contain 
  * the info of the configuration files. 
  */ 

 /* 
  * graph_static -- 
  * 
  * Author -- Pedro Aguilar <paguilar@junkerhq.net> 
  * 
  * Copyright (c) 2006 Pedro Aguilar 
  */ 

 /* 
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

#include "misc.h"
#include "graph.h"

#include "/home/paguilar/projects/tarasca/tarasca-0.3.2/commands/config_mode.h"
#include "/home/paguilar/projects/tarasca/tarasca-0.3.2/commands/privileged_mode.h"
#include "/home/paguilar/projects/tarasca/tarasca-0.3.2/commands/user_mode.h"

/*****************************
 * DATA TYPES
 *****************************/

extern CLIMatrix matrix;
extern char **cmd_tbl;
extern void (**fn_tbl)(SList);

/*****************************
 * FUNCTION IMPLEMENTATION
 *****************************/

int create_static_graph() {

    SList       mode_list;
    CLIModeNode mode_node;

    matrix = graph_matrix_init();

    graph_mode_insert("user_mode");
    graph_mode_insert_prompt(" %h> ");

    graph_cmd_insert("quit");
    graph_insert_desc(" Quit the shell ");

    graph_cmd_insert("exit");
    graph_insert_desc(" Exit the current mode ");

    graph_cmd_insert("help");
    graph_insert_desc(" Display the syntax of the given command ");
    graph_arg_insert("cmd");
    graph_arg_insert_value("string");
    graph_arg_insert_prio("mandatory");
    graph_insert_desc(" Name of the command ");


    graph_cmd_insert("show");
    graph_insert_desc(" Sample command: Display information about an item ");
    graph_arg_insert("item");
    graph_arg_insert_value("string");
    graph_arg_insert_prio("mandatory");
    graph_insert_desc(" Sample parameter: Item from which information has to be gathered ");

    graph_arg_insert("item_instance");
    graph_arg_insert_value("number");
    graph_arg_insert_prio("mandatory");
    graph_insert_desc(" Sample parameter: Instance of the item ");


    graph_cmd_insert("?");
    graph_insert_desc(" Display the list of available commands in the current mode ");

    graph_cmd_insert("enable");
    graph_insert_desc(" Enable privileged mode ");



    graph_mode_insert("privileged_mode");
    graph_mode_insert_prompt(" %h# ");

    graph_cmd_insert("configure");
    graph_insert_desc(" Configure a system's module ");
    graph_arg_insert("interface");
    graph_arg_insert_value("string");
    graph_arg_insert_prio("mandatory");
    graph_insert_desc(" Module to be configured. Values: { iface | system } ");


    graph_cmd_insert("passwd-login");
    graph_insert_desc(" Modify system access password ");

    graph_cmd_insert("passwd-enable");
    graph_insert_desc(" Modify enable password ");

    graph_cmd_insert("shell");
    graph_insert_desc(" Open a shell session ");



    graph_mode_insert("iface");
    graph_mode_insert_prompt(" %h(iface)# ");

    graph_cmd_insert("ifconfig");
    graph_insert_desc(" Configure the given network interface ");
    graph_arg_insert("interface");
    graph_arg_insert_value("string");
    graph_arg_insert_prio("mandatory");
    graph_insert_desc(" Interface name ");

    graph_arg_insert("ip");
    graph_arg_insert_name(" ip ");
    graph_arg_insert_value("ipaddr");
    graph_arg_insert_prio("optional");
    graph_insert_desc(" IP address ");

    graph_arg_insert("netmask");
    graph_arg_insert_name(" netmask ");
    graph_arg_insert_value("networkmask");
    graph_arg_insert_prio("optional");
    graph_insert_desc(" Netmask ");
    graph_arg_insert_parent("ip");




    graph_mode_insert("system");
    graph_mode_insert_prompt(" %h(system)# ");

    graph_cmd_insert("sys-info");
    graph_insert_desc(" Display system information ");




    mode_list = matrix->graph;
    mode_node = (CLIModeNode)mode_list->data;

    /* Exporting modes and cmds of mode 'user_mode' (0) */
    mode_list = mode_list->next;
    mode_node = (CLIModeNode)mode_list->data;

    /* Exporting modes and cmds of mode 'privileged_mode' (1) */
    graph_mode_insert_avail(mode_node, 0, 0);
    graph_mode_insert_avail(mode_node, 0, 1);
    graph_mode_insert_avail(mode_node, 0, 2);
    graph_mode_insert_avail(mode_node, 0, 3);
    graph_mode_insert_avail(mode_node, 0, 4);
    mode_list = mode_list->next;
    mode_node = (CLIModeNode)mode_list->data;

    /* Exporting modes and cmds of mode 'iface' (2) */
    graph_mode_insert_avail(mode_node, 0, 0);
    graph_mode_insert_avail(mode_node, 0, 1);
    graph_mode_insert_avail(mode_node, 0, 2);
    graph_mode_insert_avail(mode_node, 0, 3);
    graph_mode_insert_avail(mode_node, 0, 4);
    graph_mode_insert_avail(mode_node, 1, 3);
    graph_mode_insert_avail(mode_node, 3, 0);
    mode_list = mode_list->next;
    mode_node = (CLIModeNode)mode_list->data;

    /* Exporting modes and cmds of mode 'system' (3) */
    graph_mode_insert_avail(mode_node, 0, 0);
    graph_mode_insert_avail(mode_node, 0, 1);
    graph_mode_insert_avail(mode_node, 0, 2);
    graph_mode_insert_avail(mode_node, 0, 3);
    graph_mode_insert_avail(mode_node, 0, 4);
    graph_mode_insert_avail(mode_node, 1, 3);

    return 1;
}

int create_cmd_func_link() {
    int         cmd_cnt = 0;

    /* Both arrays end up with a NULL element */
    cmd_cnt = graph_mode_cmd_count(matrix->graph) + 1;

    /* Create table (array) of functions */
    if ((fn_tbl = (void (**)(SList))xmalloc(sizeof(void (*)(SList)), cmd_cnt)) == NULL)
        return 0;

    /* Create table (array) of cmds */
    if ((cmd_tbl = (char **)xmalloc(sizeof(char *), cmd_cnt)) == NULL)
        return 0;

    cmd_tbl[0] = "quit";
    fn_tbl[0] = user_mode_quit;

    cmd_tbl[1] = "exit";
    fn_tbl[1] = user_mode_exit;

    cmd_tbl[2] = "help";
    fn_tbl[2] = user_mode_help;

    cmd_tbl[3] = "show";
    fn_tbl[3] = user_mode_show;

    cmd_tbl[4] = "?";
    fn_tbl[4] = user_mode_list_cmds;

    cmd_tbl[5] = "enable";
    fn_tbl[5] = user_mode_enable;

    cmd_tbl[6] = "configure";
    fn_tbl[6] = privileged_mode_configure;

    cmd_tbl[7] = "passwd-login";
    fn_tbl[7] = privileged_mode_passwd;

    cmd_tbl[8] = "passwd-enable";
    fn_tbl[8] = privileged_mode_enable_pwd;

    cmd_tbl[9] = "shell";
    fn_tbl[9] = privileged_mode_shell;

    cmd_tbl[10] = "ifconfig";
    fn_tbl[10] = iface_mode_ifconfig;

    cmd_tbl[11] = "sys-info";
    fn_tbl[11] = system_mode_info;

    cmd_tbl[12] = (char *)NULL;
    fn_tbl[12] = (void (*)(SList))NULL;

    return 1;
}
