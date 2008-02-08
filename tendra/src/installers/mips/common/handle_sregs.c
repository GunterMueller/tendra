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


/* handle_sregs.c
 *	handle_sregs dumps and recovers the s-registers;
 *	the registers to be dumped are expressed as bits in a long eg
 *	bit 31 => reg 31 must be dumped.
 *	They are called in the dump_tag part of make_code.
 
****************************************************************/



#include "config.h"
#include "mips_ins.h"
#include "inst_fmt.h"
#include "addrtypes.h"
#include "frames.h"
#include "basicread.h"
#include "handle_sregs.h"

extern long frame_size;
long  dumpstart;		/* where to start dumping fixeds */
long  fldumpstart;		/* where to start dumping floats */

long  fixdump;			/* all the fixeds to be dumped */
long  floatdump;		/* all the floats to be dumped */

long  fixdone;			/* the fixeds which have already been
						 *				   dumped */
long  fltdone;			/* the floats which have already been
						 *				   dumped */

/* dump_sreg, dump_sfreg & recover .. etc not used at present!! */

baseoff
dump_baseoff(long ds)
{	baseoff b;
 b.base = (Has_vcallees)?local_reg:(29+Has_fp);
 b.offset = (Has_vcallees)?(ds - (frame_size>>3)):
	 (Has_fp)?(ds - ((frame_size+callee_size)>>3)):ds;
 return b;
}

void
dump_sreg(int n)
{
	unsigned long maskn = 1 << n;
	unsigned long mask = 1 << 31;
	int i;
	baseoff b;
	b = dump_baseoff(dumpstart);
	Assert(maskn & fixdump);
	
	for (i = 31; ; i--) {
		if (mask & fixdump) b.offset -=4;
		if (i== n) {
       		ls_ins(i_sw, n, b);
       		fixdone |= maskn;
       		return;
		}
		mask >>= 1;
	}
}

void
dump_sfreg(int n)
{
	unsigned long maskn = 3 << n;
	unsigned long mask = 3 << 30;
	int i;
	baseoff b;
	b = dump_baseoff(fldumpstart);
	Assert(maskn & floatdump);
	
	for (i =30; ; i-=2) {
		if (mask & floatdump) b.offset -=8;
		if (i==n) {
      	   	lsfp_ins(i_s_d, i, b);
      	   	fltdone |= maskn;
      	   	return;
		}
		mask >>= 2;
	}
}

void
recover_sreg(int n)
{
	unsigned long maskn = 1 << n;
	unsigned long mask = 1 << 31;
	int i;
	baseoff b;
	b = dump_baseoff(dumpstart);
	Assert(maskn & fixdump);
	
	for (i = 31; ; i--) {
		if (mask & fixdump) b.offset -=4;
		if (i== n) {
       		ls_ins(i_lw, n, b);
       		fixdone &= ~maskn;
       		return;
		}
		mask >>= 1;
	}
}

void
recover_sfreg(int n)
{
	unsigned long maskn = 3 << n;
	unsigned long mask = 3 << 30;
	int i;
	baseoff b;
	b = dump_baseoff(fldumpstart);
	Assert(maskn & floatdump);
	
	for (i =30; ; i-=2) {
		if (mask & floatdump) b.offset -=8;
		if (i==n) {
      	   	lsfp_ins(i_l_d, i, b);
      	   	fltdone &= ~maskn;
      	   	return;
		}
		mask >>= 2;
	}
}

void
dump_sregs(long fdi, long fldi)
{
	unsigned long mask = 1 << 31;
	long fd = fdi;
	long fld = fldi;
	baseoff b;
	int   i;
	b = dump_baseoff(dumpstart);
	
	for (i = 31; fd != 0; i--) {
		if (mask & fixdump) {
			b.offset -= 4;
			if (mask & fd) {
				ls_ins (i_sw, i, b);
				fd &= ~mask;
			}
		}
		mask = mask >> 1;
	}
	
	b = dump_baseoff(fldumpstart);
	mask = 3 << 30;
	for (i = 30; fld != 0; i -= 2) {
		if ((mask & floatdump) == mask) {
			b.offset -= 8;
			if (mask & fld) {
				lsfp_ins (i_s_d, i, b);
				fld &= ~mask;
			}
		}
		mask = mask >> 2;
	}
	fixdone |= fdi;
	fltdone |= fldi;
}



void
restore_sregs(long fd, long fld)
{
	unsigned long mask = 1 << 31;
	baseoff b;
	int   i;
	b = dump_baseoff(dumpstart);
	
	for (i = 31; fd != 0; i--) {
		if (mask & fixdump) {
			b.offset -= 4;
			if (mask & fd) {
				ls_ins (i_lw, i, b);
				fd &= ~mask;
			}
		}
		mask = mask >> 1;
	}
	
	b = dump_baseoff(fldumpstart);
	mask = 3 << 30;
	for (i = 30; fld != 0; i -= 2) {
		if ((mask & floatdump) == mask) {
			b.offset-= 8;
			if (mask & fld) {
				lsfp_ins (i_l_d, i, b);
				fld &= ~mask;
			}
		}
		mask = mask >> 2;
	}
}

void
dump_all_sregs()
{
	unsigned long mask = 1 << 31;
	long fd = fixdump;
	long fld = floatdump;
	baseoff b;
	int   i;
	
	b = dump_baseoff(dumpstart);
	for (i = 31; fd != 0; i--) {
		if (mask & fixdump) {
			b.offset-= 4;
			ls_ins (i_sw, i, b);
			fd &= ~mask;
		}
		mask = mask >> 1;
	}
	
	b = dump_baseoff(fldumpstart);
	mask = 3 << 30;
	for (i = 30; fld != 0; i -= 2) {
		if ((mask & floatdump) == mask) {
			b.offset-= 8;
			lsfp_ins (i_s_d, i, b);
			fld &= ~mask;
		}
		mask = mask >> 2;
	}
	fixdone = fixdump;
	fltdone = floatdump;
}