/* $Id$ */

/*
 * Copyright 2002-2011, The TenDRA Project.
 * Copyright 1997, United Kingdom Secretary of State for Defence.
 *
 * See doc/copyright/ for the full copyright terms.
 */


#ifndef C_TYPES_INCLUDED
#define C_TYPES_INCLUDED


/*
 * CLEAN UP NAMESPACE
 *
 * Certain symbols need to be undefined on some systems. It is convenient to
 * do this here as this header is usually included directly after any system
 * headers.
 */

#ifdef FLOAT
#undef FLOAT
#endif

#ifdef ulong
#undef ulong
#endif


/*
 * BASIC TYPES
 *
 * All characters and strings within the program are represented in terms of
 * the type character. In particular, the type string is defined to be
 * chararacter *. ulong is defined as a macro to avoid duplicate definitions
 * in the system headers.
 */

typedef unsigned char character;
#define ulong ulong_type


/*
 * CONST TOKENS
 *
 * The tokens generated by the calculus toolset use const to indicate run-time
 * constant expression tokens. Older versions of the TenDRA C producer which
 * do not understand const tokens may suppress them by defining
 * NO_CONST_TOKEN.
 */

#ifdef NO_CONST_TOKEN
#define const
#endif


/*
 *  MAIN TYPE SYSTEM
 *
 *  The main type system is generated using the calculus tool. See c_class.alg
 *  for more details.
 */

#include "c_class.h"


/*
 * DEBUG MEMORY ALLOCATION ROUTINES
 *
 * In debug mode an alternative memory allocation scheme allowing for run-time
 * type information is implemented.
 */

#ifndef NDEBUG
#if c_class_IMPLEMENTATION
extern c_class *debug_c_class(unsigned, unsigned);
#undef GEN_c_class
#define GEN_c_class(A, B)	debug_c_class((unsigned)(A), (B))
#define TYPEID(A)		((A)[-1].ag_tag)
#define TYPEID_free		((unsigned)42)
#endif
#endif


/*
 * SYNONYMS FOR CALCULUS CONSTRUCTS
 *
 * These macros give synonyms for various constructs defined within the
 * calculus plus some other useful constructs.
 */

#define btype_struct		btype_struct_
#define btype_union		btype_union_
#define btype_enum		btype_enum_
#define cinfo_struct		cinfo_struct_
#define cinfo_union		cinfo_union_
#define destroy			destroy_c_class
#define null_tag		((unsigned)0xffff)
#define NULL_string		((string)NULL)

#if LANGUAGE_C
#define cv_lang			cv_c
#define dspec_lang		dspec_c
#define btype_lang		btype_struct
#define ERR_ALTERNATE		1
#else
#define cv_lang			cv_cpp
#define dspec_lang		dspec_cpp
#define btype_lang		btype_class
#endif


/*
 * DISTINGUISHED LINKAGE VALUES
 *
 * These values are used as distinguished linkage numbers for use in the
 * output routines. Unassigned linkage numbers are given the value LINK_NONE.
 * External linkage numbers are or-ed with the value LINK_EXTERN to
 * distinguish them from internal linkage numbers.
 */

#define LINK_NONE		((ulong)0xffffffffUL)
#define LINK_EXTERN		((ulong)0x80000000UL)
#define LINK_ZERO		((ulong)0xfffffffeUL)
#define LINK_TOKDEF		((ulong)0xfffffffdUL)


/**
 * TYPE REPRESENTING A CHECKING SCOPE
 *
 * This type is used to represent a checking scope, describing which checks
 * should be applied in a particular part of the program. It consists of an
 * array of option states - one for each option which can be set - plus a
 * pointer to the enclosing checking scope. A scope may have an associated
 * name. Also a list of all scopes is maintained.
 */

typedef unsigned char OPTION;

typedef struct opt_tag {
	OPTION *opt;
	OPTION *set;
	DECL_SPEC lnk_opt[2];
	unsigned long val_opt[1];
	struct opt_tag *prev;
	HASHID name;
	struct opt_tag *next;
} OPTIONS;


/**
 * TYPE REPRESENTING A PREPROCESSING TOKEN
 *
 * This type represents a preprocessing token. This consists of a token value,
 * corresponding to the macros defined in syntax.h, plus any associated data.
 * In some instances the space field is used to indicate that the token is
 * preceded by a white-space. The next field points to the next token.
 */

typedef struct pptok_tag {
	int tok;
	struct pptok_tag *next;
	union {
		/* Associated data */
		int sint;
		string text;
		unsigned uint;
		character buff[8];
		EXP exp;
		NAT nat;
		FLOAT flt;
		NAMESPACE ns;
		STRING strlit;
		TYPE type;
		struct {
			/* Identifier */
			HASHID hash;
			IDENTIFIER use;
		} id;
		struct {
			/* String */
			string start;
			string end;
		} str;
		struct {
			/* Macro parameter */
			HASHID hash;
			unsigned long no;
		} par;
		struct {
			/* Location */
			unsigned long line;
			PTR(POSITION)posn;
		} loc;
		struct {
			/* Token application */
			IDENTIFIER id;
			struct pptok_tag *args;
		} tok;
	} pp_data;
	unsigned long pp_space;
	OPTIONS *pp_opts;
} PPTOKEN;


/**
 * TYPE REPRESENTING A SERIES OF BITS
 *
 * A bitstream consists of an array of characters comprising the bits
 * themselves, plus the current offset (in bytes and bits) of the end of these
 * bits. A pointer to the previous bitstream is used to chain bitstreams
 * together.
*/

typedef struct bits_tag {
	string text;
	unsigned bytes;
	unsigned bits;
	unsigned size;
	FILE *file;
	gen_ptr link;
	struct bits_tag *prev;
} BITSTREAM;


/**
 * TYPE REPRESENTING A CHARACTER BUFFER
 *
 * A character buffer consists of an array of characters and a pointer to the
 * current position in the buffer.
 */

typedef struct buff_tag {
	string start;
	string posn;
	string end;
	FILE *file;
} BUFFER;

#define NULL_buff	{ NULL, NULL, NULL, NULL }


#endif
