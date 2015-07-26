/*
 * privileded_mode.c --
 *
 * Author -- Pedro Aguilar <paguilar@junkerhq.net>
 *
 * Copyright (c) 2004 - 2006 Pedro Aguilar
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
#include <sys/types.h>
#include <sys/wait.h>

/*
 * Needed for the printing funcs, the graph and generic defines
 * inside the Tarasca project
 */
#include "tarasca.h"

/* Declare the global pointer to the list of modes, commands and arguments */
extern CLIMatrix  matrix;

/*
 * Description: Implementation of the 'configure' command 
 *              that enables a configuration mode
 */
void privileged_mode_configure(SList arg_list) {
    char        *mode_name = NULL,
                res_buff[LOG_BUFF * 2];;

    if ((mode_name = trs_get_param_name(arg_list, "interface")) == NULL) {
        //FIXME
        //trs_error("Parameter not found");
        return;
    }

    if (!strcmp(mode_name, "iface") || !strcmp(mode_name, "system")) {
        if (!trs_graph_mode_change(mode_name)) {
            sprintf(res_buff, "Could not change to mode '%s'\n", mode_name);
            trs_print_str(res_buff);
        }
    }
    else {
        sprintf(res_buff, "Unrecognized mode '%s'\n", mode_name);
        trs_print_str(res_buff);
    }

    return;
}


/*
 * Description: Implementation of the 'passwd' command 
 *              that modifies the system access passwd
 */
void privileged_mode_passwd(SList arg_list) {
#ifdef ENABLE_AUTH
    short int     i,
                  size;
    char          *ptr = NULL,
                  filename[NAME_SIZE],
                  passwd[PASSWD_LEN],
                  gen_passwd[PASSWD_LEN];
    md5_context   ctx;
    unsigned char md5sum[16];

    /* Get current passwd from user */
    if (login_read_input(passwd, "Current password: ") <= 0) {
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

    if ((ptr = trs_config_file_param_read(filename, "passwd_login")) == NULL) {
        trs_print_line("FATAL: Password not found\n");
        return;
    }

    if (memcmp(gen_passwd, ptr, 16) != 0)  {
        trs_print_line("Password incorrect!\n");
        xfree(ptr);
        return;
    }

    xfree(ptr);
 
    /* Since the user given passwd and the DB passwd match,
       get the new passwd and save it into DB */
    reset_buff(passwd, PASSWD_LEN);
    login_read_input(passwd, "New password: ");

    /* Empty passwd? */
    if (passwd[0] == '\n') {
        printf("Password invalid! It cannot be null\n");
        return;
    }

    /* Substitute the trailing '\n' by a '\0' */
    ptr = passwd;
    *(ptr + strlen(passwd) - 1) = '\0';

    /* Check length */
    size = strlen(passwd);
    if (size < PASSWD_MIN || size > PASSWD_LEN) {
        printf("Password invalid! It must have between 6 and 32 characters\n");
        return;
    }

    for (i = 0; i < size; i++) {
        if (!isalnum((int)passwd[i])) {
            trs_print_line("Password invalid! It must contain only alphanumeric characters\n");
            return;
        }
    }

    /* Calculate MD5 hash */
    md5_starts(&ctx);
    md5_update(&ctx, (uint8 *)passwd, strlen(passwd));
    md5_finish(&ctx, md5sum);

    reset_buff(gen_passwd, PASSWD_LEN);
    for (i = 0; i < 16; i++) 
        sprintf(gen_passwd, "%s%02x", gen_passwd, md5sum[i]);

    if (!trs_config_file_param_write(filename, "passwd_login", gen_passwd)) {
        trs_print_line("FATAL: Could not save passwd\n");
        return;
    }

    trs_print_line("Success: Password changed!");
#else
    trs_print_line("Operation not enabled... See configure option '--enable-auth'");
#endif /*ENABLE_AUTH */
    return;
}


/*
 * Description: Implementation of the 'enable-passwd' command 
 *              that modifies the enable passwd
 */
void privileged_mode_enable_pwd(SList arg_list) {
#ifdef ENABLE_AUTH
    short int     i,
                  size;
    char          *ptr = NULL,
                  filename[NAME_SIZE],
                  passwd[PASSWD_LEN],
                  gen_passwd[PASSWD_LEN];
    md5_context   ctx;
    unsigned char md5sum[16];

    /* Get current passwd from user */
    if (login_read_input(passwd, "Current password: ") <= 0) {
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
 
    /* Since the user given passwd and the DB passwd are equal,
       get the new passwd and save it into DB */
    reset_buff(passwd, PASSWD_LEN);
    login_read_input(passwd, "New password: ");

    /* Empty passwd? */
    if (passwd[0] == '\n') {
        printf("Password invalid! It cannot be null\n");
        return;
    }

    /* Substitute the trailing '\n' by a '\0' */
    ptr = passwd;
    *(ptr + strlen(passwd) - 1) = '\0';

    /* Check length */
    size = strlen(passwd);
    if (size < PASSWD_MIN || size > PASSWD_LEN) {
        printf("Password invalid! It must have between 6 and 32 characters\n");
        return;
    }

    for (i = 0; i < size; i++) {
        if (!isalnum((int)passwd[i])) {
            trs_print_line("Password invalid! It must contain only alphanumeric characters\n");
            return;
        }
    }

    /* Calculate MD5 hash */
    md5_starts(&ctx);
    md5_update(&ctx, (uint8 *)passwd, strlen(passwd));
    md5_finish(&ctx, md5sum);

    reset_buff(gen_passwd, PASSWD_LEN);
    for (i = 0; i < 16; i++) 
        sprintf(gen_passwd, "%s%02x", gen_passwd, md5sum[i]);

    if (!trs_config_file_param_write(filename, "passwd_enable", gen_passwd)) {
        trs_print_line("FATAL: Could not save passwd\n");
        return;
    }

    trs_print_line("Success: Password changed!");
#else
    trs_print_line("Operation not enabled... See configure option '--enable-auth'");
#endif /*ENABLE_AUTH */
    return;
}


/*      
 * Description: Open a shell session
 */
void privileged_mode_shell(SList arg_list) {
    int   status;
    pid_t pid;
    
    pid = fork();
    if (pid < 0) {
        trs_print_str("Failure: Could not open shell");
    }
    else if (pid == 0) {
        if (execl("/bin/bash", "/bin/bash", "-i", NULL) == -1) {
            trs_print_line("Failure: Could not open bash");
        }
    }
    else {
        waitpid(pid, &status, 0);
    }
}       



