/*
 * Copyright (c) 2002, The Tendra Project <http://www.ten15.org/>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice unmodified, this list of conditions, and the following
 *    disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
 *    		 Crown Copyright (c) 1997
 *
 *    This TenDRA(r) Computer Program is subject to Copyright
 *    owned by the United Kingdom Secretary of State for Defence
 *    acting through the Defence Evaluation and Research Agency
 *    (DERA).  It is made available to Recipients with a
 *    royalty-free licence for its use, reproduction, transfer
 *    to other parties and amendment for any purpose not excluding
 *    product development provided that any such use et cetera
 *    shall be deemed to be acceptance of the following conditions:-
 *
 *        (1) Its Recipients shall ensure that this Notice is
 *        reproduced upon any copies or amended versions of it;
 *
 *        (2) Any amended version of it shall be clearly marked to
 *        show both the nature of and the organisation responsible
 *        for the relevant amendment or amendments;
 *
 *        (3) Its onward transfer from a recipient to another
 *        party shall be deemed to be that party's acceptance of
 *        these conditions;
 *
 *        (4) DERA gives no warranty or assurance as to its
 *        quality or suitability for any purpose and DERA accepts
 *        no liability whatsoever in relation to any use to which
 *        it may be put.
 *
 * $TenDRA$
 */


#include "config.h"
#include "msgcat.h"
#include "ostream.h"
#include "tenapp.h"

#include "release.h"
#include "fetch.h"
#include "types.h"
#include "read_types.h"
#include "analyser.h"
#include "check.h"
#include "de_types.h"
#include "de_capsule.h"
#include "enc_cap.h"
#include "eval.h"
#include "file.h"
#include "help.h"
#include "node.h"
#include "read_cap.h"
#include "shape.h"
#include "tdf.h"
#include "table.h"
#include "utility.h"
#include "write.h"


/*
 *    PROCESS AN OUTPUT OPTION
 *
 *    The show option corresponding to arg is set to t.
 */

static boolean
output_option(char *arg, boolean t)
{
    boolean *p = null;
    if (streq (arg, "tokdecs")) {
		p = &show_tokdecs;
    } else if (streq (arg, "tokdefs")) {
		p = &show_tokdefs;
    } else if (streq (arg, "aldecs")) {
		p = &show_aldecs;
    } else if (streq (arg, "aldefs")) {
		p = &show_aldefs;
    } else if (streq (arg, "tagdecs")) {
		p = &show_tagdecs;
    } else if (streq (arg, "tagdefs")) {
		p = &show_tagdefs;
    }
    if (p == null) return (0);
    if (t) {
		show_tokdecs = 0;
		show_tokdefs = 0;
		show_aldefs = 0;
		show_tagdecs = 0;
		show_tagdefs = 0;
    }
    *p = t;
    return (1);
}


static void
msg_uh_where(char ch, void *pp)
{
	UNUSED(ch);
	UNUSED(pp);
    if (input_file) {
		write_fmt (msg_stream, ", %s", input_file);
		if (text_input) {
			write_fmt (msg_stream, ", line %ld", line_no);
		} else {
			long b = input_posn ();
			if (capname)
				write_fmt(msg_stream, ", capsule %s", capname);
			switch (decode_status) {
			case 0:
				write_fmt (msg_stream, " (at outermost level)");
				break;
			case 1:
				write_fmt (msg_stream, " (in linking information)");
				break;
			case 2 :
				write_fmt (msg_stream, " (in unit body)");
				break;
			}
			write_fmt (msg_stream, ", byte %ld, bit %ld", b / 8, b % 8);
			if (decode_status == 0)
				write_fmt (msg_stream, " (Illegal TDF capsule?)");
			if (decode_status >= 1 && !have_version)
				write_fmt (msg_stream, " (TDF version error?)");
		}
    }
}


/*
 *    MAIN ROUTINE
 *
 *    This is the main routine.  It processes the command line arguments
 *    and calls the appropriate input and output routines.
 */

int
main(int argc, char **argv)
{
    int a;
    int status = 0;
    boolean expand = 0;
    boolean evaluate = 0;
    boolean lib_input = 0;
    boolean output_next = 0;
    void (*input_fn)(void) ;
    void (*output_fn)(void) ;

	tenapp_init(argc, argv, "TDF notation compiler", "1.9");
	msg_uh_add(MSG_GLOB_where, msg_uh_where);

    /* Default action : read text, encode TDF capsule */
    input_fn = read_capsule;
    output_fn = enc_capsule;
    text_input = 1;
    text_output = 0;

    /* Initialize internal tables */
    output = stdout;
    init_tables ();
    init_constructs ();

    /* Scan arguments */
    for (a = 1 ; a < argc ; a++) {
		char *arg = argv [a];
		if (output_next) {
			open_output (arg);
			output_next = 0;
		} else if (*arg == '-') {
			boolean known = 0;
			switch (arg [1]) {
			case 'h' : {
				/* Help option */
				if (streq (arg, "-help")) {
					if (status) MSG_getopt_too_many_arguments ();
					a++;
					if (a == argc) {
						help ("all");
					} else {
						while (a < argc) {
							help (argv [a]);
							a++;
						}
					}
					exit (exit_status);
				}
				break;
			}
			case 'd' : {
				if (arg [2] == 0 || streq (arg, "-decode")) {
					/* Decode mode */
					input_fn = de_capsule;
					text_input = 0;
					known = 1;
				}
				break;
			}
			case 'e' : {
				if (arg [2] == 0 || streq (arg, "-encode")) {
					/* Encode mode */
					output_fn = enc_capsule;
					text_output = 0;
					known = 1;
				} else if (streq (arg, "-eval")) {
					evaluate = 1;
					known = 1;
				} else if (streq (arg, "-expand")) {
					expand = 1;
					known = 1;
				}
				break;
			}
			case 'r' : {
				if (arg [2] == 0 || streq (arg, "-read")) {
					/* Read mode */
					input_fn = read_capsule;
					text_input = 1;
					known = 1;
				}
				break;
			}
			case 'w' : {
				if (arg [2] == 0 || streq (arg, "-write")) {
					/* Write mode */
					output_fn = print_capsule;
					text_output = 1;
					known = 1;
				}
				break;
			}
			case 'p' : {
				if (arg [2] == 0 || streq (arg, "-print")) {
					/* Pretty printer mode */
					input_fn = de_capsule;
					output_fn = print_capsule;
					text_input = 0;
					text_output = 1;
					known = 1;
				}
				break;
			}
			case 't' : {
				if (arg [2] == 0 || streq (arg, "-tsimp")) {
					/* Expand token definitions */
					evaluate = 1;
					expand = 1;
					known = 1;
				}
				break;
			}
			case 'c' : {
				if (arg [2] == 0 || streq (arg, "-check")) {
					/* Switch on shape checking */
					init_shapes ();
					do_check = 1;
					known = 1;
				} else if (streq (arg, "-cv")) {
					init_shapes ();
					do_check = 1;
					print_shapes = 1;
					known = 1;
				}
				break;
			}
			case 'l' : {
				if (arg [2] == 0 || streq (arg, "-lib")) {
					lib_input = 1;
					known = 1;
				}
				break;
			}
			case 'f' : {
				if (arg [2] == 0 || streq (arg, "-func")) {
					/* Check on form of input and output */
					func_input = 1;
					func_output = 1;
					known = 1;
				} else if (streq (arg, "-func_out")) {
					func_output = 1;
					known = 1;
				} else if (streq (arg, "-func_in")) {
					func_input = 1;
					known = 1;
				}
				break;
			}
			case 'n' : {
				if (strncmp (arg, "-no_", 4) == 0) {
					known = output_option (arg + 4, 0);
				}
				break;
			}
			case 'o' : {
				if (arg [2] == 0) {
					output_next = 1;
					known = 1;
				} else if (strncmp (arg, "-only_", 6) == 0) {
					known = output_option (arg + 6, 1);
				}
				break;
			}
			case 'q' : {
				if (arg [2] == 0) {
					dont_check = 1;
					known = 1;
				}
				break;
			}
			case 'u' : {
				if (arg [2] == 0 || streq (arg, "-unsorted")) {
					order_names = 0;
					known = 1;
				}
				break;
			}
			case 'v' : {
				if (arg [2] == 0) {
					verbose = 1;
					known = 1;
				}
				break;
			}
			case 'I' : {
				add_directory (arg + 2);
				known = 1;
				break;
			}
			case 'L' : {
				local_prefix = arg + 2;
				known = 1;
				break;
			}
			case 'V' : {
				if (arg [2] == 0 || streq (arg, "-version")) {
					tenapp_report_version ();
					MSG_TDF_version(VERSION_major, VERSION_minor);
					known = 1;
				}
				break;
			}
			}
			if (!known) MSG_getopt_unknown_option (arg);

		} else {
			/* Initialize input and output files */
			if (status == 0) {
				open_input (arg, 0);
			} else if (status == 1) {
				open_output (arg);
			} else {
				MSG_getopt_too_many_arguments ();
			}
			status++;
		}
    }

    /* Check on library input */
    if (lib_input && input_fn == de_capsule) input_fn = de_library;

    /* Perform the appropriate actions */
    if (status == 0) MSG_getopt_not_enough_arguments ();
    (*input_fn) ();
    if (exit_status == EXIT_SUCCESS || text_output) {
		if (expand) expand_all ();
		if (evaluate) eval_all ();
		sort_all ();
		(*output_fn) ();
    }
    return (exit_status);
}
