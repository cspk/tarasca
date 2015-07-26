/*
 * config_mode.c --
 *
 * Author -- Pedro Aguilar <paguilar@junkerhq.net>
 *
 * Copyright (c) 2004 - 2006 Pedro Aguilar
 *
 * Description: This is a sample file. All the definitions of the
 *     user-defined functions must be in an .c file which name
 *     must be the same as the .h file
 *     This sample file shows the structure and properties 
 *     of modes, commands and arguments.
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

/*
 * Needed for sysconf() and getpagesize()
 */
#include <stdio.h>
#include <unistd.h>
#include <sys/sysinfo.h>

/*
 * Needed for the printing funcs, the graph and generic defines
 * inside the Tarasca project
 */
#include "tarasca.h"

/*
 * Global pointer to the graph that contains the list of modes
 * Every configuration file must declare it.
 */
extern SList  graph;

/*
 * Global pointer to the reconfigurable table used for
 * displaying tables of width 10 columns, height any rows and
 * with columns of diffent data types
 */
extern PrintTable    ptable;

/*****************************************
 * Commands of the 'iface' mode
 *****************************************/

/*
 * Description: Implementation of the 'iface' command 
 *              that configures an interface (sample)
 */
void iface_mode_ifconfig(SList arg_list) {
    char *if_name = NULL,
         *ipaddr = NULL,
         *netmask = NULL,
         str[128];

    if ((if_name = trs_get_param_str(arg_list, "interface")) == NULL) {
        trs_print_line("Failure: Could not get 'interface'");
        return;
    }

    if (trs_exists_param(arg_list, "ip")) {
        if ((ipaddr = trs_get_param_str(arg_list, "ip")) == NULL) {
            trs_print_line("Failure: Could not get IP address");
            return;
        }
    }

    if (trs_exists_param(arg_list, "netmask")) {
        if (!ipaddr) {
            trs_print_line("Failure: Netmask invalid without IP address");
            return;
        }
        if ((netmask = trs_get_param_str(arg_list, "netmask")) == NULL) {
            trs_print_line("Failure: Could not get netmask");
            return;
        }
    }

    if (!ipaddr)
        sprintf(str, "Sample: Shows inteface %s info", if_name);
    else {
        sprintf(str, "Sample command:\n\tConfiguring:\n\tInterface %s\n\tIP: %s\n\t", 
            if_name, ipaddr);
        if (netmask)
            sprintf(str, "%sNetmask: %s", str, netmask);
    }

    trs_print_line(str);
    trs_print_nl(1);
}


/*****************************************
 * Commands of the 'system' mode
 *****************************************/

/*
 * Description: Implementation of the 'info' command 
 *              that displays system's information
 */
void system_mode_info(SList arg_list) {
    int page_size,
        total_pages,
        avail_phy_pages;
#ifdef LINUX
    int proc_number;

    proc_number = get_nprocs_conf();
#endif

    page_size = getpagesize();
    total_pages = sysconf(_SC_PHYS_PAGES);
    avail_phy_pages = sysconf (_SC_AVPHYS_PAGES);

    /*
     * Create a table with 2 columns.
     * Print table header
     */
    trs_print_table_config(2, 's', 30, 's', 10);
    trs_print_row(ptable, "System characteristic", "Size");
    trs_print_nl(1);
    /*
     * Column 1 is of type string (s) and takes 30 spaces to be displayed
     * Column 2 is of type integer (d) and takes 10 spaces to be displayed
     */
    trs_print_table_config(2, 's', 30, 'd', 10);
    trs_print_row(ptable, "Page size", page_size);
    trs_print_row(ptable, "Total pages", total_pages);
    trs_print_row(ptable, "Available physical pages", avail_phy_pages);
#ifdef LINUX
    trs_print_row(ptable, "Number of processors", proc_number);
#endif
    trs_print_nl(1);
}

