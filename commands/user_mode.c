/*
 * user_mode.c --
 *
 * Author -- Pedro Aguilar <paguilar@junkerhq.net>
 *
 * Copyright (c) 2004 - 2006 Pedro Aguilar
 *
 * NOTE: The cmds that have to be available to all modes have to be
 *       declared in this file that is the last one to be included
 *       in the command_files.conf. The reason is that if we want a 
 *       cmd to be available to a mode that hasn't been defined it 
 *       won't be inserted into that mode's availability list, 
 *       thus we have first to define all modes and then declared 
 *       the cmds that will be available to those modes.
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

#include <stdio.h>

/* Needed for the Tarasca project funcs */
#include "tarasca.h"

#include "privileged_mode.h"

/* Declare the global pointer to the list of modes, commands and arguments */
extern CLIMatrix matrix;

/*
 * Global pointer to the reconfigurable table used for
 * displaying tables of width 10 columns, height any rows and
 * with columns of diffent data types
 */
extern PrintTable    ptable;

/*
 * Description: Implementation of the 'quit' command 
 *              that quits the shell
 */
void user_mode_quit(SList arg_list) {
#ifdef ENABLE_LAST_LOGIN
    char    *ptr = NULL,
            msg_buff[LOG_BUFF],
            filename[NAME_SIZE];
    time_t  timeval;

    timeval = time(0);
    sprintf(msg_buff, "\"%s", ctime(&timeval));

    /* Remove the enter added by ctime() */
    ptr = msg_buff + strlen(msg_buff) - 1;
    *ptr++ = '"';
    *ptr = '\0';

    sprintf(filename, "%s/%s", TARASCA_CONF_PATH, TARASCA_CONF_FILE);
    /*sprintf(filename, "%s/%s/%s", TARASCA_PATH, TARASCA_CONF_PATH, TARASCA_CONF_FILE);*/
    if (!trs_config_file_param_write(filename, "last_login", msg_buff)) {
        trs_print_line("FATAL: Could not save last login info\n");
        free(matrix);
        exit(EXIT_SUCCESS);
    }

#endif /* ENABLE_LAST_LOGIN*/
    trs_print_end_msg(END_MSG);
    free(matrix);
    exit(EXIT_SUCCESS);
}


/*
 * Description: Implementation of the 'exit' command 
 *              that exits an access mode
 */
void user_mode_exit(SList arg_list) {
    CLIModeNode  mode_node = NULL;

    mode_node = (CLIModeNode)matrix->mode->data;
    if (mode_node->parent == -1) {
#ifdef ENABLE_LAST_LOGIN
        char   *ptr = NULL,
               filename[NAME_SIZE],
               msg_buff[LOG_BUFF];
        time_t timeval;

        timeval = time(0);
        sprintf(msg_buff, "\"%s", ctime(&timeval));

        /* Remove the enter added by ctime() */
        ptr = msg_buff + strlen(msg_buff) - 1;
        *ptr++ = '"';
        *ptr = '\0';

        sprintf(filename, "%s/%s", TARASCA_CONF_PATH, TARASCA_CONF_FILE);
        /*sprintf(filename, "%s/%s/%s", TARASCA_PATH, TARASCA_CONF_PATH, TARASCA_CONF_FILE);*/
        if (!trs_config_file_param_write(filename, "last_login", msg_buff)) {
            trs_print_line("FATAL: Could not save last login info\n");
            free(matrix);
            exit(EXIT_SUCCESS);
        }
#endif /* ENABLE_LAST_LOGIN */
        trs_print_end_msg(END_MSG);
        free(matrix);
        exit(EXIT_SUCCESS);
    }

    if (!graph_mode_id_exist(mode_node->parent)) {
        trs_print_line("Could not exit mode\n");
        return;
    }
 
    return;
}


/*
 * Description: Implementation of the 'help' command 
 *              that display the syntax of a given cmd
 */
void user_mode_help(SList arg_list) {
    char         *cmd = NULL;
    SList        avail_list = NULL,
                 cmd_list = NULL;
    short int    cmd_found = 0;
    CLICmdNode   cmd_node = NULL;
    CLIModeNode  mode = NULL;
    CLIAvailNode avail_node = NULL;

    if ((cmd = trs_get_param_str(arg_list, "cmd")) == NULL) {
        trs_print_line("Syntax: help <cmd>");
        return;
    }

    /* If the cmd does not exist in the current mode, 
       search it in the mode's availability list */
    if (!graph_cmd_exist(matrix->mode, cmd)) {
        mode = (CLIModeNode)matrix->mode->data;
        avail_list = mode->avail;
        while (avail_list) {
            avail_node = (CLIAvailNode)avail_list->data;
            if ((cmd_list = graph_mode_cmd_id_exist(avail_node->mode_id, avail_node->cmd_id)) != NULL) {
                cmd_node = (CLICmdNode)cmd_list->data;
                if (!strcmp(cmd, cmd_node->name)) {
                    cmd_found = 1;
                    break;
                }
            }
            avail_list = avail_list->next;
        }
        if (!cmd_found) {
            trs_print_line("Type <TAB> or 'show' for a list of available commands");
            return;
        }
    }
    else {
        mode = (CLIModeNode)matrix->mode->data;
        cmd_list = (SList)mode->cmd_list;
        while (cmd_list) {
            cmd_node = (CLICmdNode)cmd_list->data;
            if (!strcmp(cmd_node->name, cmd)) {
                cmd_found = 1;
                break;
            }
            cmd_list = cmd_list->next;
        }
        if (!cmd_found) {
            trs_print_line("Type <TAB> or 'show' for a list of available commands");
            return;
        }
    }
    trs_print_cmd_syntax(cmd_node, 1);
    return;
}


/*
 * Description: Implementation of the 'show' command 
 *              that display the list of available commands 
 *              in the current mode
 */
void user_mode_show(SList arg_list) {
    trs_print_cmd_list(matrix->mode);
}


/*
 * Description: Implementation of the '?' command 
 *              that display the list of available commands 
 *              in the current mode
 */
void user_mode_list_cmds(SList arg_list) {
    trs_print_cmd_list(matrix->mode);
    return;
}

/*
 * Description: Implementation of the 'enable' command 
 *              that enables the privileged mode
 */
void user_mode_enable(SList arg_list) {
#ifdef ENABLE_AUTH
    short int     i,
                  size;
    char          mode_name[16],
                  *ptr = NULL,
                  filename[NAME_SIZE],
                  passwd[PASSWD_LEN],
                  gen_passwd[PASSWD_LEN];
    md5_context   ctx;
    unsigned char md5sum[16];
#else
    char          mode_name[16];
#endif /* ENABLE_AUTH */

#ifdef ENABLE_AUTH
    /* Get current passwd from user */
    if (login_read_input(passwd, "Password: ") <= 0) {
        printf("Password incorrect!\n");
        return;
    }

    /* Empty passwd? */
    if (passwd[0] == '\n') {
        printf("Password incorrect!\n");
        return;
    }

    /* Substitute the trailing '\n' by a '\0' */
    ptr = passwd;
    *(ptr + strlen(passwd) - 1) = '\0';

    /* Check length */
    size = strlen(passwd);
    if (size < PASSWD_MIN || size > PASSWD_LEN) {
        printf("Password incorrect!\n");
        return;
    }

    for (i = 0; i < size; i++) {
        if (!isalnum((int)passwd[i])) {
            printf("%s\n\n", PASSWD_BAD);
            continue;
        }
    }

    /* Calculate MD5 hash */
    md5_starts(&ctx);
    md5_update(&ctx, (uint8 *)passwd, strlen(passwd));
    md5_finish(&ctx, md5sum);

    reset_buff(gen_passwd, PASSWD_LEN);
    for (i = 0; i < 16; i++) 
        sprintf(gen_passwd, "%s%02x", gen_passwd, md5sum[i]);

    sprintf(filename, "%s/%s", TARASCA_CONF_PATH, TARASCA_CONF_FILE);
    /*sprintf(filename, "%s/%s/%s", TARASCA_PATH, TARASCA_CONF_PATH, TARASCA_CONF_FILE);*/

    if ((ptr = trs_config_file_param_read(filename, "passwd_enable")) == NULL) {
        trs_print_line("FATAL: Password not found\n");
        return;
    }

    if (memcmp(gen_passwd, ptr, 16) != 0)  {
        trs_print_line("Password incorrect!\n");
        xfree(ptr);
        return;
    }

    xfree(ptr);
#endif /* ENABLE_AUTH */

    reset_buff(mode_name, 16);
    sprintf(mode_name, "privileged_mode");

    if (!trs_graph_mode_change(mode_name))
        fprintf(FD_OUT, "Could not change to mode %s\n", mode_name);

    return;
}

