/*
 * Copyright (c) 2002-2005 The TenDRA Project <http://www.tendra.org/>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of The TenDRA Project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific, prior written permission.
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
    		 Crown Copyright (c) 1996

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
			    VERSION INFORMATION
			    ===================

--------------------------------------------------------------------------
$Header: /u/g/release/CVSROOT/Source/src/installers/680x0/common/mach_op.h,v 1.1.1.1 1998/01/17 15:55:49 release Exp $
--------------------------------------------------------------------------
$Log: mach_op.h,v $
 * Revision 1.1.1.1  1998/01/17  15:55:49  release
 * First version to be checked into rolling release.
 *
Revision 1.2  1997/10/29 10:22:23  ma
Replaced use_alloca with has_alloca.

Revision 1.1.1.1  1997/10/13 12:42:55  ma
First version.

Revision 1.3  1997/09/25 06:45:17  ma
All general_proc tests passed

Revision 1.2  1997/06/18 10:09:39  ma
Checking in before merging with Input Baseline changes.

Revision 1.1.1.1  1997/03/14 07:50:15  ma
Imported from DRA

 * Revision 1.2  1996/09/20  13:51:40  john
 * *** empty log message ***
 *
 * Revision 1.1.1.1  1996/09/20  10:56:55  john
 *
 * Revision 1.1.1.1  1996/03/26  15:45:15  john
 *
 * Revision 1.2  94/02/21  16:00:39  16:00:39  ra (Robert Andrews)
 * Declare functions with no arguments using ( void ).
 *
 * Revision 1.1  93/02/22  17:16:09  17:16:09  ra (Robert Andrews)
 * Initial revision
 *
--------------------------------------------------------------------------
*/


#ifndef MACH_OP_INCLUDED
#define MACH_OP_INCLUDED

extern bool check_op(mach_op *, int);
extern bool equal_op(mach_op *, mach_op *);
extern void free_mach_op(mach_op *);
extern mach_op * new_mach_op(void);

extern mach_op * make_bitfield_op(mach_op *, int, int);
extern mach_op * make_dec_sp(void);
extern mach_op * make_extern(char *, long);
extern mach_op * make_extern_data(char *, long);
extern mach_op * make_extern_ind(char *, long);
extern mach_op * make_float_data(flt *);
extern mach_op * make_hex_data(long);
extern mach_op * make_hex_value(long);
extern mach_op * make_inc_sp(void);
extern mach_op * make_ind_rel_ap(long, long);
extern mach_op * make_index_op(mach_op *, mach_op *, int);
extern mach_op * make_indirect(int, long);
extern mach_op * make_int_data(long);
extern mach_op * make_lab(long, long);
extern mach_op * make_lab_data(long, long);
extern mach_op * make_lab_diff(long, long);
extern mach_op * make_lab_ind(long, long);
extern mach_op * make_postinc(int);
extern mach_op * make_reg_index(int, int, long, int);
extern mach_op * make_reg_pair(int, int);
extern mach_op * make_register(int);
extern mach_op * make_rel_ap(long);

#ifndef tdf3
extern mach_op * make_rel_ap2(long);
extern mach_op * make_rel_sp(long);
extern mach_op * make_ind_rel_ap2(long, long);
extern mach_op * make_ind_rel_ap3(long, long);
extern mach_op * make_predec(int);
extern mach_op * make_ldisp(long);
#endif

extern mach_op * make_special(char *);
extern mach_op * make_special_data(char *);
extern mach_op * make_value(long);

extern long special_no;
extern char *special_str;

extern int tmp_reg_prefer;
extern int tmp_reg_status;
extern void avoid_tmp_reg(int);
extern int next_tmp_reg(void);
extern int tmp_reg(int, mach_op *);

#endif
