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
 * lexer.h - SID lexical analyser.
 *
 * See the file "lexer.c" for more information.
 */

#ifndef H_LEXER
#define H_LEXER

#include <exds/common.h>
#include <exds/exception.h>
#include <exds/dstring.h>
#include <exds/istream.h>
#include  "lexi_lexer.h"

struct lexi_state lexi_current_state ;

/*
 * Note:
 *
 * It is important that any changes to this enumerated type declaration are
 * reflected in the "parser.sid" file.
 */
typedef enum {
	LEXER_TOK_BLT_HTYPES,
	LEXER_TOK_BLT_HTERMINALS,
	LEXER_TOK_BLT_HPRODUCTIONS,
	LEXER_TOK_BLT_HENTRY,
	LEXER_TOK_IDENTIFIER,
	LEXER_TOK_TYPEMARK,
	LEXER_TOK_OPEN_HTUPLE,
	LEXER_TOK_CLOSE_HTUPLE,
	LEXER_TOK_ARROW,
	LEXER_TOK_TERMINATOR,
	LEXER_TOK_BEGIN_HACTION,
	LEXER_TOK_END_HACTION,
	LEXER_TOK_DEFINE,
	LEXER_TOK_BEGIN_HRULE,
	LEXER_TOK_END_HRULE,
	LEXER_TOK_ALT_HSEP,
	LEXER_TOK_HANDLER_HSEP,
	LEXER_TOK_EMPTY,
	LEXER_TOK_PRED_HRESULT,
	LEXER_TOK_IGNORE,
	LEXER_TOK_SCOPEMARK,
	LEXER_TOK_BEGIN_HSCOPE,
	LEXER_TOK_END_HSCOPE,
	LEXER_TOK_SEPARATOR,
	LEXER_TOK_REFERENCE,
	LEXER_TOK_EOF,
	LEXER_TOK_ERROR
} LexerTokenT;

typedef struct LexT {
	LexerTokenT t;
	union {
		NStringT string;
	} u;
} LexT;

typedef struct LexerStreamT {
	IStreamT istream;
	LexT token;
	LexerTokenT saved_terminal;
} LexerStreamT;

void	lexer_init(LexerStreamT *, IStreamT *);
void	lexer_close(LexerStreamT *);
char	*lexer_stream_name(LexerStreamT *);
unsigned	lexer_stream_line(LexerStreamT *);
LexerTokenT	lexer_get_terminal(LexerStreamT *);
void	lexer_next_token(LexerStreamT *);
NStringT	*lexer_string_value(LexerStreamT *);
void	lexer_save_terminal(LexerStreamT *, LexerTokenT);
void	lexer_restore_terminal(LexerStreamT *);


/*
 * Interfaces for the generated lexer.
 */
int lexi_getchar(void);
int read_identifier(int c);
int read_builtin(int c);
int lexi_unknown_token(int c0);

/* XXX remove once lexi provides opaque pointers */
LexerStreamT *lexer_stream;

#endif /* !defined (H_LEXER) */