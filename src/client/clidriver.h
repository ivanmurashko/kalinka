/**
   @file clidriver.h
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


#ifndef KLK_ASTERISK_CLI_H
#define KLK_ASTERISK_CLI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "histedit.h"

    /** global variables */
    extern int continuation;
    extern EditLine *el;


    extern void ast_cli(int fd, const char *fmt, ...)
        __attribute__ ((format (printf, 2, 3)));

#define AST_MAX_ARGS     16
#define RESULT_SUCCESS    0
#define RESULT_SHOWUSAGE  1
#define RESULT_FAILURE    2
#define AST_MAX_CMD_LEN  16


    /*! \brief A command line entry */
    struct ast_cli_entry {
        /*! Null terminated list of the words of the command */
        const char *cmda[AST_MAX_CMD_LEN];
        /*! Handler for the command (fd for output, # of arguments, argument list).  Returns RESULT_SHOWUSAGE for improper arguments */
        int (*handler)(int fd, int argc, char *argv[],
                       const char*, const char*, int);
        /*! Summary of the command (< 60 characters) */
        const char *summary;
        /*! Detailed usage information */
        const char *usage;
        /*! Generate a list of possible completions for a given word */
        char *(*generator)(const char *line, const char *word,
                           int pos, int state,
                           const char* msgid);
        /*! For linking */
        struct ast_cli_entry *next;
        /*! For keeping track of usage */
        int inuse;
        /*! KLK specific data - command name*/
        const char *klkname;
        /*! KLK specific data - command message id*/
        const char *klkmsgid;
        /*! KLK specific data - is the module name used for the command*/
        int klkmodnameused;
    };

    int ast_cli_display_match_list(char **matches, int len, int max);

    /*! \brief Interprets a command
     * Interpret a command s, sending output to fd
     * Returns 0 on succes, -1 on failure
     */
    extern int ast_cli_command(int fd, char *s);

    /*! \brief Registers a command or an array of commands
     * \param e which cli entry to register
     * Register your own command
     * Returns 0 on success, -1 on failure
     */
    extern int ast_cli_register(struct ast_cli_entry *e);

    /*!
     * \brief Register multiple commands
     * \param e pointer to first cli entry to register
     * \param len number of entries to register
     */
    extern void ast_cli_register_multiple(struct ast_cli_entry *e, int len);

    /*! \brief Unregisters a command or an array of commands
     *
     * \param e which cli entry to unregister
     * Unregister your own command.  You must pass a completed ast_cli_entry structure
     * Returns 0.
     */
    extern int ast_cli_unregister(struct ast_cli_entry *e);

    /*!
     * \brief Unregister multiple commands
     * \param e pointer to first cli entry to unregister
     * \param len number of entries to unregister
     */
    extern void ast_cli_unregister_multiple(struct ast_cli_entry *e, int len);

    /*! \brief Readline madness
     * Useful for readline, that's about it
     * Returns 0 on success, -1 on failure
     */
    extern char *ast_cli_generator(char *, char *, int);

    extern int ast_cli_generatornummatches(char *, char *);
    extern char **ast_cli_completion_matches(char *, char *);

    /*!
     * \brief editline's el_init() wrapper
     */
    extern EditLine *ast_el_init(History *hist,const char *prog, FILE *fin, FILE *fout, FILE *ferr);
    /*!
     * \brief main CLI loop
     */
    extern void ast_cli_worker(EditLine *el,History *hist,HistEvent *ev,Tokenizer *tok);

#ifdef __cplusplus
}
#endif

#endif  /* _ASTERISK_CLI_H */
