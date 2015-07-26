/* vi: set sw=4 ts=4: */
/*
 * Copyright (c) 2004 - 2006 Pedro Aguilar
 *
 * Adapted from the 'Termios command line History and Editing'
 * written by Vladimir Oleynik <dzo@simtreas.ru>
 *
 * Copyright (c) 1986-2003 may safely be consumed by a BSD or GPL license.
 *
 * Used ideas:
 *      Adam Rogoyski    <rogoyski@cs.utexas.edu>
 *      Dave Cinege      <dcinege@psychosis.com>
 *      Jakub Jelinek (c) 1995
 *      Erik Andersen    <andersen@codepoet.org> (Majorly adjusted for busybox)
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
   Usage and Known bugs:
   Terminal key codes are not extensive, and more will probably
   need to be added. This version was created on Debian GNU/Linux 2.x.
   Delete, Backspace, Home, End, and the arrow keys were tested
   to work in an Xterm and console. Ctrl-A also works as Home.
   Ctrl-E also works as End.

   Small bugs (simple effect):
   - not true viewing if terminal size (x*y symbols) less
     size (prompt + editor`s line + 2 symbols)
   - not true viewing if length prompt less terminal width
 */

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <signal.h>
#include <limits.h>

#include "config.h"
#include "cmdedit.h"
#include "crish.h"
#include "rhal.h"
#include "print.h"
#include "misc.h"


#ifdef CONFIG_LOCALE_SUPPORT
#define Isprint(c) isprint((c))
#else
#define Isprint(c) ( (c) >= ' ' && (c) != ((unsigned char)'\233') )
#endif

#define CONFIG_FEATURE_COMMAND_EDITING
#define CONFIG_FEATURE_COMMAND_TAB_COMPLETION
#define CONFIG_FEATURE_NONPRINTABLE_INVERSE_PUT
#define CONFIG_FEATURE_CLEAN_UP

#ifdef CONFIG_FEATURE_COMMAND_TAB_COMPLETION
#include <dirent.h>
#include <sys/stat.h>
#endif


#ifdef CONFIG_FEATURE_COMMAND_EDITING

extern CLIMatrix matrix;

/* Maximum length of the linked list for the command line history */
#ifndef CONFIG_FEATURE_COMMAND_HISTORY
#define MAX_HISTORY   15
#else
#define MAX_HISTORY   CONFIG_FEATURE_COMMAND_HISTORY
#endif

#if MAX_HISTORY < 1
#warning cmdedit: You set MAX_HISTORY < 1. The history algorithm switched off.
#else
static char *history[MAX_HISTORY+1]; /* history + current */
static int n_history;                /* saved history lines */
static int cur_history;              /* current pointer to history line */
#endif


#include <termios.h>
#define setTermSettings(fd,argp) tcsetattr(fd,TCSANOW,argp)
#define getTermSettings(fd,argp) tcgetattr(fd, argp);

/* Current termio and the previous termio before starting sh */
static struct termios initial_settings, new_settings;

static volatile int cmdedit_termw = 80;  /* actual terminal width */
static volatile int handlers_sets = 0;   /* Set next bites:  */

enum {
    SET_ATEXIT = 1,        /* when atexit() has been called and to get euid,uid,gid for fast compare */
    SET_WCHG_HANDLERS = 2, /* winchg signal handler */
    SET_RESET_TERM = 4,    /* if the terminal needs to be reset upon exit */
};

static int cmdedit_x;           /* real x terminal position */
static int cmdedit_y;           /* pseudoreal y terminal position */
static int cmdedit_prmt_len;    /* lenght prompt without colores string */

static int cursor;              /* required global for signal handler */
static int len;
static char *command_ps;
static char *cmdedit_prompt;


static void cmdedit_setwidth(int w, int redraw_flg);


/**************************************************
 * Input processing, terminal handling and prompt funcs
 *************************************************/

/* 
 * It is perfectly ok to pass a NULL for either width or for
 * height, in which case that value will not be set.  It is also
 * perfectly ok to have CONFIG_FEATURE_AUTOWIDTH disabled, in
 * which case you will always get 80x24 
 */
void get_terminal_width_height(int fd, int *width, int *height) {
    struct winsize win = { 0, 0, 0, 0 };
#ifdef CONFIG_FEATURE_AUTOWIDTH
    if (ioctl(fd, TIOCGWINSZ, &win) != 0) {
        win.ws_row = 24;
        win.ws_col = 80;
    }
#endif
    if (win.ws_row <= 1) {
        win.ws_row = 24;
    }
    if (win.ws_col <= 1) {
        win.ws_col = 80;
    }
    if (height) {
        *height = (int) win.ws_row;
    }
    if (width) {
        *width = (int) win.ws_col;
    }
}


static void win_changed(int nsig) {
    /* For reset */
    static __sighandler_t previous_SIGWINCH_handler;

    /* Emulate || signal call */
    if (nsig == -SIGWINCH || nsig == SIGWINCH) {
    	int width = 0;
    	get_terminal_width_height(0, &width, NULL);
    	cmdedit_setwidth(width, nsig == SIGWINCH);
    }

    /* Unix is not all standard in recall signal */
    if (nsig == -SIGWINCH)                            /* save previous handler */
    	previous_SIGWINCH_handler = signal(SIGWINCH, win_changed);
    else if (nsig == SIGWINCH)                        /* signaled called handler */
    	signal(SIGWINCH, win_changed);                /* set for next call */
    else                                              /* nsig == 0  */
    	signal(SIGWINCH, previous_SIGWINCH_handler);  /* set previous handler: reset */
}


static void cmdedit_reset_term(void) {
	if ((handlers_sets & SET_RESET_TERM) != 0) {
        /* sparc and other have broken termios support: use old termio handling. */
		setTermSettings(STDIN_FILENO, (void *) &initial_settings);
		handlers_sets &= ~SET_RESET_TERM;
	}
	if ((handlers_sets & SET_WCHG_HANDLERS) != 0) {
		/* reset SIGWINCH handler to previous (default) */
		win_changed(0);
		handlers_sets &= ~SET_WCHG_HANDLERS;
	}
	fflush(stdout);
}


/* 
 * Special for recount position for scroll and remove terminal margin effect 
 */
static void cmdedit_set_out_char(int next_char)
{
	int c = (int)((unsigned char) command_ps[cursor]);

	if (c == 0)
		c = ' ';        /* destroy end char? */
#ifdef CONFIG_FEATURE_NONPRINTABLE_INVERSE_PUT
	if (!Isprint(c)) {      /* Inverse put non-printable characters */
		if (c >= 128)
			c -= 128;
		if (c < ' ')
			c += '@';
		if (c == 127)
			c = '?';
		printf("\033[7m%c\033[0m", c);
	} else
#endif
		putchar(c);
	if (++cmdedit_x >= cmdedit_termw) {
		/* terminal is scrolled down */
		cmdedit_y++;
		cmdedit_x = 0;

		if (!next_char)
			next_char = ' ';
		/* destroy "(auto)margin" */
		putchar(next_char);
		putchar('\b');
	}
	cursor++;
}


static void passwd_set_out_char(int next_char) {
    int c = (int)((unsigned char) command_ps[cursor]);

    if (c == 0)
        c = ' ';        /* destroy end char? */
#ifdef CONFIG_FEATURE_NONPRINTABLE_INVERSE_PUT
    if (!Isprint(c)) {  /* Inverse put non-printable characters */
        if (c >= 128)
            c -= 128;
        if (c < ' ')
            c += '@';
        if (c == 127)
            c = '?';
    }
#endif
    if (++cmdedit_x >= cmdedit_termw) {
        /* terminal is scrolled down */
        cmdedit_y++;
        cmdedit_x = 0;

        if (!next_char)
            next_char = ' ';
    }
    cursor++;
}


/* 
 * Move to end line. Bonus: rewrite line from cursor 
 */
static void input_end(void) {
    while (cursor < len)
        cmdedit_set_out_char(0);
}


/* 
 * Go to the next line 
 */
static void goto_new_line(void) {
    input_end();
    if (cmdedit_x)
        putchar('\n');
}


static inline void out1str(const char *s) {
    if (s)
        fputs(s, stdout);
}


static inline void beep(void) {
    putchar('\007');
}


/* 
 * Move back one character, special for slow terminal 
 */
static void input_backward(int num) {
    if (num > cursor)
        num = cursor;
    cursor -= num;          /* new cursor (in command, not terminal) */

    if (cmdedit_x >= num) {         /* no to up line */
    	cmdedit_x -= num;
    	if (num < 4)
    	    while (num-- > 0)
    	    	putchar('\b');

    	else
    	    printf("\033[%dD", num);
    } 
    else {
    	int count_y;

    	if (cmdedit_x) {
    	    putchar('\r');          /* back to first terminal pos.  */
    	    num -= cmdedit_x;       /* set previous backward  */
        }
        count_y = 1 + num / cmdedit_termw;
        printf("\033[%dA", count_y);
        cmdedit_y -= count_y;
        /*  require  forward  after  uping */
        cmdedit_x = cmdedit_termw * count_y - num;
        printf("\033[%dC", cmdedit_x);  /* set term cursor */
    }
}


static void put_prompt(void) {
    out1str(cmdedit_prompt);
    cmdedit_x = cmdedit_prmt_len;   /* count real x terminal position */
    cursor = 0;
    cmdedit_y = 0;
}


static void parse_prompt(char *prmt_ptr) {
    cmdedit_prompt = prmt_ptr;
    cmdedit_prmt_len = strlen(prmt_ptr);
    put_prompt();
}


/* 
 * Draw prompt, editor line, and clear tail 
 */
static void redraw(int y, int back_cursor) {
    if (y > 0)                     /* up to start y */
        printf("\033[%dA", y);
    putchar('\r');
    put_prompt();
    input_end();                   /* rewrite */
    printf("\033[J");              /* destroy tail after cursor */
    input_backward(back_cursor);
}


/* 
 * Delete the char in front of the cursor 
 */
static void input_delete(void)
{
	int j = cursor;

	if (j == len)
		return;

	strcpy(command_ps + j, command_ps + j + 1);
	len--;
	input_end();                    /* rewtite new line */
	cmdedit_set_out_char(0);        /* destroy end char */
	input_backward(cursor - j);     /* back to old pos cursor */
}


/* 
 * Delete the char in back of the cursor 
 */
static void input_backspace(void) {
    if (cursor > 0) {
       	input_backward(1);
        input_delete();
    }
}


static void passwd_backspace(void) {
    if (cursor > 0) {
        cursor--;
        len--;
    }
}


/* 
 * Move forward one character 
 */
static void input_forward(void) {
    if (cursor < len)
        cmdedit_set_out_char(command_ps[cursor + 1]);
}


static void cmdedit_setwidth(int w, int redraw_flg) {
	cmdedit_termw = cmdedit_prmt_len + 2;
	if (w <= cmdedit_termw) {
		cmdedit_termw = cmdedit_termw % w;
	}
	if (w > cmdedit_termw) {
		cmdedit_termw = w;

		if (redraw_flg) {
			/* new y for current cursor */
			int new_y = (cursor + cmdedit_prmt_len) / w;

			/* redraw */
			redraw((new_y >= cmdedit_y ? new_y : cmdedit_y), len - cursor);
			fflush(stdout);
		}
	}
}


static void cmdedit_init(void) {
    cmdedit_prmt_len = 0;
    if ((handlers_sets & SET_WCHG_HANDLERS) == 0) {
        /* emulate usage handler to set handler and call yours work */
        win_changed(-SIGWINCH);
        handlers_sets |= SET_WCHG_HANDLERS;
    }

    if ((handlers_sets & SET_ATEXIT) == 0) {
        handlers_sets |= SET_ATEXIT;
        atexit(cmdedit_reset_term);     /* be sure to do this only once */
    }
}


/**************************************************
 * Tab completion funcs
 *************************************************/

#ifdef CONFIG_FEATURE_COMMAND_TAB_COMPLETION

/*
 * Do TAB completion
 */
static void input_tab(int *lastWasTab) {
    int sav_cursor;

    if (lastWasTab == 0) {          /* free all memory */
        sav_cursor = cursor;

        goto_new_line();
        trs_print_cmd_list(matrix->mode);
        redraw(0, len - sav_cursor);
    	return;
    }
    if (! *lastWasTab) {
        int          len_found,
                     recalc_pos;
        char         *tmp,
                     matchBuf[BUFSIZ];
        SList        cmd_list = NULL,
                     avail_list = NULL;
        CLICmdNode   cmd = NULL;
        CLIModeNode  mode = NULL;
        CLIAvailNode avail_node = NULL;

        *lastWasTab = TRUE;             /* flop trigger */

    	/* Make a local copy of the string -- up to the position of the cursor */
    	tmp = strncpy(matchBuf, command_ps, cursor);
    	tmp[cursor] = 0;

        recalc_pos = strlen(tmp);
        /* Empty command? */
        if (!recalc_pos) {
            sav_cursor = cursor;

            goto_new_line(); 
            trs_print_cmd_list(matrix->mode);
            redraw(0, len - sav_cursor);
            return;
        }

        /* If we have a space, print the complete cmd list */
        if (strchr(tmp, 32)) {
            sav_cursor = cursor;

            goto_new_line();
            trs_print_cmd_list(matrix->mode);
            redraw(0, len - sav_cursor);
            return;
        }

        mode = (CLIModeNode)matrix->mode->data;
        cmd_list = (SList)mode->cmd_list;
        while (cmd_list) {
            cmd = (CLICmdNode)cmd_list->data;
            /* A match was found */
            if (!strncmp(cmd->name, tmp, cursor)) {
                *lastWasTab = FALSE;
                recalc_pos = strlen(tmp);

                len_found = strlen(cmd->name);
                /* Have space to placed match? */
                if ((len_found - strlen(matchBuf) + len) < BUFSIZ) {
                        /* before word for match */
                        command_ps[cursor - recalc_pos] = 0;
                        /* add command name */
                        strcat(command_ps, cmd->name);
                        /* back to begin word for match */
                        input_backward(recalc_pos);
                        /* new pos */
                        recalc_pos = cursor + len_found;
                        /* new len */
                        len = strlen(command_ps);
                        /* write out the matched command */
                        redraw(cmdedit_y, len - recalc_pos);
                }            
            }
            cmd_list = cmd_list->next;
        }

        /* Search cmds in the avail list of the mode because they could be  */
        /* declared in another mode and made available to this one */
        avail_list = mode->avail; 
        while (avail_list) {
            avail_node = (CLIAvailNode)avail_list->data;
            if ((cmd_list = graph_mode_cmd_id_exist(avail_node->mode_id, avail_node->cmd_id)) != NULL) {
                cmd = (CLICmdNode)cmd_list->data;
                /* A match was found */
                if (!strncmp(cmd->name, tmp, cursor)) {
                    *lastWasTab = FALSE;
                    recalc_pos = strlen(tmp);

                    len_found = strlen(cmd->name);
                    /* Have space to placed match?  */
                    if ((len_found - strlen(matchBuf) + len) < BUFSIZ) {
                            /* before word for match */
                            command_ps[cursor - recalc_pos] = 0;
                            /* add command name */
                            strcat(command_ps, cmd->name);
                            /* back to begin word for match */
                            input_backward(recalc_pos);
                            /* new pos */
                            recalc_pos = cursor + len_found;
                            /* new len */
                            len = strlen(command_ps);
                            /* write out the matched command */
                            redraw(cmdedit_y, len - recalc_pos);
                    }
                }
            }
            avail_list = avail_list->next;
        }


    }
    else {
	/* Ok -- the last char was a TAB.  Since they */
	/* just hit TAB again, print a list of all the */
	/* available choices...  */
    	sav_cursor = cursor;        /* change goto_new_line() */

    	goto_new_line();
        trs_print_cmd_list(matrix->mode);
        redraw(0, len - sav_cursor);
    }
}
#endif  /* CONFIG_FEATURE_COMMAND_TAB_COMPLETION */


/**************************************************
 * History handling funcs
 *************************************************/

#if MAX_HISTORY >= 1
static void get_previous_history(void) {
    if(command_ps[0] != 0 || history[cur_history] == 0) {
        free(history[cur_history]);
        history[cur_history] = xstrdup(command_ps);
    }
    cur_history--;
}


static int get_next_history(void) {
    int ch = cur_history;

    if (ch < n_history) {
        get_previous_history(); /* save the current history line */
        return (cur_history = ch + 1);
    } 
    else {
        beep();
        return 0;
    }
}


#ifdef CONFIG_FEATURE_COMMAND_SAVEHISTORY
extern void load_history(const char *fromfile)
{
	FILE *fp;
	int hi;

	/* cleanup old */

	for(hi = n_history; hi > 0; ) {
		hi--;
		free ( history [hi] );
	}

	if (( fp = fopen ( fromfile, "r" ))) {

		for ( hi = 0; hi < MAX_HISTORY; ) {
			char * hl = bb_get_chomped_line_from_file(fp);
			int l;

			if(!hl)
				break;
			l = strlen(hl);
			if(l >= BUFSIZ)
				hl[BUFSIZ-1] = 0;
			if(l == 0 || hl[0] == ' ') {
				free(hl);
				continue;
			}
			history [hi++] = hl;
		}
		fclose ( fp );
	}
	cur_history = n_history = hi;
}


extern void save_history(const char *tofile) {
    FILE *fp = fopen ( tofile, "w" );

    if (fp) {
        int i;

        for (i = 0; i < n_history; i++ )
    	    fprintf(fp, "%s\n", history [i]);
        fclose ( fp );
    }
}

#endif    /* CONFIG_FEATURE_COMMAND_SAVEHISTORY */

#endif    /* MAX_HISTORY >= 1 */


/**************************************************
 * Command edit main
 *************************************************/

enum {
	ESC = 27,
	DEL = 127,
};


/*
 * This function is used to grab a character buffer
 * from the input file descriptor and allows you to
 * a string with full command editing (sort of like
 * a mini readline).
 *
 * The following standard commands are not implemented:
 * ESC-b -- Move back one word
 * ESC-f -- Move forward one word
 * ESC-d -- Delete back one word
 * ESC-h -- Delete forward one word
 * CTL-t -- Transpose two characters
 *
 */
int cmdedit_read_input(char command[BUFSIZ])
{
	int break_out = 0;
	int lastWasTab = FALSE;
        char *prompt = NULL;
	unsigned char c = 0;

	/* prepare before init handlers */
	cmdedit_y = 0;  /* quasireal y, not true work if line > xt*yt */
	len = 0;
	command_ps = command;

	getTermSettings(0, (void *) &initial_settings);
	memcpy(&new_settings, &initial_settings, sizeof(struct termios));
	new_settings.c_lflag &= ~ICANON;        /* unbuffered input */
	/* Turn off echoing and CTRL-C, so we can trap it */
	new_settings.c_lflag &= ~(ECHO | ECHONL | ISIG);
	/* Hmm, in linux c_cc[] not parsed if set ~ICANON */
	new_settings.c_cc[VMIN] = 1;
	new_settings.c_cc[VTIME] = 0;
	/* Turn off CTRL-C, so we can trap it */
#       ifndef _POSIX_VDISABLE
#               define _POSIX_VDISABLE '\0'
#       endif
	new_settings.c_cc[VINTR] = _POSIX_VDISABLE;
	command[0] = 0;

	setTermSettings(0, (void *) &new_settings);
	handlers_sets |= SET_RESET_TERM;

	/* Now initialize things */
	cmdedit_init();
	/* Print out the command prompt */
    if ((prompt = get_prompt(prompt)) == NULL)
        goto prepare_to_die;
    printf("parse_prompt=%s\r\n", prompt);
	parse_prompt(prompt);

	while (1) {

		fflush(stdout);                 /* buffered out to fast */

		if (safe_read(0, &c, 1) < 1)
			/* if we can't read input then exit  */
			goto prepare_to_die;

		switch (c) {
		case '\n':
		case '\r':
			/* Enter  */
			goto_new_line();
			break_out = 1;
			break;
		case 1:
			/* Control-a -- Beginning of line */
			input_backward(cursor);
			break;
		case 2:
			/* Control-b -- Move back one character  */
			input_backward(1);
			break;
		case 3:
			/* Control-c -- stop gathering input */
			goto_new_line();
			command[0] = 0;
			len = 0;
			lastWasTab = FALSE;
			put_prompt();
			break;
		case 4:
			/* Control-d -- Delete one character, or exit if the len=0 and no chars to delete  */
			if (len == 0) {
				errno = 0;
            /* FIXME: For debugging only */
prepare_to_die:
				printf(END_MSG);
				goto_new_line();
                free(matrix);
				exit(EXIT_SUCCESS);
			} 
            else {
				input_delete();
			}
			break;
		case 5:
			/* Control-e -- End of line  */
			input_end();
			break;
		case 6:
			/* Control-f -- Move forward one character  */
			input_forward();
			break;
		case '\b':
		case DEL:
			/* Control-h and DEL  */
			input_backspace();
			break;
		case '\t':
#ifdef CONFIG_FEATURE_COMMAND_TAB_COMPLETION
			input_tab(&lastWasTab);
#endif
			break;
		case 11:
			/* Control-k -- clear to end of line  */
			*(command + cursor) = 0;
			len = cursor;
			printf("\033[J");
			break;
		case 12:
			/* Control-l -- clear screen  */
			printf("\033[H");
			redraw(0, len-cursor);
			break;
#if MAX_HISTORY >= 1
		case 14:
			/* Control-n -- Get next command in history  */
			if (get_next_history())
				goto rewrite_line;
			break;
		case 16:
			/* Control-p -- Get previous command from history  */
			if (cur_history > 0) {
				get_previous_history();
				goto rewrite_line;
			} else {
				beep();
			}
			break;
#endif
		case 21:
			/* Control-U -- Clear line before cursor  */
			if (cursor) {
				strcpy(command, command + cursor);
				redraw(cmdedit_y, len -= cursor);
			}
			break;
		case 23:
			/* Control-W -- Remove the last word  */
			while (cursor > 0 && isspace(command[cursor-1]))
				input_backspace();
			while (cursor > 0 &&!isspace(command[cursor-1]))
				input_backspace();
			break;
		case ESC: {
			/* escape sequence follows  */
			if (safe_read(0, &c, 1) < 1)
				goto prepare_to_die;
			/* different vt100 emulations  */
			if (c == '[' || c == 'O') {
				if (safe_read(0, &c, 1) < 1)
					goto prepare_to_die;
			}
			if (c >= '1' && c <= '9') {
				unsigned char dummy;

				if (safe_read(0, &dummy, 1) < 1)
					goto prepare_to_die;
				if(dummy != '~')
					c = 0;
			}
			switch (c) {
#ifdef CONFIG_FEATURE_COMMAND_TAB_COMPLETION
			case '\t':                      /* Alt-Tab  */

				input_tab(&lastWasTab);
				break;
#endif
#if MAX_HISTORY >= 1
			case 'A':
				/* Up Arrow -- Get previous command from history  */
				if (cur_history > 0) {
					get_previous_history();
					goto rewrite_line;
				} else {
					beep();
				}
				break;
			case 'B':
				/* Down Arrow -- Get next command in history  */
				if (!get_next_history())
				break;
				/* Rewrite the line with the selected history item  */
rewrite_line:
				/* change command  */
				len = strlen(strcpy(command, history[cur_history]));
				/* redraw and go to end line  */
				redraw(cmdedit_y, 0);
				break;
#endif
			case 'C':
				/* Right Arrow -- Move forward one character  */
				input_forward();
				break;
			case 'D':
				/* Left Arrow -- Move back one character  */
				input_backward(1);
				break;
			case '3':
				/* Delete  */
				input_delete();
				break;
			case '1':
			case 'H':
				/* <Home>  */
				input_backward(cursor);
				break;
			case '4':
			case 'F':
				/* <End>  */
				input_end();
				break;
			default:
				c = 0;
				beep();
			}
			break;
		}

		default:        /* If it's regular input, do the normal thing  */
#ifdef CONFIG_FEATURE_NONPRINTABLE_INVERSE_PUT
			/* Control-V -- Add non-printable symbol  */
			if (c == 22) {
				if (safe_read(0, &c, 1) < 1)
					goto prepare_to_die;
				if (c == 0) {
					beep();
					break;
				}
			} else
#endif
			if (!Isprint(c))            /* Skip non-printable characters  */
				break;

			if (len >= (BUFSIZ - 2))    /* Need to leave space for enter  */
				break;

			len++;

			if (cursor == (len - 1)) {  /* Append if at the end of the line  */
				*(command + cursor) = c;
				*(command + cursor + 1) = 0;
				cmdedit_set_out_char(0);
			} 
            else {                      /* Insert otherwise  */
				int sc = cursor;

				memmove(command + sc + 1, command + sc, len - sc);
				*(command + sc) = c;
				sc++;
				/* rewrite from cursor  */
				input_end();
				/* to prev x pos + 1  */
				input_backward(cursor - sc);
			}
			break;
		}
		if (break_out)                  /* Enter is the command terminator, no more input. */
			break;

		if (c != '\t')
			lastWasTab = FALSE;
	}

	setTermSettings(0, (void *) &initial_settings);
	handlers_sets &= ~SET_RESET_TERM;

#if MAX_HISTORY >= 1
	/* Handle command history log  */
	/* cleanup may be saved current command line  */
	if (len> 0) {                           /* no put empty line  */
		int i = n_history;

		free(history[MAX_HISTORY]);
		history[MAX_HISTORY] = 0;
		/* After max history, remove the oldest command  */
		if (i >= MAX_HISTORY) {
			free(history[0]);
			for(i = 0; i < (MAX_HISTORY-1); i++)
				history[i] = history[i+1];
		}
		history[i++] = xstrdup(command);
		cur_history = i;
		n_history = i;
	}
#endif  /* MAX_HISTORY >= 1  */
	if (break_out > 0) {
		command[len++] = '\n';          /* set '\n'  */
		command[len] = 0;
	}
#if defined(CONFIG_FEATURE_CLEAN_UP) && defined(CONFIG_FEATURE_COMMAND_TAB_COMPLETION)
/*	input_tab(0); */                    /* strong free  */
#endif
	cmdedit_reset_term();
        if (prompt)
            free(prompt);
	return len;
}


/*
 * Description: Called from login_read_input when we have 
 *              to exit the app: FATAL error ocurred
 */
static void prepare_to_die(void) {
    printf(END_MSG);
    goto_new_line();
    free(matrix);
    exit(EXIT_SUCCESS);
}


/*
 * This function is used to grab a character buffer
 * from the input file descriptor when logging and allows 
 * a limited editing only when writing the login.
 *
 * Commands allowed:
 * CTRL-U, CTRL-W, CTRL-L, CTRL-H and DEL
 */
int login_read_input(char command[LOGIN_LEN], char *str) {
	int           break_out = 0;
    char          *prompt = NULL;
	unsigned char c = 0;

	/* Prepare before init handlers */
	cmdedit_y = 0;  /* quasireal y, not true work if line > xt*yt */
	len = 0;
	command_ps = command;

	getTermSettings(0, (void *) &initial_settings);
	memcpy(&new_settings, &initial_settings, sizeof(struct termios));
/*    new_settings.c_lflag &= ~(ICANON | ECHO); */
	new_settings.c_lflag &= ~ICANON;        /* unbuffered input */
	/* Turn off echoing and CTRL-C, so we can trap it */
	new_settings.c_lflag &= ~(ECHO | ECHONL | ISIG);
	/* Hmm, in linux c_cc[] not parsed if set ~ICANON */
	new_settings.c_cc[VMIN] = 1;
	new_settings.c_cc[VTIME] = 0;
	/* Turn off CTRL-C, so we can trap it */
#       ifndef _POSIX_VDISABLE
#               define _POSIX_VDISABLE '\0'
#       endif
	new_settings.c_cc[VINTR] = _POSIX_VDISABLE;
	command[0] = 0;

	setTermSettings(0, (void *) &new_settings);
	handlers_sets |= SET_RESET_TERM;

	/* Now initialize things */
	cmdedit_init();
	/* Print out the command prompt */
    if (strcmp(str, LOGIN_MSG) || strcmp(str, PASSWD_MSG))
        prompt = str;
    else
        prepare_to_die(); 
	parse_prompt(prompt);

	while (1) {

		fflush(stdout);                 /* buffered out to fast */

		if (safe_read(0, &c, 1) < 1)
			/* if we can't read input then exit  */
			prepare_to_die();

		switch (c) {
		case '\n':
		case '\r':
			/* Enter  */
			goto_new_line();
			break_out = 1;
			break;
		case 1:
			/* Control-a -- Beginning of line */
			break;
		case 2:
			/* Control-b -- Move back one character  */
			break;
		case 3:
			/* Control-c -- stop gathering input */
			break;
		case 4:
			/* Control-d -- Delete one character, or exit if the len=0 and no chars to delete  */
            /* FIXME: For debugging only */
            prepare_to_die();
			break;
		case 5:
			/* Control-e -- End of line  */
			break;
		case 6:
			/* Control-f -- Move forward one character  */
			break;
		case '\b':
		case DEL:
			/* Control-h and DEL  */
            if (!strcmp(str, LOGIN_MSG))
                input_backspace();
            else
                passwd_backspace();
			break;
		case '\t':
			break;
		case 11:
			/* Control-k -- clear to end of line  */
			break;
		case 12:
			/* Control-l -- clear screen  */
			printf("\033[H");
			redraw(0, len-cursor);
			break;
		case 14:
			/* Control-n -- Get next command in history  */
			break;
		case 16:
			/* Control-p -- Get previous command from history  */
			break;
		case 21:
			/* Control-U -- Clear line before cursor  */
            if (!strcmp(str, LOGIN_MSG)) {
			    if (cursor) {
				    strcpy(command, command + cursor);
				    redraw(cmdedit_y, len -= cursor);
			    }
            }
            else {
                while (cursor > 0)
                    passwd_backspace();
            }
			break;
		case 23:
			/* Control-W -- Remove the last word  */
            if (!strcmp(str, LOGIN_MSG)) {
                while (cursor > 0 && isspace(command[cursor-1]))
                    input_backspace();
                while (cursor > 0 && !isspace(command[cursor-1]))
                    input_backspace();
            }
            else {
                while (cursor > 0)
                    passwd_backspace();
            }
			break;
		case ESC:
			/* escape sequence follows  */
			break;
		default:        /* If it's regular input, do the normal thing  */
#ifdef CONFIG_FEATURE_NONPRINTABLE_INVERSE_PUT
			/* Control-V -- Add non-printable symbol  */
			if (c == 22) {
				if (safe_read(0, &c, 1) < 1)
					prepare_to_die();
				if (c == 0) {
					beep();
					break;
				}
			} else
#endif
			if (!Isprint(c))             /* Skip non-printable characters  */
				break;

			if (len >= (LOGIN_LEN - 2))  /* Need to leave space for enter  */
				break;

			len++;

            /* Append if at the end of the line, insert is not allowed */
			if (cursor == (len - 1)) {
				*(command + cursor) = c;
				*(command + cursor + 1) = 0;
                if (!strcmp(str, LOGIN_MSG))
				    cmdedit_set_out_char(0);
                else
                    passwd_set_out_char(0);
			} 
			break;
		}

		if (break_out)                  /* Enter is the command terminator, no more input. */
			break;
	}

	setTermSettings(0, (void *) &initial_settings);
	handlers_sets &= ~SET_RESET_TERM;

	if (break_out > 0) {
		command[len++] = '\n';          /* set '\n'  */
		command[len] = 0;
	}
	cmdedit_reset_term();

	return len;
}


#endif  /* CONFIG_FEATURE_COMMAND_EDITING */

