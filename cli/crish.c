/* vi: set sw=4 ts=4: */
/*
 * crish.c --
 *
 * Author -- Pedro Aguilar <paguilar@junkerhq.net>
 *
 * Copyright (c) 2004 - 2006 Pedro Aguilar
 *
 * Description: Command Line Interface
 *
 * Part of this code is taken from the hush and lash shells used in BusyBox
 *
 * hush -- a HUmble SHell for BusyBox
 * Copyright (C) 2000,2001  Larry Doolittle  <larry@doolittle.boa.org>
 *
 * lash -- the BusyBox Lame-Ass SHell
 * Copyright (C) 1999-2004 by Erik Andersen <andersen@codepoet.org>
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

#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <stdarg.h>
#include <termios.h>

#include "config.h"
#include "crish.h"
#include "cmdedit.h"
#include "graph.h"
#include "rhal.h"
#include "print.h"
#include "misc.h"
#include "graph_static.h"

#ifdef ENABLE_AUTH
#include "md5.h"
#endif

/*****************************
 * DATA TYPES
 *****************************/

extern CLIMatrix  matrix;
extern PrintTable        ptable;
static unsigned int shell_terminal;

/*****************************
 * FUNCTION IMPLEMENTATION
 *****************************/

/**************************************************
 * Terminal and packet control funcs
 *************************************************/

/* 
 * Make sure we have a controlling tty. If we get started under 
 * a job aware app (like bash for example), make sure we are now 
 * in charge so we don't fight over who gets the foreground
 */
static void setup_job_control(void) {       
    static pid_t shell_pgrp;
    /* Loop until we are in the foreground. */
    while (tcgetpgrp(shell_terminal) != (shell_pgrp = getpgrp()))
        kill (- shell_pgrp, SIGTTIN);

    /* Ignore interactive and job-control signals. */
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTERM, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGCHLD, SIG_IGN);
        
    /* Put ourselves in our own process group. */
    setsid();
    shell_pgrp = getpid();
    setpgid(shell_pgrp, shell_pgrp);
                        
    /* Grab control of the terminal. */
    tcsetpgrp(shell_terminal, shell_pgrp);
}


/*
 * Get login and passwd using a striped version of 
 * cmdedit_read_input()
 */
#ifdef ENABLE_AUTH
short int get_login_passwd() {
    short int     i,
                  size,
                  fail = 1;
    char          *ptr = NULL,
                  filename[NAME_SIZE],
                  passwd[PASSWD_LEN],
                  gen_passwd[PASSWD_LEN],
                  *str = NULL;
    md5_context   ctx;
    unsigned char md5sum[16];

    sprintf(filename, "%s/%s", TARASCA_CONF_PATH, TARASCA_CONF_FILE);
    /*sprintf(filename, "%s/%s/%s", TARASCA_PATH, TARASCA_CONF_PATH, TARASCA_CONF_FILE);*/

    do {
        /* Get passwd */
        if (login_read_input(passwd, PASSWD_MSG) <= 0)
            return 0;
    
        /* Empty passwd? */
        if (passwd[0] == '\n') {
            printf("%s\n\n", PASSWD_BAD);
            continue;
        }
    
        /* Substitute the trailing '\n' by a '\0' */
        ptr = passwd;
        *(ptr + strlen(passwd) - 1) = '\0';

        /* Check length */
        size = strlen(passwd);
        if (size < PASSWD_MIN || size > PASSWD_LEN) {
            printf("%s\n\n", PASSWD_BAD);
            continue;
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

        if ((str = trs_config_file_param_read(filename, "passwd_login")) == NULL) {
            fprintf(FD_OUT, "Bad login\n");
            continue;
        }
        /*printf("Generated: %s, File: %s\n", gen_passwd, str); */

        if (memcmp(gen_passwd, str, 16) != 0) {
            printf("%s\n\n", PASSWD_BAD);
            xfree(str);
            continue;
        }
        else
            fail = 0;
    } while (fail);

    xfree(str);

    return 1;
}
#endif /*ENABLE_AUTH */


char * get_prompt(char *prompt) {
    CLIModeNode mode;
    char        *raw_prompt = NULL,
                hostname[32];
    int         i = 0,
                prompt_len;

    mode = matrix->mode->data;
    if (!mode) {
        if ((prompt = (char *)xmalloc(sizeof(char), 4)) == NULL) {
            return NULL;
        }
        reset_buff(prompt, 4);
        sprintf(prompt, "$> ");
        return prompt;
    }

    raw_prompt = mode->prompt;
    if (!raw_prompt) {
        if ((prompt = (char *)xmalloc(sizeof(char), 4)) == NULL) {
            return NULL;
        }
        reset_buff(prompt, 4);
        sprintf(prompt, "$> ");
        return prompt;
    }

    if ((prompt = (char *)xmalloc(sizeof(char), strlen(raw_prompt) + 33)) == NULL) {
        return NULL;
    }

    reset_buff(prompt, strlen(raw_prompt) + 33);
    reset_buff(hostname, 32);
    prompt_len = strlen(raw_prompt);
    while (i < prompt_len) {
        if ((*raw_prompt == '%') && (*(raw_prompt + 1) == 'h') && (i < prompt_len - 1)) {
            gethostname(hostname, 32);
            sprintf(prompt, "%s%s", prompt, hostname);
            raw_prompt++;
        }
        else {
            sprintf(prompt, "%s%c", prompt, *raw_prompt);
        }
        raw_prompt++; i++;
    }
    sprintf(prompt, "%s ", prompt);
    return prompt;
}

/*
 * Converts the input line buffer to a linked list of tokens
 */
static SList tokenize_input_line(char *input_line) {
    InputToken  token_node;
    SList       input_list;
    char        *ptr = NULL;
    int         start = 0,
                end = 0,
                i = 0,
                token_size = 0,
                input_len = 0;

    ptr = input_line;
    input_len = strlen(input_line);
    input_list = slist_new();

    debug_shell_printf("List of tokens: ");
    for (i = 0; i < input_len; i++) {
        /* Set the start/end delimiters when we find a space/tab */
        if (!start) {
            if (input_line[i] != 9 && input_line[i] != 32) {
                start = i + 1;
            }
        }
        else if (start && !end) {
            if (input_line[i] == 9 || input_line[i] == 32)
                end = i;
        }

        /* We have a token delimited by space/tab */
        if (start && end) {
            start--;
            token_size = end - start;
            ptr = ptr - token_size;

            if ((token_node = (InputToken)xmalloc(sizeof(struct input_token_st), 1)) == NULL)
                return (SList)NULL;

            if ((token_node->value = (char *)xmalloc(sizeof(char), token_size + 1)) == NULL)
                return (SList)NULL;

            reset_buff(token_node->value, token_size + 1);
            strncpy(token_node->value, ptr, token_size);
            token_node->sid = (char *)NULL;
            debug_shell_printf("'%s' -> ", token_node->value);

            input_list = slist_append(input_list, (void *)token_node);
            if (!input_list)
                return (SList)NULL;
            /* Reset vars and ptr so we can keep creating tokens */
            start = end = 0;
            ptr = ptr + token_size;
        }
        ptr++;
    }

    /* Normally, for the last token we don't have a space/tab as delimiter */
    if (start && !end) {
        start--; 
        token_size = input_len - start;
        ptr = ptr - token_size;

        if ((token_node = (InputToken)xmalloc(sizeof(struct input_token_st), 1)) == NULL)
            return (SList)NULL;

        if ((token_node->value = (char *)xmalloc(sizeof(char), token_size + 1)) == NULL)
            return (SList)NULL;

        reset_buff(token_node->value, token_size + 1);
        strncpy(token_node->value, ptr, token_size);
        token_node->sid = (char *)NULL;
        debug_shell_printf("'%s' -> ", token_node->value);

        input_list = slist_append(input_list, (void *)token_node);
        if (!input_list)
            return (SList)NULL;
    }
    debug_shell_printf("NULL\n");

    return input_list;
}

/*
 * This func is executed when the program is called 
 * with only one arg: the CLI
 */
static int get_line(FILE *fd) {
    int         valid = CMD_INVALID;
    char        *ptr = NULL;
    SList       input_list = NULL;
    CLIModeNode mode = NULL;
    static int  size = 1;
    static char command[BUFSIZ];

    /* Enable command line editing */
    while (size) {
        if (cmdedit_read_input(command) == 0)
            return 0;

        size = strlen(command);
        /* Do nothing if we received just an enter */
        if (size == 1 && command[0] == '\n')
            continue;

        /* Substitute the trailing '\n' by a '\0' */
        ptr = command;
        *(ptr + strlen(command) - 1) = '\0';

        input_list = tokenize_input_line(command); 
        if (input_list == NULL) {
            TRS_ERR("Could not tokenize input!");
            continue;
        }

        valid = rhal_validate_input_list(input_list);

        switch (valid) {
            /* If the cmd was right, execute it */
            case CMD_OK:
                if (!rhal_exec_cmd((CLICmdNode)matrix->cmd->data, input_list->next))
#ifdef ENABLE_LOGS
                    TRS_ERR("Could not execute command");
#else
                    fprintf(FD_OUT, "Could not execute command\n");
#endif /* ENABLE_LOGS */
                break;

            /* Cmd not found */
            case CMD_NOT_FOUND:
#ifdef ENABLE_LOGS
                TRS_ERR("Command not found. Type <TAB> or '?'");
#else
                fprintf(FD_OUT, "Command not found. Type <TAB> or '?'\n");
#endif /* ENABLE_LOGS */
                break;

            /* At least one of the cmd args was wrong */
            case CMD_INVALID:
#ifdef ENABLE_LOGS
                TRS_ERR("Invalid syntax. Type 'help <cmd>'");
#else
                fprintf(FD_OUT, "Invalid syntax. Type 'help <cmd>'\n");
#endif /* ENABLE_LOGS */
                break;

            default:
#ifdef ENABLE_LOGS
                TRS_ERR("Unknown validation error code %d", valid);
#else
                fprintf(FD_OUT, "Unknown validation error code %d\n", valid);
#endif /* ENABLE_LOGS */
                break;
        }

        free(input_list);

        /* Move global ptr 'cmd' to the beginning of the mode's cmd list */
        mode = (CLIModeNode)matrix->mode->data;
        matrix->cmd = (SList)mode->cmd_list;

    } /* while */
    return 0;
}

/*
 * This func is executed when the program is called 
 * with more than one arg: called from a GUI
 */
#ifdef ENABLE_GUI
static int get_line_from_gui(int argc, char **argv) {
    int         i,
                valid = CMD_INVALID;
    SList       input_list = NULL;
    static char command[BUFSIZ];

    reset_buff(command, BUFSIZ);

    for (i = 1; i <= --argc; i++)
        sprintf(command, "%s %s", command, argv[i]);

    input_list = tokenize_input_line(command); 
    if (input_list == NULL) {
        TRS_ERR("Could not tokenize input");
        return 0;
    }

    valid = rhal_validate_input_list(input_list);

    switch (valid) {
        /* If the cmd was right, execute it */
        case CMD_OK:
            if (!rhal_exec_cmd((CLICmdNode)matrix->cmd->data, input_list->next))
#ifdef ENABLE_LOGS
                TRS_ERR("Could not execute command");
#else
                fprintf(FD_OUT, "Could not execute command\n");
#endif /* ENABLE_LOGS */
            break;

        /* Cmd not found */
        case CMD_NOT_FOUND:
#ifdef ENABLE_LOGS
            TRS_ERR("Command not found. Type <TAB> or '?'");
#else
            fprintf(FD_OUT, "Command not found. Type <TAB> or '?'\n");
#endif /* ENABLE_LOGS */
            break;

        /* At least one of the cmd args was wrong */
        case CMD_INVALID:
#ifdef ENABLE_LOGS
            TRS_ERR("Invalid syntax. Type 'help <cmd>'");
#else
            fprintf(FD_OUT, "Invalid syntax. Type 'help <cmd>'\n");
#endif /* ENABLE_LOGS */
            break;

        default:
#ifdef ENABLE_LOGS
            TRS_ERR("Unknown validation error code %d", valid);
#else
            fprintf(FD_OUT, "Unknown validation error code %d\n", valid);
#endif /* ENABLE_LOGS */
            break;
    }

    free(input_list);

    return valid == CMD_OK ? 1 : 0;
}
#endif /* ENABLE_GUI */


/**************************************************
 * Input processing funcs
 *************************************************/

/*
 * Search in the input list the given 'name' and return it
 */
char * trs_get_param_name(SList input_list, const char *name) {
    SList      list = NULL;
    InputToken token = NULL;

    list = input_list;
    while (list) {
        token = (InputToken)list->data;
        if (!strcmp(token->sid, name)) {
            debug_shell_printf("Token name: %s\n", token->sid);
            return token->value;
        }
        list = list->next;
    }
    return NULL;
}


/*
 * Search in the input list the value of the given 'name'
 *              This func must be called when the param's type is BOOL or positive NUMBER
 */
int trs_exists_param(SList input_list, const char *name) {
    SList      list = NULL,
               arg_list = NULL;
    InputToken token = NULL;
    CLICmdNode cmd_node = NULL;
    CLIArgNode arg_node = NULL;

    /* Search 'name' in the input list */
    list = input_list;
    while (list) {
        token = (InputToken)list->data;
        if (!strcmp(token->sid, name)) {
            cmd_node = (CLICmdNode)matrix->cmd->data;
            arg_list = (SList)cmd_node->arg_list;
            /* If 'name' was found in the input list, */
            /* then search it in the graph's arg list */
            while (arg_list) {
                arg_node = (CLIArgNode)arg_list->data;
                /* In a madatory arg this member is NULL */
                if (arg_node->name) {
                    /* The second part of the 'if' makes the difference between the param's name and */
                    /* the param's value. If they're equal we have the param's name (trs_get_param_name()), */
                    /* if they're different we have the param's value */
                    if (!strcmp(arg_node->sid, name) && strcmp(arg_node->name, token->value)) {
                        return 1;
                    } 
                }
                else {
                    if (!strcmp(arg_node->sid, name)) {
                        return 1;
                    }
                }
                arg_list = arg_list->next;
            }
        }   
        list = list->next;
    }
    debug_shell_printf("Name '%s' not found!\n", name);
    return 0;
}


/*
 * Search in the input list the value of the given 'name'
 * This func must be called when the param's type is BOOL or NUMBER (int)
 * If the param's type is BOOL but its value is invalid or not found returns -1
 * If the param's type is NUMBER but its value is invalid or not found returns (1 << 30) * (-1)
 *
 * WARNING: ((1 << 30) * (-1)) is not likely to be used, but could appear 
 *          and produce a false positive
 */
int trs_get_param_num(SList input_list, const char *name) {
    SList      list = NULL,
               arg_list = NULL;
    InputToken token = NULL;
    CLICmdNode cmd_node = NULL;
    CLIArgNode arg_node = NULL;

    /* Search 'name' in the input list */
    list = input_list;
    while (list) {
        token = (InputToken)list->data;
        if (!strcmp(token->sid, name)) {
            cmd_node = (CLICmdNode)matrix->cmd->data;
            arg_list = (SList)cmd_node->arg_list;
            /* If 'name' was found in the input list, */
            /* then search it in the graph's arg list */
            while (arg_list) {
                arg_node = (CLIArgNode)arg_list->data;
                /* In a madatory arg this member is NULL, so we have here an optional arg */
                if (arg_node->name) {
                    /* The second part of the 'if' makes the difference between the param's name and */
                    /* the param's value. If they're equal we have the param's name (trs_get_param_name()), */
                    /* if they're different we have the param's value */
                    if (!strcmp(arg_node->sid, name) && strcmp(arg_node->name, token->value)) {
                        if (!strcmp(arg_node->value, "bool")) {
                            debug_shell_printf("Token value: %s\n", token->value);
                            if ((!strcasecmp(token->value, ENABLE_STR)) || (atoi(token->value) == 1))
                                return 1;
                            else if ((!strcasecmp(token->value, DISABLE_STR)) || (atoi(token->value) == 0))
                                return 0;
                            else
                                return -1;
                        }
                        else {
                            debug_shell_printf("Token value: %s\n", token->value);
                            /* Accept negative numbers */
                            if (*(token->value) == '-')
                                return atoi(token->value + 1) * (-1);
                            else
                                return atoi(token->value);
                        }
                    } 
                }
                /* A mandatory arg */
                else {
                    if (!strcmp(arg_node->sid, name)) {
                        if (!strcmp(arg_node->value, "bool")) {
                            debug_shell_printf("Token value: %s\n", token->value);
                            if ((!strcasecmp(token->value, ENABLE_STR)) || (atoi(token->value) == 1))
                                return 1;
                            else if ((!strcasecmp(token->value, DISABLE_STR)) || (atoi(token->value) == 0))
                                return 0;
                            else
                                return -1;
                        }
                        else {
                            debug_shell_printf("Token value: %s\n", token->value);
                            /* Accept negative numbers */
                            if (*(token->value) == '-')
                                return atoi(token->value + 1) * (-1);
                            else
                                return atoi(token->value);
                        }
                    }
                }
                arg_list = arg_list->next;
            }
        }   
        list = list->next;
    }
    debug_shell_printf("Name '%s' not found!\n", name);
    /* Warning: return a number that is not likely to be used */
    return (1 << 30) * (-1);
}


/*
 * Description: Search in the input list the value of the given 'name'
 *              This func must be called when the param's type is RANGE
 */
double trs_get_param_range(SList input_list, const char *side, const char *name) {
    int        minus = 0;
    char       *limit = NULL,
               *ptr = NULL,
               number[64];
    SList      list = NULL,
               arg_list = NULL;
    InputToken token = NULL;
    CLICmdNode cmd_node = NULL;
    CLIArgNode arg_node = NULL;

    reset_buff(number, 64);

    /* Search 'name' in the input list */
    list = input_list;
    while (list) {
        token = (InputToken)list->data;
        if (!strcmp(token->sid, name)) {
            cmd_node = (CLICmdNode)matrix->cmd->data;
            arg_list = (SList)cmd_node->arg_list;
            /* If 'name' was found in the input list, */
            /* then search it in the graph's arg list */
            while (arg_list) {
                arg_node = (CLIArgNode)arg_list->data;
                /* In a madatory arg this member is NULL */
                if (arg_node->name) {
                    /* The second part of the 'if' makes the difference between the param's name and */
                    /* the param's value. If they're equal we have the param's name (trs_get_param_name()), */
                    /* if they're different we have the param's value */
                    if (!strcmp(arg_node->sid, name) && strcmp(arg_node->name, token->value)) {
                        if (!strcmp(arg_node->value, "range")) {
                            debug_shell_printf("Token value: %s\n", token->value);
                            if (is_number(token->value)) {
                                /* Accept negative numbers */
                                if (*(token->value) == '-')
                                    return atof(token->value + 1) * (-1);
                                else
                                    return atof(token->value);
                            }
                            ptr = token->value;
                            /* Left side */
                            if (!strcmp(side, LEFT_SIDE)) {
                                if (*(ptr) == '-' || (*(ptr + 1) == '-')) {
                                    minus = 1;
                                    ptr += 2;
                                    limit = ptr;
                                }
                                else {
                                    ptr++;
                                    limit = ptr;
                                }
                                while (*ptr != '-')
                                    ptr++;
                                strncpy(number, limit, ptr - limit); 
                                if (minus)
                                    return atoi(number) * (-1);
                                else
                                    return atoi(number);
                            }
                            /* Right side */
                            else {
                                if (*(ptr) == '-' || (*(ptr + 1) == '-')) {
                                    minus = 1;
                                    ptr += 2;
                                }
                                else
                                    ptr++;
                                while (*ptr != '-')
                                    ptr++;
                                limit = ptr + 1;
                                while (*ptr != ']')
                                    ptr++;
                                strncpy(number, limit, ptr - limit);  
                                if (minus)
                                    return atof(number) * (-1);
                                else
                                    return atof(number);
                            }
                       }
                    }
                }
                else {
                    if (!strcmp(arg_node->sid, name)) {
                        if (!strcmp(arg_node->value, "range")) {
                            debug_shell_printf("Token value: %s\n", token->value);
                            if (is_number(token->value)) {
                                /* Accept negative numbers */
                                if (*(token->value) == '-')
                                    return atoi(token->value + 1) * (-1);
                                else
                                    return atoi(token->value);
                            }
                            ptr = token->value;
                            /* Left side */
                            if (!strcmp(side, LEFT_SIDE)) {
                                if (*(ptr) == '-' || (*(ptr + 1) == '-')) {
                                    minus = 1;
                                    ptr += 2;
                                    limit = ptr;
                                }
                                else {
                                    ptr++;
                                    limit = ptr;
                                }
                                while (*ptr != '-')
                                    ptr++;
                                strncpy(number, limit, ptr - limit); 
                                if (minus)
                                    return atoi(number) * (-1);
                                else
                                    return atoi(number);
                            }
                            /* Right side */
                            else {
                                if (*(ptr) == '-' || (*(ptr + 1) == '-')) {
                                    minus = 1;
                                    ptr += 2;
                                }
                                else
                                    ptr++;
                                while (*ptr != '-')
                                    ptr++;
                                limit = ptr + 1;
                                while (*ptr != ']')
                                    ptr++;
                                strncpy(number, limit, ptr - limit);  
                                if (minus)
                                    return atoi(number) * (-1);
                                else
                                    return atoi(number);
                            }
                        }
                    }
                }
                arg_list = arg_list->next;
            } /* while (arg_list) */
        }
        list = list->next;
    } /* while (list) */
    debug_shell_printf("Name '%s' not found!\n", name);
    /* Warning: return a number that is not likely to be used */
    return (1 << 30) * (-1);
}


/*
 * Description: Search in the input list the value of the given 'name'
 *              This func must be called when the param's type is STRING,
                HEX, IPADDR, MACADDR or RANGE
 */
char * trs_get_param_str(SList input_list, const char *name) {
    SList      list = NULL,
               arg_list = NULL;
    InputToken token = NULL;
    CLICmdNode cmd_node = NULL;
    CLIArgNode arg_node = NULL;

    /* Search 'name' in the input list */
    list = input_list;
    while (list) {
        token = (InputToken)list->data;
        if (!strcmp(token->sid, name)) {
            cmd_node = (CLICmdNode)matrix->cmd->data;
            arg_list = (SList)cmd_node->arg_list;
            /* If 'name' was found in the input list, */
            /* then search it in the graph's arg list */
            while (arg_list) {
                arg_node = (CLIArgNode)arg_list->data;
                /* In a madatory arg this member is NULL */
                if (arg_node->name) {
                    /* The second part of the 'if' makes the difference between 
                     * the param's name and the param's value. If they're equal 
                     * we have the param's name (get_param_name()), */
                    /* if they're different we have the param's value */
                    if (!strcmp(arg_node->sid, name) && strcmp(arg_node->name, token->value)) {
                        if ((!strcmp(arg_node->value, "string")) || 
                            (!strcmp(arg_node->value, "range")) ||
                            (!strcmp(arg_node->value, "ipaddr")) || 
                            (!strcmp(arg_node->value, "networkmask")) || 
                            (!strcmp(arg_node->value, "macaddr")) ||
                            (!strcmp(arg_node->value, "hex"))) {
                            debug_shell_printf("Token value: %s\n", token->value);
                            return token->value;
                        }
                        else
                            return NULL;
                    }
                }
                else {
                    if (!strcmp(arg_node->sid, name)) {
                        if ((!strcmp(arg_node->value, "string")) || 
                            (!strcmp(arg_node->value, "range")) ||
                            (!strcmp(arg_node->value, "ipaddr")) || 
                            (!strcmp(arg_node->value, "networkmask")) || 
                            (!strcmp(arg_node->value, "macaddr")) ||
                            (!strcmp(arg_node->value, "hex"))) {
                            debug_shell_printf("Token value: %s\n", token->value);
                            return token->value;
                        }   
                        else
                            return NULL;
                    }
                }
                arg_list = arg_list->next;
            }
        }   
        list = list->next;
    }
    return NULL;
}


/**************************************************
 * Main
 *************************************************/

int main(int argc, char **argv) {

    /* Create graph */
    create_static_graph();

    debug_rhal_printf("Total number of cmds: %d\n", graph_mode_cmd_count(matrix->graph));

    graph_reset_ptrs();

    /* Create the commands and functions tables */
    create_cmd_func_link();

    /* We want the CLI: no args are received (only the program name) */
    if (argc == 1) {
        setup_job_control();

        trs_print_init_msg(SHELL);

        /* Get and validate login/passwd */
#ifdef ENABLE_AUTH
        if (!get_login_passwd()) {
            TRS_ERR("FATAL: Could not get login/passwd");
            exit(EXIT_FAILURE);
        }
#endif /*ENABLE_AUTH */

#ifdef ENABLE_LAST_LOGIN
        {
            char *str = NULL;
            char filename[NAME_SIZE];

            sprintf(filename, "%s/%s", TARASCA_CONF_PATH, TARASCA_CONF_FILE);
            /*sprintf(filename, "%s/%s/%s", TARASCA_PATH, TARASCA_CONF_PATH, TARASCA_CONF_FILE);*/

            if ((str = trs_config_file_param_read(filename, "last_login")) != NULL) {
                printf("%s%s\n", LAST_LOGIN_MSG, str);
                xfree(str);
            }
        }
#endif /*ENABLE_LAST_LOGIN */
        /* Start getting commands from the command-line */
        if (!get_line(FD_IN)) {
	        TRS_ERR("FATAL: Could not get command line");
	        exit(EXIT_FAILURE);
        }
    }
    /* We want a GUI: Two or more args are received */
#ifdef ENABLE_GUI
    if (argc > 1) {
        if (!get_line_from_gui(argc, argv)) {
            TRS_ERR("FATAL: Could not execute command");
	        exit(EXIT_FAILURE);
        }
    }
#endif /*ENABLE_GUI */

    return 1;
}

