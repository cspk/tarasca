/*
 * misc.c --
 *
 * Author -- Pedro Aguilar <paguilar@junkerhq.net>
 *
 * Copyright (c) 2005 - 2006 Pedro Aguilar
 *
 * Description: Contains all the utility functions used around the
 *     project: Lists, dynamic memory allocation, logs, read/write
 *     config files, data type convertion/checking
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

#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#include "config.h"
#include "misc.h"

/*****************************
 * FUNCTION IMPLEMENTATION
 *****************************/

/*
 * Description: Allocates num blocks of memory of a given size.
 *              The new allocated memory is cleared.
 */
void * xmalloc(size_t size, int num) {
    void	*ptr = NULL;

    if (size == 0 || num < 1) {
	TRS_ERR("FATAL: Invalid size or number of elements!");
	exit(EXIT_FAILURE);
    }

    if ((ptr = malloc(size * num)) == (void *)NULL) {
	TRS_ERR("FATAL: Could not allocate memory!");
	exit(EXIT_FAILURE);
    }

    memset(ptr, '0', size);
    return ptr;
}


/*
 * Description: Changes the size of the memory block pointed to by ptr 
 *              to size bytes. The new reallocated memory is cleared.
 */
void * xrealloc(void *ptr, size_t size) {
    void	*new_ptr = NULL;

    if (size == 0)
        return NULL;

    if (ptr == NULL)
        return NULL;

    if ((new_ptr = realloc(ptr, size)) == (void *)NULL) {
	TRS_ERR("FATAL: Could not allocate memory!");
	return NULL;
    }

    memset(new_ptr, '0', size);
    return ptr;
}


/*
 * Description: Free the allocated memory 
 */
void xfree(void *ptr) {
    if (ptr != NULL)
        free(ptr);
}


char * xstrdup(const char *s) {
    char *t;

    if (s == NULL)
        return NULL;

    t = strdup (s);

    if (t == NULL)
        return NULL;
/*         bb_error_msg_and_die(bb_msg_memory_exhausted); */

    return t;
}


/*
 * Description: Prints the log msgs to a log file using a specific format.
 *     Eg. Tue Nov  9 11:50:13 2004 : func(): line n: Error msg
 */
void log_error(const char *func, int line, char *fmt, ...) {
    char     msg_buff[LOG_BUFF + 128],
             va_buff[LOG_BUFF];
    va_list  ap;
#ifdef ENABLE_LOGS
    FILE     *fd;
    DIR      *dir;
    char     name[NAME_SIZE];
    time_t   timeval;
#if 0
    unsigned short logdir_exists = 0;
    struct dirent *ep;
#endif

    /* Insert time to buffer */
    timeval = time(0);
    sprintf(msg_buff, "%s", ctime(&timeval));
    msg_buff[strlen(msg_buff) - 1] = '\0';
    msg_buff[strlen(msg_buff)] = '\0';

    /* Open the Tarasca logs dir */
    if ((dir = opendir(TARASCA_LOG_DIR)) == NULL) {
        sprintf(name, "%.200s", TARASCA_LOG_DIR);
        /* If it doesn't exist, create it with 755 permissions */
        if (mkdir(name, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) < 0) {
            fprintf(stderr, "%s: Tarasca: %s", msg_buff, strerror(errno));
            return;
        }
    }
    closedir(dir);

    sprintf(name, "%.200s/%.50s", TARASCA_LOG_DIR, TARASCA_LOG_FILE);
    if ((fd = fopen(name, "a")) == NULL) {
        fprintf(stderr, "%s: Tarasca: %s", msg_buff, strerror(errno));
        return;
    }
    /*fprintf(fd, "%s", msg_buff);*/

 
#if 0
    if ((dir = opendir(TARASCA_LOG_DIR)) == NULL) {
        fprintf(stderr, "%s: Tarasca: %s", strerror(errno));
    }

    /* If the logs file is found, open it */
    while ((ep = readdir(dir)) != NULL) {
        sprintf(name, "%.100s", TARASCA_LOG_DIR);
        if (!strncmp(name, ep->d_name, strlen(name))) {
            logdir_exists |= 1;
            sprintf(name, "%.100s/%.100s/%.50s", 
                TARASCA_PATH, TARASCA_LOG_DIR, TARASCA_LOG_FILE);
            if ((fd = fopen(name, "a")) == NULL) {
                fprintf(stderr, "%s: Tarasca: %s", msg_buff, strerror(errno));
                closedir(dir);
                return;
            }
            break;
        }
    }
    closedir(dir);

    /* Otherwise, create it and open it */
    if (!logdir_exists) {
        sprintf(name, "%.100s/%.100s", TARASCA_PATH, TARASCA_LOG_DIR);
        /* Create dir with 755 permissions */
        if (mkdir(name, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) < 0) {
            fprintf(stderr, "%s: Tarasca: %s", msg_buff, strerror(errno));
            return;
        }

        sprintf(name, "%.100s/%.100s/%.50s", 
            TARASCA_PATH, TARASCA_LOG_DIR, TARASCA_LOG_FILE);
        if ((fd = fopen(name, "a")) == NULL) {
            fprintf(stderr, "%s: Tarasca: %s", msg_buff, strerror(errno));
            return;
        }
        fprintf(fd, "%s", msg_buff);
    }
#endif

    sprintf(msg_buff, "%s: %s(): line %d: ", msg_buff, func, line);
#endif /* ENABLE_LOGS */

    /* Insert msg to buffer */
    if (fmt) {
        va_start(ap, fmt);
        vsnprintf(va_buff, sizeof(va_buff), fmt, ap);
        va_end(ap);
        sprintf(msg_buff, "%s%s\n", msg_buff, va_buff);
        fprintf(FD_OUT, "%s\n", va_buff);
    }
    else {
        fprintf(FD_OUT, "Unespecified error ocurred\n");
        sprintf(msg_buff, "%s: Unespecified error ocurred\n", msg_buff);
    }

#ifdef ENABLE_LOGS
    fprintf(fd, "%s", msg_buff);
    fclose(fd);
#else
    fflush(FD_OUT);
#endif /* ENABLE_LOGS */
}


#if 0
int log_error(int level, char *fmt, ...) {
#ifndef ENABLE_LOGS
    return 1;
#endif /*ENABLE_LOGS */

    FILE                *msgfd;
    va_list		ap;
    char		msg_buff[LOG_BUFF * 2],
    			va_buff[LOG_BUFF];
    time_t              timeval;
    int			n;

    reset_buff(msg_buff, LOG_BUFF * 2);
    reset_buff(va_buff, LOG_BUFF);

    /* Open file or stdout */
    if (strcmp(TARASCA_LOG_FILE, "stdout") != 0) {
        sprintf(msg_buff, "%.200s/%.50s", TARASCA_LOG_DIR, TARASCA_LOG_FILE);
        if ((msgfd = fopen(msg_buff, "a")) == NULL) {
            fprintf(stderr, "WARNING: Couldn't open %s for logging\n", msg_buff);
            return -1;
        }
    }
    else
        msgfd = FD_OUT;

    /* Insert time to buffer */
    timeval = time(0);
    sprintf(msg_buff, "%s", ctime(&timeval));
    for (n = strlen(msg_buff) - 1; n < LOG_BUFF * 2; n++) {
	if (msg_buff[n] == '\n')
	    msg_buff[n] = '\0';
    }

    /* Insert error level to buffer */
    switch(level) {
        case LOG_ERROR:
	    strcat(msg_buff, ": error: ");
            break;
        case LOG_WARN:
	    strcat(msg_buff, ": warn: ");
            break;
        case LOG_COMM:
	    strcat(msg_buff, ": comm: ");
            break;
        case LOG_GRAPH:
	    strcat(msg_buff, ": graph: ");
            break;
        case LOG_RHAL:
	    strcat(msg_buff, ": rhal: ");
            break;
        case LOG_PARSER:
	    strcat(msg_buff, ": parser: ");
            break;
        case LOG_SHELL:
	    strcat(msg_buff, ": shell: ");
            break;
        case LOG_WWW:
	    strcat(msg_buff, ": www: ");
            break;
        case LOG_MALLOC:
	    strcat(msg_buff, ": xmalloc: ");
            break;
	default:
	    strcat(msg_buff, ": ");
	    break;
    }

    /* Insert msg to buffer */
    if (fmt) {
        va_start(ap, fmt);
        vsprintf(va_buff, fmt, ap);
        va_end(ap);
        sprintf(msg_buff, "%s%s\n", msg_buff, va_buff);
    }
    else
        sprintf(msg_buff, "%s\n", msg_buff);

    fputs(msg_buff, msgfd);

    fprintf(FD_OUT, "%s", msg_buff);

    if (msgfd != FD_OUT)
        fclose(msgfd);
    else
        fflush(FD_OUT);

    return 1;
}
#endif


ssize_t safe_read(int fd, void *buf, size_t count) {
    ssize_t n;

    do {
        n = read(fd, buf, count);
    } while (n < 0 && errno == EINTR);

    return n;
}


/*
 * Description: Read a line from the configuration file that contains the
 *              filenames that define the commands
 * TODO: Change this function. It must return a char *
 */
int get_line_cmd_file(FILE *fd, char *stream) {
    int	 ch;
    char *ptr = NULL;
    int	 cnt = 0;

    ptr = stream;
    ch = fgetc(fd);

    do {
	if (ch == 32 || ch == 9)     /* Spaces and tabs */
	    continue;
	else if (ch == 10 && !cnt)   /* Enter and buffer empty */
	    continue;
	else if (ch == 10 && cnt) {  /* Enter and buffer not empty */
	    return 1;
	}
	/* [0-9A-Za-z], '.' (point), '-' (dash), '_' (underscore) */
	else if (isalnum(ch) || (ch == '.') || (ch == '-') || (ch == '_')) {
            /* TODO: Check buffer size */
	    *ptr = ch;
	    ptr++;
	    if (cnt > NAME_SIZE)
		return -2;
	    cnt++;
	}
	else if (ch == '#') {		/* '#', comments */
	    while ((ch = fgetc(fd)) != 10)
		continue;
            if (ch == EOF  && cnt)
            	return -1;
            if (cnt)
            	return 1;

	}
	else {				/* Any other char */
	    return 0;
	}
    } while ((ch = fgetc(fd)) != EOF);

    return -1;
}


/*
 * Description: Read the value of the given key inside the
 *     given filename
 */
char * trs_config_file_param_read(char *filename, char *key) {
    int   ch,
          have_key = 1,
          have_comment = 0,
          ch_cnt = 0,
          line_cnt = 1;
    char  *key_name = NULL,
          *value = NULL;
    FILE  *fd = NULL;
    List  key_list = NULL,
          value_list = NULL;

    if (!filename || !key)
        return NULL;

    if ((fd = fopen(filename, "r")) == NULL) {
        TRS_ERR("%s", strerror(errno));
        return NULL;
    }
    
    ch = fgetc(fd);

    do {
        /*printf("Read: %c\n", ch); */
        if (ch_cnt == NAME_SIZE) {
            TRS_ERR("%s:%d: parse error: maximum string size of %d reached\n", 
                filename, line_cnt, NAME_SIZE);
            fclose(fd); return NULL;
        }

        /* Get key */
        if (have_key) {
            if (ch != '=') {
                /* white space chars and empty list */
                if (isspace(ch)) { 
                    if (ch == 10 && !list_empty(key_list)) {
                        TRS_ERR("%s:%d: parse error: invalid new line\n", filename, line_cnt);
                        fclose(fd); return NULL;
                    }
                    else if (ch == 10)
                        line_cnt++;
                    continue;
                }
                /* [0-9A-Za-z], '-', '_' */
                else if (isalnum(ch) || (ch == '-') || (ch == '_')) {
                    key_list = list_append(key_list, ch);
                    ch_cnt++;
                }
                /* ch is '#', commments */
                else if (ch == '#') {
                    while ((ch = fgetc(fd)) != 10)
                        continue;
                    line_cnt++;
                }

                else {
                    TRS_ERR("%s:%d: parse error: invalid character\n", filename, line_cnt);
                    fclose(fd); return NULL;
                }
            }
            else {
                /* list not empty */
                if (!list_empty(key_list)) {
                    have_key = 0;
                    ch_cnt = 0;
                }
                else {
                    TRS_ERR("%s:%d: parse error: key not found\n", filename, line_cnt);
                    fclose(fd); return NULL;
                }
            }
        }
        /* Get value */
        else {
            if (ch != 10) {
                /* sp and tabs */
                if ((ch == 32 || ch == 9))
                    continue;

                else if (ch == '"') {
                    while ((ch = fgetc(fd)) != '"') {
                        /*printf("Read: %c\n", ch); */
                        value_list = list_append(value_list, ch);
                        ch_cnt++;
                    }

                    if (ch == EOF) {
                        TRS_ERR("%s:%d: parse error: EOF reached\n", 
                            filename, line_cnt);
                        fclose(fd); return NULL;
                    }
                }
                /* [0-9A-Za-z], '_' */
                else if (isalnum(ch) || (ch == '_')) {
                    value_list = list_append(value_list, ch);
                    ch_cnt++;
                }
                /* Comments */
                else if (ch == '#') {
                    have_comment = 1;
                    if (list_empty(value_list)) {
                        TRS_ERR("%s:%d: parse error: value not found\n", 
                            filename, line_cnt);
                        fclose(fd); return NULL;
                    }
                    while ((ch = fgetc(fd)) != 10)
                        continue;
                    line_cnt++;
                }

                else {
                    TRS_ERR("%s:%d: parse error: invalid character\n", 
                        filename, line_cnt);
                    fclose(fd); return NULL;
                }
            }

            if ((ch == 10) || (ch != 10 && have_comment)) {
                line_cnt++;

                if (list_empty(key_list) || list_empty(value_list)) {
                    TRS_ERR("%s:%d: parse error: key/value not found\n", 
                        filename, line_cnt);
                    fclose(fd); return NULL;
                }

                /* Convert the linked lists to strings */
                if ((key_name = list_2buff(key_list)) == NULL) {
                    TRS_ERR("%s:%d: Could not allocate memory\n", 
                        filename, line_cnt);
                    fclose(fd); return NULL;
                }

                if ((value = list_2buff(value_list)) == NULL) {
                    TRS_ERR("%s:%d: Could not allocate memory\n", 
                        filename, line_cnt);
                    fclose(fd); xfree(key_name); return NULL;
                }

                /* Check if we have spaces/tabs  */
                if (!check_space(key_name) || !check_space(value)) {
                    if (!have_double_quotes(value)) {
                        TRS_ERR("%s:%d: Invalid key/value\n", 
                            filename, line_cnt);
                        fclose(fd); xfree(key_name); xfree(value); return NULL;
                    }
                }

                /*printf("key_name: %s, value: %s\n", key_name, value); */
                if (!strcmp(key, key_name)) {
                    xfree(key_name);
                    key_list = list_new();
                    value_list = list_new();
                    fclose(fd);
                    return value;
                }

                ch_cnt = 0;
                have_key = 1;
                have_comment = 0;
                xfree(key_name);
                xfree(value);
                key_list = list_new();
                value_list = list_new();
            }
        }

    } while ((ch = fgetc(fd)) != EOF);

    fclose(fd);
    return NULL;
}


/*
 *
 */
int trs_config_file_param_write(char *filename, char *key, char *value) {
    int   ch,
          have_key = 1,
          have_comment = 0,
          ch_cnt = 0,
          line_cnt = 1;
    char  name[NAME_SIZE],
          backup[NAME_SIZE],
          *key_name = NULL,
          *value_name = NULL;
    FILE  *src = NULL,
          *dst = NULL;
    List  key_list = NULL,
          value_list = NULL;

    if (!filename || !key || !value)
        return 0;

    if ((src = fopen(filename, "r")) == NULL) {
        TRS_ERR("%s", strerror(errno));
        return 0;
    }

    sprintf(backup, "%s.tmp", filename);
    if ((dst = fopen(backup, "w+")) == NULL) {
        TRS_ERR("fopen(): %s", strerror(errno));
        fclose(src); return 0;
    }
    fprintf(dst, "#\n# Configuration file\n# %s\n#\n\n", filename);

    ch = fgetc(src);

    do {
        /*printf("Read: %c\n", ch); */
        if (ch_cnt == NAME_SIZE + 1) {
            TRS_ERR("%s:%d: parse error: maximum string size of %d reached\n", 
                filename, line_cnt, NAME_SIZE);
            fclose(src); return 0;
        }

        /* Get key */
        if (have_key) {
            if (ch != '=') {
                /* white space chars and empty list */
                if (isspace(ch)) { 
                    if (ch == 10 && !list_empty(key_list)) {
                        TRS_ERR("%s:%d: parse error: invalid new line\n", 
                            filename, line_cnt);
                        fclose(src); fclose(dst); return 0;
                    }
                    else if (ch == 10)
                        line_cnt++;
                    continue;
                }
                /* [0-9A-Za-z], '-', '_' */
                else if (isalnum(ch) || (ch == '-') || (ch == '_')) {
                    key_list = list_append(key_list, ch);
                    ch_cnt++;
                }
                /* ch is '#', commments */
                else if (ch == '#') {
                    while ((ch = fgetc(src)) != 10)
                        continue;
                    line_cnt++;
                }

                else {
                    TRS_ERR("%s:%d: parse error: invalid character\n", 
                        filename, line_cnt);
                    fclose(src); fclose(dst); return 0;
                }
            }
            else {
                /* list not empty */
                if (!list_empty(key_list)) {
                    have_key = 0;
                    ch_cnt = 0;
                }
                else {
                    TRS_ERR("%s:%d: parse error: key not found\n", 
                        filename, line_cnt);
                    fclose(src); fclose(dst); return 0;
                }
            }
        }
        /* Put value */
        else {
            if (ch != 10) {
                /* sp and tabs */
                if ((ch == 32 || ch == 9))
                    continue;

                else if (ch == '"') {
                    value_list = list_append(value_list, ch);
                    ch_cnt++;

                    while ((ch = fgetc(src)) != '"') {
                        /*printf("Read: %c\n", ch); */
                        value_list = list_append(value_list, ch);
                        ch_cnt++;
                    }

                    value_list = list_append(value_list, ch);
                    ch_cnt++;

                    if (ch == EOF) {
                        TRS_ERR("%s:%d: parse error: EOF reached\n", 
                            filename, line_cnt);
                        fclose(src); fclose(dst); return 0;
                    }
                }
                /* [0-9A-Za-z], '_' */
                else if (isalnum(ch) || (ch == '_') || (ch == '.')) {
                    value_list = list_append(value_list, ch);
                    ch_cnt++;
                }
                /* Comments */
                else if (ch == '#') {
                    have_comment = 1;
                    if (list_empty(value_list)) {
                        TRS_ERR("%s:%d: parse error: value not found\n", 
                            filename, line_cnt);
                        fclose(src); fclose(dst); return 0;
                    }
                    while ((ch = fgetc(src)) != 10)
                        continue;
                    line_cnt++;
                }

                else {
                    TRS_ERR("%s:%d: parse error: invalid character\n", 
                            filename, line_cnt);
                    fclose(src); fclose(dst); return 0;
                }
            }

            if ((ch == 10) || (ch != 10 && have_comment)) {
                line_cnt++;

                if (list_empty(key_list) || list_empty(value_list)) {
                    TRS_ERR("%s:%d: parse error: key/value not found\n", 
                        filename, line_cnt);
                    fclose(src); fclose(dst); return 0;
                }

                /* Convert the linked lists to strings */
                if ((key_name = list_2buff(key_list)) == NULL) {
                    TRS_ERR("%s:%d: Could not allocate memory\n", 
                        filename, line_cnt);
                    fclose(src); fclose(dst); return 0;
                }

                if ((value_name = list_2buff(value_list)) == NULL) {
                    TRS_ERR("%s:%d: Could not allocate memory\n", 
                        filename, line_cnt);
                    fclose(src); fclose(dst); xfree(key_name); return 0;
                }

                /* Check if we have spaces/tabs  */
                if (!check_space(key_name) || !check_space(value_name)) {
                    if (!have_double_quotes(value_name)) {
                        TRS_ERR("%s:%d: Invalid key/value\n", filename, line_cnt);
                        fclose(src); fclose(dst); xfree(key_name); xfree(value_name); return 0;
                    }
                }

                if (strcmp(key, key_name))
                    fprintf(dst, "%s = %s\n", key_name, value_name);
                else
                    fprintf(dst, "%s = %s\n", key_name, value);

                ch_cnt = 0;
                have_key = 1;
                have_comment = 0;
                xfree(key_name);
                xfree(value_name);
                key_list = list_new();
                value_list = list_new();
            }
        }

    } while ((ch = fgetc(src)) != EOF);

    fputs("\n", dst);
    fclose(dst);
    fclose(src);

    sprintf(name, "%s.bak", filename);
    if (rename(filename, name) == -1) {
        TRS_ERR("%s", strerror(errno));
        return 0;
    }

    if (rename(backup, filename) == -1) {
        TRS_ERR("%s", strerror(errno));
        return 0;
    }

    return 1;
}


/*
 * Description: Resets a buffer of a given size
 */
void reset_buff(char *buff, int size) {
    int i;

    for (i = 0; i < size; i++)
	buff[i] = '\0';
}


/*
 * Description: Convert an integer to a string.
 *              The allocated memory should be freed later 
 *              (perhaps the function that call int2str())
 */
char * int2str(int num) {
    int    i,
           cnt = 0,
           minus = 0,
           tmp;
    char   *str;

    if (!num) {
        if ((str = (char *)xmalloc(sizeof(char), 2)) == NULL)
            return NULL;
        *str = 48;
        *(str + 1) = '\0';
        return str;
    }

    if (num < 0) {
        minus = 1;
        num = num * (-1);
    }

    tmp = num;
    while (tmp) {
        tmp = tmp / 10;
        cnt++;
    }

    if (minus)
        cnt++;

    if ((str = (char *)xmalloc(sizeof(char), cnt + 1)) == NULL)
        return NULL;

    reset_buff(str, strlen(str));

    if (minus)
        cnt--;

    str += cnt - 1; 
    for (i = cnt; i > 0; i--) {
         *str = (num % 10) + 48;
         num = num / 10;
         str--;
    }
    if (minus)
        *str = '-';
    else
        str++;

    return str;
}


/*
 * Description: Convert a 'normal' string to 
 *              a mac address string
 */
int str2mac(unsigned char *mac_addr, char *str) {
    int  i;
    char high,
         low;
    char *ptr = str;

    memset(mac_addr, 0, 6);

    for (i = 0; i < 6; i++) {
        high = *ptr;
        low = *(ptr + 1);

        if (high >= 48 && high <= 57)
            high -= 48;
        else if (high >= 97 && high <= 102)
            high -= 87;
        else if (high >= 65 && high <= 70)
            high -= 55;
        else
            return 0;

        if (low >= 48 && low <= 57)
            low -= 48;
        else if (low >= 97 && low <= 102)
            low -= 87;
        else if (low >= 65 && low <= 70)
/*        else if (low >= 65 && high <= low) */
            low -= 55;
        else
            return 0;

        mac_addr[i] |= high;
        mac_addr[i] = mac_addr[i] << 4;
        mac_addr[i] |= low;

        ptr = ptr + 3;
    }

    return 1;
}


/*
 *
 */
short int check_space(char *buff) {
    int i,
        len,
        have_space = 0;

    len = strlen(buff);

    for (i = 0; i < len; i++) {
        if (isspace(buff[i]))
            have_space = 1;
        else if (isalnum(buff[i] && have_space))
            return 0;
    }
    return 1;
}


/*
 *
 */
short int have_double_quotes(char *buff) {
    char *start = NULL,
         *end = NULL;

    start = buff;
    end = buff + sizeof(buff);

    if (*start == '"' && *end == '"')
        return 1;

    return 0;
}


/**************************************************
 * Simple linked list of chars
 *************************************************/

/*
 *
 */
List list_new() {
    return (List)NULL;
}


/*
 * Description: Tells if a list is empty
 */
int list_empty(List node) {
    return node == NULL ? 1 : 0;
}
    
    
/*  
 * Description: Adds a new element to the end of the List
 */ 
List list_append(List node, char ch) {
    List ptr = NULL,
         aux = NULL; 
    
    if ((ptr = (List)xmalloc(sizeof(struct List_st), 1)) == NULL) {
        return NULL; 
    }   
        
    ptr->ch = ch;
    ptr->next = NULL;
    
    aux = node;
    if (!aux)
        return ptr;

    while (aux->next) {
        aux = aux->next;
    }
    aux->next = ptr;
        
    return node;
}
    

/*  
 * Description: Count the number of elements in the list
 */         
int list_count(List node) {
    int  counter = 0;
    List ptr = NULL;
    
    ptr = node;
    while (ptr) {
        counter++;
        ptr = ptr->next;
    }

    return counter;
}


/*
 * Description: Convert a list of chars to a string of the size of the length of
 *              the list plus 1 for the end of string.
 */
char * list_2buff(List node) {
    int  i = 0;
    char *buff;
    List list_ptr;

    if ((buff = (char *)xmalloc(sizeof(char), list_count(node) + 1)) == NULL)
        return (char *)NULL;

    reset_buff(buff, list_count(node) + 1);
    list_ptr = node;

    while (list_ptr) {
        buff[i++] = list_ptr->ch;
        list_ptr = list_ptr->next;
    }

    return buff;
}


/**************************************************
 * Data type checking funcs
 *************************************************/

/* 
 * Description: Checks if the given string has a bool value.
 *              Accepts 0, 1, enable and disable (case insensitive)
 */
short int is_bool(char *str) {
    /*unsigned char *ch; */
    char *ch;

    if (!strcasecmp(str, ENABLE_STR) || !strcasecmp(str, DISABLE_STR))
        return 1;

    ch = str;
    if (strlen(ch) != 1)
        return 0;

    if (*ch != '0' && *ch != '1')
        return 0;

    return 1;
}


/* 
 * Description: Checks if the given string is an integer or decimal
 *              Accepts [+-]\d+\.{0,1}\d*
 */
short int is_number(char *str) {
    char      *ch;
    int       len;
    short int have_dot = 0;

    ch = str;
    len = strlen(str);
    while (len) {
        /* First char can be a minus sign, an addition sign or a digit */
        if (len == strlen(str)) {
            if (*ch != '-' && *ch != '+' && *ch != '.' && !isdigit(*ch))
                return 0;
            if (*ch == '.' && len == 1)
                return 0;
            if (*ch == '.')
                have_dot = 1;
            if (*ch == '-' && len == 1)
                return 0;
        }
        else {
            if (*ch != '.' && !isdigit(*ch))
                return 0;
            if (have_dot && *ch == '.')
                return 0;
            if (!have_dot && *ch == '.')
                have_dot = 1;
        }
        ch++;
        len--;
    }
    return 1;
}


/* 
 * Description: Checks if the given string has the correct syntax
 *              Accepts [0-9aA-zZ_-]+
 */
short int is_string(char *str) {
    char *ch;
    int           len;

    ch = str;
    len = strlen(str);
    while (len) {
        if (*ch != '-' && *ch != '_' && !isalnum(*ch))
            return 0;
        ch++;
        len--;
    }
    return 1;
}


/* 
 * Description: Checks if the given string is an hex number
 *              Accepts 0x\h+ or 0X\h+ or \h+
 */
short int is_hex(char *str) {
    char *ch;
    int  len;

    ch = str;
    len = strlen(str);
    while (len) {
        if (len == (strlen(str) - 1)) {
            if (*ch == 'x' || *ch == 'X') {
                if (*(--ch) != '0')
                    return 0;
                ch++;
            }
            else {
                if (!isxdigit(*ch))
                    return 0;
            }
        }
        else {
            if (!isxdigit(*ch))
                return 0;
        }
        ch++;
        len--;
    }
    return 1;
}


/* 
 * Description: Checks if the given string is a valid IP address
 *              Accepts the dotted decimal and hex notations
 */
short int is_ipaddr(char *str) {
    char *ch,
         class[4];
    int  len,
         have_dot = 3,
         num = 0;

    len = strlen(str);
    if (len < 7 || len > 15)
        return 0;

    // The IP addr is in hex format?
    if (strstr(str, "X") || strstr(str, "x")) {
        if (is_hex(str))
            return 1;
        return 0;
    }

    reset_buff(class, sizeof(class));

    ch = str;
    while (len) {
        // Error if ch is diff than digit and '.'
        if (*ch != '.' && !isdigit(*ch))
            return 0;

        // Error if first ch is '.'
        if (len == strlen(str))
            if (*ch == '.')
                return 0;

        // Error if there are two '.' together
        if (*ch == '.' && *(ch - 1) == '.')
            return 0;

        // We got a digit
        if (isdigit(*ch)) {
            class[num] = *ch;
            num++;
        }
        // We got a '.'
        else {
            if (atoi(class) > 254)
                return 0;
            reset_buff(class, sizeof(class));
            num = 0;
            have_dot--;
        }

        if (num > 3 || have_dot < 0)
            return 0;

        ch++;
        len--;
    }

    if (have_dot)
        return 0;

    // Check host part
    num = atoi(class);
    if (num < 1 || num > 254)
        return 0;

    return 1;
}


/* 
 * Description: Checks if the given string is a valid IP netmask
 *              Accepts the dotted decimal and hex notations
 *              TODO: Accepts the /24 format
 */
short int is_netmask(char *str) {
    char *ch,
         class[4];
    int  len,
         have_dot = 3,
         num = 0;

    len = strlen(str);
    if (len < 7 || len > 15)
        return 0;

    // The netmask is in hex format?
    if (strstr(str, "X") || strstr(str, "x")) {
        if (is_hex(str))
            return 1;
        return 0;
    }

    reset_buff(class, sizeof(class));

    ch = str;
    while (len) {
        // Error if ch is diff than digit and '.'
        if (*ch != '.' && !isdigit(*ch))
            return 0;

        // Error if first ch is '.'
        if (len == strlen(str))
            if (*ch == '.')
                return 0;

        // Error if there are two '.' together
        if (*ch == '.' && *(ch - 1) == '.')
            return 0;

        // We got a digit
        if (isdigit(*ch)) {
            class[num] = *ch;
            num++;
        }
        // We got a '.'
        else {
            if (atoi(class) > 255)
                return 0;
            reset_buff(class, sizeof(class));
            num = 0;
            have_dot--;
        }

        if (num > 3 || have_dot < 0)
            return 0;

        ch++;
        len--;
    }

    if (have_dot)
        return 0;

    if ((strlen(class) < 1) || (atoi(class) > 255))
        return 0;

    return 1;
}


/* 
 * Description: Checks if the given string is a valid MAC address
 *              Accepts the 00:11:22:33:44:55 and hex notations
 *              TODO: accept other formats
 */
short int is_macaddr(char *str) {
    char *ch;
    int  len;

    if (is_hex(str))
        return 1;

    len = strlen(str);
    if (len != 17)
        return 0;

    ch = str;
    while (len) {
        if (len % 3) {
            if (!isxdigit(*ch))
                return 0;
        }
        else {
            if (*ch != ':')
                return 0;
        }
        ch++;
        len--;
    }

    return 1;
}


/* 
 * Description: Checks if the given string is a range
 *              Accepts [m-n] -> a range from m to n, m < n
 *                      [-m-n] -> a range from -m to n
 *                      -[m-n] -> a range from -m to -n, -m < -n
 *              where m and n are integer or decimal
 */
short int is_range(char *str) {
    char *ch,
         *m,
         *n;
    int  len,
         negative_range = 0,
         negative_value = 0,
         start_p,
         end_p;

    /* Could be just a number */
    if (is_number(str))
        return 1;

    len = strlen(str);
    if (len < 5)
        return 0;

    ch = str;
    /* We must start with '[' or '-' */
    if (*ch != '[' && *ch != '-')
        return 0;

    /* We must finish with ']' */
    if (*(ch + len - 1) != ']')
        return 0;

    /* Do we start with '[\d' or '[\.' */
    if (*ch == '[' && (isdigit(*(ch + 1)) || *(ch + 1) == '.')) {
        ch++;
        len--;
    }
    /* or with '[-' or '-[' */
    else if ((*ch == '[' && *(ch + 1) == '-') || (*ch == '-' && *(ch + 1) == '[')) {
        if (*ch == '-')
            negative_range = 1;
        if (*(ch + 1) == '-')
            negative_value = 1;
        ch = ch + 2;
        len = len - 2;
    }
    else
        return 0;

    /* Get first limit m */
    start_p = len;
    while (*ch != '-') {
        if (!len)
            return 0;
        if (!isdigit(*ch) && *ch != '.')
            return 0;
        ch++;
        len--;
    }
    end_p = len;

    /* Empty m? */
    if (start_p == len)
        return 0;

    if ((m = (char *)xmalloc(sizeof(char), start_p - end_p + 1)) == NULL) {
         fprintf(FD_OUT, "Not enough memory!!!\n");
         return 0;
    }
    reset_buff(m, start_p - end_p + 1);
    strncpy(m, str + strlen(str) - start_p, start_p - end_p);
    if (!is_number(m)) {
        free(m);
        return 0;
    }

    /* Get last limit n */
    ch++;
    len--;
    start_p = len;
    while (*ch != ']') {
        if (!len) {
            free(m);
            return 0;
        }
        if (!isdigit(*ch) && *ch != '.') {
            free(m);
            return 0;
        }
        ch++;
        len--;
    }
    end_p = len;

    /* Empty n? */
    if (start_p == len)
        return 0;

    if ((n = (char *)xmalloc(sizeof(char), start_p - end_p + 1)) == NULL) {
         fprintf(FD_OUT, "Not enough memory!!!\n");
         free(m);
         return 0;
    }
    reset_buff(n, start_p - end_p + 1);
    strncpy(n, str + strlen(str) - start_p, start_p - end_p);
    if (!is_number(n)) {
        free(n); free(m);
        return 0;
    }

    /* Extra invalid chars? */
    if (len > 1) {
        free(m); free(n);
        return 0;
    }

    /* Compare m and n */
    debug_shell_printf("Range: left limit = %lf, right limit = %lf\n", atof(m), atof(n));
    if (negative_range) {
        if (atof(m) < atof(n)) {
            free(m); free(n);
            return 0;
        }
    }

/*
    if (negative_value) {
        if (atof(m) > atof(n)) {
            free(m); free(n);
            return 0;
        }
    }
*/

    if (!negative_range && !negative_value) {
        if (atof(m) > atof(n)) {
            free(m); free(n);
            return 0;
        }
    }

    free(m); free(n);
    return 1;
}


/**************************************************
 * Debug printing funcs
 *************************************************/

#ifdef DEBUG_SHELL
    void debug_shell_printf(const char *format, ...) {
        va_list args;
        va_start(args, format);
        vfprintf(stderr, format, args);
        va_end(args);
    }
#else
    void debug_shell_printf(const char *format, ...) { }
#endif    /* DEBUG_SHELL */


#ifdef DEBUG_RHAL
    void debug_rhal_printf(const char *format, ...) {
        va_list args;
        va_start(args, format);
        vfprintf(stderr, format, args);
        va_end(args);
    }
#else
    void debug_rhal_printf(const char *format, ...) { }
#endif    /* DEBUG_RHAL */


#ifdef DEBUG_PRINTF
    void debug_printf(const char *format, ...) {
        va_list args;
        va_start(args, format);
        vfprintf(stderr, format, args);
        va_end(args);
    }
#else
    void debug_printf(const char *format, ...) { }
#endif    /* DEBUG_PRINTF */


