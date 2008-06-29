/*
 * Copyright (c) 2002-2005 The TenDRA Project <http://www.tendra.org/>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *	this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *	this list of conditions and the following disclaimer in the documentation
 *	and/or other materials provided with the distribution.
 * 3. Neither the name of The TenDRA Project nor the names of its contributors
 *	may be used to endorse or promote products derived from this software
 *	without specific, prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS
 * IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $Id$
 */
/*
			 Crown Copyright (c) 1997

	This TenDRA(r) Computer Program is subject to Copyright
	owned by the United Kingdom Secretary of State for Defence
	acting through the Defence Evaluation and Research Agency
	(DERA).  It is made available to Recipients with a
	royalty-free licence for its use, reproduction, transfer
	to other parties and amendment for any purpose not excluding
	product development provided that any such use et cetera
	shall be deemed to be acceptance of the following conditions:-

		(1) Its Recipients shall ensure that this Notice is
		reproduced upon any copies or amended versions of it;

		(2) Any amended version of it shall be clearly marked to
		show both the nature of and the organisation responsible
		for the relevant amendment or amendments;

		(3) Its onward transfer from a recipient to another
		party shall be deemed to be that party's acceptance of
		these conditions;

		(4) DERA gives no warranty or assurance as to its
		quality or suitability for any purpose and DERA accepts
		no liability whatsoever in relation to any use to which
		it may be put.
*/

/*
 * lexer.c - SID lexical analyser.
 *
 * This file implements the support functions for the SID lexical analyser to
 * be called from the code generated by lexi. One of the reasons this has been
 * ported to use lexi is to provide an example use from a different perspective
 * (as Sid has a well-abstracted stream system) which will help clarify features
 * during lexi's forthcoming API change - particularly passing opaque pointers.
 *
 * Any changes to the syntax of SID identifiers should be made to the language
 * specifc lexical analyser files as well.
 */

#include <assert.h>
#include <ctype.h>

#include "shared/check/check.h"
#include "lexer.h"
#include "gen-errors.h"

struct lexi_state lexi_current_state ;

/*
 * These are global until lexi supports passing opaque pointers, which is
 * planned during its forthcoming API change.
 */
LexerStreamT *lexer_stream;
LexT *lexer_token;

/*
 * Externally visible functions
 */

void
lexer_init(LexerStreamT *stream, IStreamT *istream)
{
	istream_assign(&stream->istream, istream);
	lexer_stream = stream;
	lexer_token = &stream->token;
	lexi_init(&lexi_current_state);
	lexer_next_token(stream);
}

void
lexer_close(LexerStreamT *stream)
{
	istream_close(&stream->istream);
}

char *
lexer_stream_name(LexerStreamT *stream)
{
	return istream_name(&stream->istream);
}

unsigned
lexer_stream_line(LexerStreamT *stream)
{
	return istream_line(&stream->istream);
}

LexerTokenT
lexer_get_terminal(LexerStreamT *stream)
{
	return stream->token.t;
}

void
lexer_next_token(LexerStreamT *stream)
{
	lexer_token->t = lexi_read_token(&lexi_current_state);
	stream->token = *lexer_token;
}

NStringT *
lexer_string_value(LexerStreamT *stream)
{
	assert(stream->token.t == LEXER_TOK_IDENTIFIER);
	return &stream->token.u.string;
}

void
lexer_save_terminal(LexerStreamT *stream, LexerTokenT error_terminal)
{
	assert(stream->token.t != error_terminal);
	stream->saved_terminal = stream->token.t;
	stream->token.t        = error_terminal;
}

void
lexer_restore_terminal(LexerStreamT *stream)
{
	stream->token.t = stream->saved_terminal;
}


/*
 * Lexi interface wrappers.
 */

int
lexi_getchar(void)
{
	char c;
	IStreamT *istream;

	istream = &lexer_stream->istream;
	assert(istream);

	if (!istream_read_char(istream, &c)) {
		return LEXI_EOF;
	}

	return c;
}

/* TODO really we should add this ability to istream itself */
/*
Using the unreadchr provided by the lexi interface
static void
unread_char(int c)
{
assert(!lexer_unreadchar);

lexer_unreadchar = c;
}
 */

int
read_identifier(int c)
{
	IStreamT *istream;
	DStringT dstring;

	istream = &lexer_stream->istream;

	dstring_init(&dstring);
	for (;;) {
		dstring_append_char(&dstring, c);

		c = lexi_readchar(&lexi_current_state);
		if (c == LEXI_EOF) {
			E_eof_in_identifier(istream);
			return LEXER_TOK_EOF;
		}

		if (!lexi_group(lexi_group_identbody, c)) {
			lexi_push(&lexi_current_state, c);
			break;
		}
	}

	lexer_token->t = LEXER_TOK_IDENTIFIER;
	dstring_to_nstring(&dstring, &(lexer_token->u.string));
	dstring_destroy(&dstring);

	return lexer_token->t;
}

int
read_builtin(int c)
{
	IStreamT *istream;
	DStringT dstring;
	char *cstring;

	istream = &lexer_stream->istream;

	dstring_init(&dstring);
	do {
		c = lexi_readchar(&lexi_current_state);
		if (c == LEXI_EOF) {
			E_eof_in_builtin(istream);
			return LEXER_TOK_EOF;
		}

		if (c == '%') {
			break;
		}

		dstring_append_char(&dstring, c);
	} while (lexi_group(lexi_group_builtin, c));

	if (c != '%') {
		E_illegal_character_in_builtin(istream, c);

		/* abandon this token and move on */
		return lexi_read_token(&lexi_current_state);
	}

	cstring = dstring_destroy_to_cstring(&dstring);
	if (cstring_ci_equal(cstring, "types")) {
		lexer_token->t = LEXER_TOK_BLT_HTYPES;
	} else if (cstring_ci_equal(cstring, "terminals")) {
		lexer_token->t = LEXER_TOK_BLT_HTERMINALS;
	} else if (cstring_ci_equal(cstring, "productions")) {
		lexer_token->t = LEXER_TOK_BLT_HPRODUCTIONS;
	} else if (cstring_ci_equal(cstring, "entry")) {
		lexer_token->t = LEXER_TOK_BLT_HENTRY;
	} else {
		E_unknown_builtin(istream, cstring);
		UNREACHED;
	}

	DEALLOCATE(cstring);
	return lexer_token->t;
}

int
lexi_unknown_token(int c)
{
	IStreamT *istream;

	istream = &lexer_stream->istream;

	if (c == LEXI_EOF) {
		return LEXER_TOK_EOF;
	}

	E_illegal_character(istream, c);

	/*
	 * We try to continue regardless as convenience to the user so that
	 * any further errors might possibly be identified.
	 */
	return lexi_read_token(&lexi_current_state);
}

