/* $Id$ */

/*
 * Copyright 2011, The TenDRA Project.
 * Copyright 1997, United Kingdom Secretary of State for Defence.
 *
 * See doc/copyright/ for the full copyright terms.
 */

	/* **************************************************** */
	/*							*/
	/*    Basic Mapping Token Definitions for sol_sparc	*/
	/*							*/
	/* **************************************************** */




	/* C MAPPING TOKENS */


Tokdef .~pv_align = [] ALIGNMENT
alignment(integer(var_width(false,8)));


Tokdef .~min_struct_rep = [] NAT
8;


Tokdef .~bitfield_is_signed = [] BOOL
true;




Keep (
.~pv_align, .~min_struct_rep,
.~bitfield_is_signed
)
