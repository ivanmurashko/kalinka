/**
   @file clidriver.c
   @brief This file is part of Kalinka mediaserver.
   @author
   - Asterisk
   - Roman Laptev <roma@bsproject.net>
   - Ivan Murashko <ivan.murashko@gmail.com>

   Copyright (c) 2007-2012 Kalinka Team

   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files (the
   "Software"), to deal in the Software without restriction, including
   without limitation the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to
   the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
   IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
   CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
   TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

   CHANGE HISTORY

   @date
   - 2008/08/06 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

/*
 * Asterisk -- An open source telephony toolkit.
 *
 * Copyright (C) 1999 - 2005, Digium, Inc.
 *
 * Mark Spencer <markster@digium.com>
 *
 * See http://www.asterisk.org for more information about
 * the Asterisk project. Please do not directly contact
 * any of the maintainers of this project for assistance;
 * the project provides a web site, mailing lists and IRC
 * channels for your use.
 *
 * This program is free software, distributed under the terms of
 * the GNU General Public License Version 2. See the LICENSE file
 * at the top of the source tree.
 */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define _GNU_SOURCE //vasprintf
#include <stdio.h>

#include <unistd.h>
#include <stdlib.h>
#include <sys/signal.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <errno.h>
#include <poll.h>
#include <sys/ioctl.h>

#include "clidriver.h"
#include "log.h"


static struct ast_cli_entry *find_cli(const char *cmds[], int exact);

//static int handle_tmp(int fd, int argc, char *argv[]);
static int handle_help(int fd, int argc, char *argv[],
                       const char*, const char*, int);


struct ast_cli_entry *helpers = NULL;

static char *__ast_cli_generator(const char *text, const char *word,
                                 int state, int lock);

static char * complete_help(const char *text, const char *word,
                            int pos, int state,
                            const char* msgod)
{
    /* skip first 4 or 5 chars, "help "*/
    int l = strlen(text);

    if (l > 5)
        l = 5;
    text += l;
    /* XXX watch out, should stop to the non-generator parts */
    return __ast_cli_generator(text, word, state, 1);
}


static struct ast_cli_entry builtins[] = {
    /* Keep alphabetized, with longer matches first (example: abcd before abc) */
    {
        { "help", NULL },
        handle_help,
        "Display help list, or specific help on a command",
        "Usage: help [topic]\n",
        complete_help, NULL, 0, NULL, NULL
    },
    {
        {NULL}, NULL, NULL,
        NULL,
        NULL, NULL, 0, NULL, NULL

    }
};




static void sig(int i)
{
    (void)fprintf(stderr, "Got signal %d.\n", i);
    el_reset(el);
}
static char *prompt()
{
    static char a[] = "cli$ ";
    static char b[] = "cli> ";
    return(continuation ? b : a);
}

static char *cli_complete(EditLine *el, int ch)
{
    int len=0;
    char *ptr;
    int nummatches = 0;
    char **matches;
    int retval = CC_ERROR;

    LineInfo *lf = (LineInfo *)el_line(el);

    *(char *)lf->cursor = '\0';
    ptr = (char *)lf->cursor;
    if(ptr) while(ptr > lf->buffer) {
            if(isspace(*ptr)) {
                ptr++;
                break;
            }
            ptr--;
        }
    len = lf->cursor - ptr;

    nummatches = ast_cli_generatornummatches((char *)lf->buffer,ptr);
    matches = ast_cli_completion_matches((char *)lf->buffer,ptr);

    if(matches) {
        int i;
        int matches_num, maxlen, match_len;

        if (matches[0][0] != '\0') {
            el_deletestr(el, (int)len);
            el_insertstr(el, matches[0]);
            retval = CC_REFRESH;
        }

        if(nummatches == 1) {
            /* Found an exact match */
            el_insertstr(el, " ");
            retval = CC_REFRESH;
        }
        else {
            /* Must be more than one match */
            for (i=1, maxlen=0; matches[i]; i++) {
                match_len = strlen(matches[i]);
                if (match_len > maxlen)
                    maxlen = match_len;
            }
            matches_num = i - 1;
            if (matches_num >1) {
                fprintf(stdout, "\n");
                ast_cli_display_match_list(matches, nummatches, maxlen);
                retval = CC_REDISPLAY;
            } else {
                el_insertstr(el," ");
                retval = CC_REFRESH;
            }
        }
        free(matches);
    }

    return (char *)(long)retval;
}

EditLine *ast_el_init(History *hist,const char *prog, FILE *fin, FILE *fout, FILE *ferr)
{
    (void)signal(SIGINT, sig);
    (void)signal(SIGQUIT,sig);
    (void)signal(SIGHUP, sig);
    (void)signal(SIGTERM,sig);

    EditLine *el = el_init(prog, fin, fout, ferr);

    el_set(el, EL_EDITOR, "emacs");  /* Default editor is vi   */
    el_set(el, EL_SIGNAL, 1); /* Handle signals gracefully  */
    el_set(el, EL_PROMPT, prompt);  /* Set the prompt function  */

    /* Tell editline to use this history interface  */
    el_set(el, EL_HIST, history, hist);

    /* Add a user-defined function  */
    el_set(el, EL_ADDFN, "ed-complete", "Complete argument", cli_complete);

    /* Bind tab to it     */
    el_set(el, EL_BIND, "^I", "ed-complete", NULL);

    /*
     * Bind j, k in vi command mode to previous and next line, instead
     * of previous and next history.
     */
    el_set(el, EL_BIND, "-a", "k", "ed-prev-line", NULL);
    el_set(el, EL_BIND, "-a", "j", "ed-next-line", NULL);
    /*
     * Source the user's defaults file.
     */
    el_source(el, NULL);

    return(el);
}

static void join(char *dest, size_t destsize, const char *w[], int tws)
{
    int x;
    /* Join words into a string */
    if (!dest || destsize < 1) {
        return;
    }
    dest[0] = '\0';
    for (x=0;w[x];x++) {
        if (x)
            strncat(dest, " ", destsize - strlen(dest) - 1);
        strncat(dest, w[x], destsize - strlen(dest) - 1);
    }
    if (tws && !ISEMPTY(dest))
        strncat(dest, " ", destsize - strlen(dest) - 1);
}

static void join2(char *dest, size_t destsize, const char *w[])
{
    int x;
    /* Join words into a string */
    if (!dest || destsize < 1) {
        return;
    }
    dest[0] = '\0';
    for (x=0;w[x];x++) {
        strncat(dest, w[x], destsize - strlen(dest) - 1);
    }
}

static char *find_best(char *argv[])
{
    static char cmdline[80];
    int x;
    /* See how close we get, then print the  */
    const char *myargv[AST_MAX_CMD_LEN];
    for (x=0;x<AST_MAX_CMD_LEN;x++)
        myargv[x]=NULL;
    for (x=0;argv[x];x++) {
        myargv[x] = argv[x];
        if (!find_cli(myargv, -1))
            break;
    }
    join(cmdline, sizeof(cmdline), myargv, 0);
    return cmdline;
}


/*! If you are calling ast_carefulwrite, it is assumed that you are calling
  it on a file descriptor that _DOES_ have NONBLOCK set.  This way,
  there is only one system call made to do a write, unless we actually
  have a need to wait.  This way, we get better performance. */
int ast_carefulwrite(int fd, char *s, int len, int timeoutms)
{
    /* Try to write string, but wait no more than ms milliseconds
       before timing out */
    int res=0;
    struct pollfd fds[1];
    while(len) {
        res = write(fd, s, len);
        if ((res < 0) && (errno != EAGAIN)) {
            return -1;
        }
        if (res < 0) res = 0;
        len -= res;
        s += res;
        res = 0;
        if (len) {
            fds[0].fd = fd;
            fds[0].events = POLLOUT;
            /* Wait until writable again */
            res = poll(fds, 1, timeoutms);
            if (res < 1)
                return -1;
        }
    }
    return res;
}

void ast_cli(int fd, const char *fmt, ...)
{
    char *stuff;
    int res = 0;
    va_list ap;

    va_start(ap, fmt);
    res = vasprintf(&stuff, fmt, ap);
    va_end(ap);
    if (res == -1)
    {
        klk_log(KLKLOG_ERROR, "Error in vasprintf()");
    }
    else {
        ast_carefulwrite(fd, stuff, strlen(stuff), 100);
        free(stuff);
    }
}


static int help_workhorse(int fd, const char *match[])
{
    char fullcmd1[80] = "";
    char fullcmd2[80] = "";
    char matchstr[80];
    const char *fullcmd = NULL;
    struct ast_cli_entry *e, *e1, *e2;
    e1 = builtins;
    e2 = helpers;
    if (match)
        join(matchstr, sizeof(matchstr), match, 0);
    while(e1->cmda[0] || e2) {
        if (e2)
            join(fullcmd2, sizeof(fullcmd2), e2->cmda, 0);
        if (e1->cmda[0])
            join(fullcmd1, sizeof(fullcmd1), e1->cmda, 0);
        if (!e1->cmda[0] ||
            (e2 && (strcmp(fullcmd2, fullcmd1) < 0))) {
            /* Use e2 */
            e = e2;
            fullcmd = fullcmd2;
            /* Increment by going to next */
            e2 = e2->next;
        } else {
            /* Use e1 */
            e = e1;
            fullcmd = fullcmd1;
            e1++;
        }
        /* Hide commands that start with '_' */
        if (fullcmd[0] == '_')
            continue;
        if (match) {
            if (strncasecmp(matchstr, fullcmd, strlen(matchstr))) {
                continue;
            }
        }
        ast_cli(fd, "%30.30s  %s\n", fullcmd, e->summary);
    }
    return 0;
}


static int handle_help(int fd, int argc, char *argv[],
                       const char* cmdname, const char* msgid, int modnameused)
{
    struct ast_cli_entry *e;
    char fullcmd[80];
    if ((argc < 1))
        return RESULT_SHOWUSAGE;
    if (argc > 1) {
        e = find_cli((const char**)(argv + 1), 1);
        if (e) {
            if (e->usage)
                ast_cli(fd, "%s", e->usage);
            else {
                join(fullcmd, sizeof(fullcmd), (const char**)(argv+1), 0);
                ast_cli(fd, "No help text available for '%s'.\n", fullcmd);
            }
        } else {
            if (find_cli((const char**)(argv + 1) , -1)) {
                return help_workhorse(fd, (const char**)(argv + 1));
            }
            else {
                join(fullcmd, sizeof(fullcmd), (const char**)(argv+1), 0);
                ast_cli(fd, "No such command '%s'.\n", fullcmd);
            }
        }
    } else {
        return help_workhorse(fd, NULL);
    }
    return(RESULT_SUCCESS);
}

static struct ast_cli_entry *find_cli(const char *cmds[], int exact)
{
    int x;
    int y;
    int match;
    struct ast_cli_entry *e=NULL;

    for (e=helpers;e;e=e->next) {
        match = 1;
        for (y=0;match && cmds[y]; y++) {
            if (!e->cmda[y] && !exact)
                break;
            if (!e->cmda[y] || strcasecmp(e->cmda[y], cmds[y]))
                match = 0;
        }
        if ((exact > -1) && e->cmda[y])
            match = 0;
        if (match)
            break;
    }
    if (e)
        return e;
    for (x=0;builtins[x].cmda[0];x++) {
        /* start optimistic */
        match = 1;
        for (y=0;match && cmds[y]; y++) {
            /* If there are no more words in the candidate command, then we're
               there.  */
            if (!builtins[x].cmda[y] && !exact)
                break;
            /* If there are no more words in the command (and we're looking for
               an exact match) or there is a difference between the two words,
               then this is not a match */
            if (!builtins[x].cmda[y] || strcasecmp(builtins[x].cmda[y], cmds[y]))
                match = 0;
        }
        /* If more words are needed to complete the command then this is not
           a candidate (unless we're looking for a really inexact answer  */
        if ((exact > -1) && builtins[x].cmda[y])
            match = 0;
        if (match)
            return &builtins[x];
    }
    return NULL;
}



static char *parse_args(const char *s, int *argc, char *argv[], int max, int *trailingwhitespace)
{
    char *dup, *cur;
    int x = 0;
    int quoted = 0;
    int escaped = 0;
    int whitespace = 1;

    *trailingwhitespace = 0;
    if (!(dup = strdup(s)))
        return NULL;

    cur = dup;
    while (*s) {
        if ((*s == '"') && !escaped) {
            quoted = !quoted;
            if (quoted & whitespace) {
                /* If we're starting a quoted string, coming off white space, start a new argument */
                if (x >= (max - 1)) {
                    klk_log(KLKLOG_ERROR, "Too many arguments, truncating");
                    break;
                }
                argv[x++] = cur;
                whitespace = 0;
            }
            escaped = 0;
        } else if (((*s == ' ') || (*s == '\t')) && !(quoted || escaped)) {
            /* If we are not already in whitespace, and not in a quoted string or
               processing an escape sequence, and just entered whitespace, then
               finalize the previous argument and remember that we are in whitespace
            */
            if (!whitespace) {
                *(cur++) = '\0';
                whitespace = 1;
            }
        } else if ((*s == '\\') && !escaped) {
            escaped = 1;
        } else {
            if (whitespace) {
                /* If we are coming out of whitespace, start a new argument */
                if (x >= (max - 1)) {
                    klk_log(KLKLOG_ERROR, "Too many arguments, truncating");
                    break;
                }
                argv[x++] = cur;
                whitespace = 0;
            }
            *(cur++) = *s;
            escaped = 0;
        }
        s++;
    }
    /* Null terminate */
    *(cur++) = '\0';
    argv[x] = NULL;
    *argc = x;
    *trailingwhitespace = whitespace;
    return dup;
}


static int ast_el_sort_compare(const void *i1, const void *i2)
{
    char *s1, *s2;

    s1 = ((char **)i1)[0];
    s2 = ((char **)i2)[0];

    return strcasecmp(s1, s2);
}

static char *__ast_cli_generator(const char *text, const char *word,
                                 int state, int lock)
{
    char *argv[AST_MAX_ARGS];
    struct ast_cli_entry *e, *e1, *e2;
    int x;
    int matchnum=0;
    char *dup = NULL;
    const char *res = NULL;
    char fullcmd1[80] = "";
    char fullcmd2[80] = "";
    char matchstr[80] = "";
    char *fullcmd = NULL;
    int tws;

    if ((dup = parse_args(text, &x, argv, sizeof(argv) / sizeof(argv[0]), &tws))) {
        join(matchstr, sizeof(matchstr), (const char**)argv, tws);
/*    if (lock)
      ast_mutex_lock(&clilock);*/
        e1 = builtins;
        e2 = helpers;
        while(e1->cmda[0] || e2) {
            if (e2)
                join(fullcmd2, sizeof(fullcmd2), e2->cmda, tws);
            if (e1->cmda[0])
                join(fullcmd1, sizeof(fullcmd1), e1->cmda, tws);
            if (!e1->cmda[0] ||
                (e2 && (strcmp(fullcmd2, fullcmd1) < 0))) {
                /* Use e2 */
                e = e2;
                fullcmd = fullcmd2;
                /* Increment by going to next */
                e2 = e2->next;
            } else {
                /* Use e1 */
                e = e1;
                fullcmd = fullcmd1;
                e1++;
            }
            if ((fullcmd[0] != '_') && !strncasecmp(matchstr, fullcmd, strlen(matchstr))) {
                /* We contain the first part of one or more commands */
                /* Now, what we're supposed to return is the next word... */
                if (!ISEMPTY(word) && x>0) {
                    res = e->cmda[x-1];
                } else {
                    res = e->cmda[x];
                }
                if (res) {
                    matchnum++;
                    if (matchnum > state) {
/*            if (lock)
              ast_mutex_unlock(&clilock);*/
                        free(dup);
                        return strdup(res);
                    }
                }
            }
            if (e->generator && !strncasecmp(matchstr, fullcmd, strlen(fullcmd))/* &&
                                                                                   (matchstr[strlen(fullcmd)] < 33)*/)
            {
                /* We have a command in its entirity within us -- theoretically only one
                   command can have this occur */
                fullcmd = e->generator(matchstr, word, (!ISEMPTY(word) ? (x - 1) : (x)), state, e->klkmsgid);
                if (fullcmd) {
/*          if (lock)
            ast_mutex_unlock(&clilock);*/
                    free(dup);
                    return fullcmd;
                }
            }

        }
/*    if (lock)
      ast_mutex_unlock(&clilock);*/
        free(dup);
    }
    return NULL;
}

char *ast_cli_generator(char *text, char *word, int state)
{
    return __ast_cli_generator(text, word, state, 1);
}

/* This returns the number of unique matches for the generator */
int ast_cli_generatornummatches(char *text, char *word)
{
    int matches = 0, i = 0;
    char *buf = NULL, *oldbuf = NULL;

    while ( (buf = ast_cli_generator(text, word, i++)) ) {
        if (!oldbuf || strcmp(buf,oldbuf))
            matches++;
        if (oldbuf)
            free(oldbuf);
        oldbuf = buf;
    }
    if (oldbuf)
        free(oldbuf);
    return matches;
}


int ast_get_termcols(int fd)
{
    struct winsize win;
    int cols = 0;

    if (!isatty(fd))
        return -1;

    if ( ioctl(fd, TIOCGWINSZ, &win) != -1 ) {
        if ( !cols && win.ws_col > 0 )
            cols = (int) win.ws_col;
    } else {
        /* assume 80 characters if the ioctl fails for some reason */
        cols = 80;
    }

    return cols;
}


int ast_cli_display_match_list(char **matches, int len, int max)
{
    int i, idx, limit, count;
    int screenwidth = 0;
    int numoutput = 0, numoutputline = 0;

    screenwidth = ast_get_termcols(STDOUT_FILENO);

    /* find out how many entries can be put on one line, with two spaces between strings */
    limit = screenwidth / (max + 2);
    if (limit == 0)
        limit = 1;

    /* how many lines of output */
    count = len / limit;
    if (count * limit < len)
        count++;

    idx = 1;

    qsort(&matches[0], (size_t)(len + 1), sizeof(char *), ast_el_sort_compare);

    for (; count > 0; count--) {
        numoutputline = 0;
        for (i=0; i < limit && matches[idx]; i++, idx++) {

            /* Don't print dupes */
            if ( (matches[idx+1] != NULL && strcmp(matches[idx], matches[idx+1]) == 0 ) ) {
                i--;
                free(matches[idx]);
                matches[idx] = NULL;
                continue;
            }

            numoutput++;
            numoutputline++;
            fprintf(stdout, "%-*s  ", max, matches[idx]);
            free(matches[idx]);
            matches[idx] = NULL;
        }
        if (numoutputline > 0)
            fprintf(stdout, "\n");
    }

    return numoutput;
}


char **ast_cli_completion_matches(char *text, char *word)
{
    char **match_list = NULL, *retstr, *prevstr;
    size_t match_list_len, max_equal, which, i;
    size_t matches = 0;

    match_list_len = 1;
    while ((retstr = ast_cli_generator(text, word, matches)) != NULL) {
        if (matches + 1 >= match_list_len) {
            match_list_len <<= 1;
            match_list = realloc(match_list, match_list_len * sizeof(char *));
        }
        match_list[++matches] = retstr;
    }

    if (!match_list)
        return (char **) NULL;

    which = 2;
    prevstr = match_list[1];
    max_equal = strlen(prevstr);
    for (; which <= matches; which++) {
        for (i = 0; i < max_equal && toupper(prevstr[i]) == toupper(match_list[which][i]); i++)
            continue;
        max_equal = i;
    }

    retstr = malloc(max_equal + 1);
    (void) strncpy(retstr, match_list[1], max_equal);
    retstr[max_equal] = '\0';
    match_list[0] = retstr;

    if (matches + 1 >= match_list_len)
        match_list = realloc(match_list, (match_list_len + 1) * sizeof(char *));
    match_list[matches + 1] = (char *) NULL;

    return (match_list);
}



int ast_cli_command(int fd, char *s)
{
    char *argv[AST_MAX_ARGS];
    struct ast_cli_entry *e;
    int x;
    char *dup;
    int tws;

    if((dup = parse_args(s, &x, argv, sizeof(argv) / sizeof(argv[0]), &tws))) {
        /* We need at least one entry, or ignore */
        if (x > 0) {
//       ast_mutex_lock(&clilock);
            e = find_cli((const char**)argv, 0);
            if (e)
                e->inuse++;
//       ast_mutex_unlock(&clilock);
            if(e) {
                switch(e->handler(fd, x, argv, e->klkname,
                                  e->klkmsgid, e->klkmodnameused)) {
                case RESULT_SHOWUSAGE:
                    if (e->usage)
                        ast_cli(fd, "%s", e->usage);
                    else
                        ast_cli(fd, "%s", "Invalid usage, but no usage information available.\n");
                    break;
                }
            }
            else
                ast_cli(fd, "No such command '%s' (type 'help' for help)\n", find_best(argv));
            if(e) {
//         ast_mutex_lock(&clilock);
                e->inuse--;
//         ast_mutex_unlock(&clilock);
            }
        }
        free(dup);
    }
    else {
        klk_log(KLKLOG_ERROR, "Error in parse_args()");
        return -1;
    }
    return 0;
}

int ast_cli_unregister(struct ast_cli_entry *e)
{
    struct ast_cli_entry *cur, *l=NULL;
//   ast_mutex_lock(&clilock);
    cur = helpers;
    while(cur) {
        if(e == cur) {
            if(e->inuse) {
                klk_log(KLKLOG_ERROR, "can't remove command that is in use");
            }
            else { /* Rewrite */
                if(l)
                    l->next = e->next;
                else
                    helpers = e->next;
                e->next = NULL;
                break;
            }
        }
        l = cur;
        cur = cur->next;
    }
//   ast_mutex_unlock(&clilock);
    return 0;
}

int ast_cli_register(struct ast_cli_entry *e)
{
    struct ast_cli_entry *cur, *l=NULL;
    char fulle[80] ="", fulltst[80] ="";
    static size_t len;
//   ast_mutex_lock(&clilock);
    join2(fulle, sizeof(fulle), e->cmda);
    if (find_cli(e->cmda, -1)) {
//     ast_mutex_unlock(&clilock);
        klk_log(KLKLOG_ERROR,
                "command '%s' already registered (or something close enough)", fulle);
        return -1;
    }
    cur = helpers;
    while(cur) {
        join2(fulltst, sizeof(fulltst), cur->cmda);
        len = strlen(fulltst);
        if(strlen(fulle) < len)
            len = strlen(fulle);
        if (strncasecmp(fulle, fulltst, len) < 0) {
            if (l) {
                e->next = l->next;
                l->next = e;
            } else {
                e->next = helpers;
                helpers = e;
            }
            break;
        }
        l = cur;
        cur = cur->next;
    }
    if (!cur) {
        if (l)
            l->next = e;
        else
            helpers = e;
        e->next = NULL;
    }
//   ast_mutex_unlock(&clilock);
    return 0;
}

/*
 * register/unregister an array of entries.
 */
void ast_cli_register_multiple(struct ast_cli_entry *e, int len)
{
    int i;

    for (i=0; i < len; i++)
        ast_cli_register(e + i);
}

void ast_cli_unregister_multiple(struct ast_cli_entry *e, int len)
{
    int i;

    for (i=0; i < len; i++)
        ast_cli_unregister(e + i);
}

void ast_cli_worker(EditLine *el,History *hist,HistEvent *ev,Tokenizer *tok)
{
    char *buf;
    int num,ncontinuation;

    while((buf = (char *)el_gets(el, &num)) != NULL && num != 0) {
        int ac = 0;
        const char **av;

        if(!continuation && num == 1) continue;

        ncontinuation = tok_str(tok, (const char *)buf, &ac, &av) > 0;
        history(hist,ev, continuation ? H_APPEND : H_ENTER, buf);

        continuation = ncontinuation;
        ncontinuation = 0;

        if(**av == 'h' || **av == 'e') {

            if(strcmp(av[0], "exit") == 0) return;

            if(strcmp(av[0], "history") == 0) {
                int rv;

                switch (ac) {
                case 1:
                    for(rv = history(hist,ev, H_LAST); rv != -1;
                        rv = history(hist,ev, H_PREV))
                        (void)fprintf(stdout, "%4d %s", ev->num, ev->str);
                    break;

                case 2:
                    if(strcmp(av[1], "clear") == 0)
                        history(hist,ev, H_CLEAR);
                    else
                        goto badhist;
                    break;

                case 3:
                    if(strcmp(av[1], "load") == 0)
                        history(hist,ev, H_LOAD, av[2]);
                    else if(strcmp(av[1], "save") == 0)
                        history(hist,ev, H_SAVE, av[2]);
                    break;

                badhist:
                default:
                    (void)fprintf(stderr, "Bad history arguments\n");
                    break;
                }
                tok_reset(tok);
            }
        }

        ac = strlen(buf) - 1;
        if(*(buf + ac) == '\n' || *(buf + ac) == '\r')
            *(buf + ac) = 0;
        ast_cli_command(STDOUT_FILENO, buf);
        tok_reset(tok);
    }

}


