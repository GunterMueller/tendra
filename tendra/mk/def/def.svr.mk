# $TenDRA$
#
# Binary / variable definitions for the SVR operating system.

# The execution startup routines.
# crt is for normal support, crti for _init, crtn for _fini.
# gcrt is for profiling support (gprof).
# mcrt is for profiling support (prof).


# Libraries

LIB_CRT0?=	
LIB_CRT1?=	/usr/ccs/lib/crt1.o
LIB_CRTI?=	/usr/ccs/lib/crti.o
LIB_CRTN?=	/usr/ccs/lib/crtn.o
LIB_GCRT0?=	
LIB_GCRT1?=	
LIB_MCRT0?=	
LIB_MCRT1?=	/usr/ccs/lib/mcrt1.o
LIB_PCRT0?=	
LIB_VALUES_XA?=	/usr/ccs/lib/values-Xa.o
LIB_VALUES_XC?=	/usr/ccs/lib/values-Xc.o


# Arguments

ARGS_CP?=	-v
ARGS_GUNZIP?=	-f
ARGS_GZIP?=	-nf -9
ARGS_MKDIR?=	-p


# Binaries

BIN_CC?=		
BIN_AR?=		
BIN_AS?=	/usr/bin/as	
BIN_AS1?=		
BIN_AWK?=		
BIN_BASENAME?=		
BIN_CAT?=		
BIN_CHGRP?=		
BIN_CHMOD?=		
BIN_CHOWN?=		
BIN_CP?=		
BIN_CUT?=		
BIN_DC?=		
BIN_DIRNAME?=		
BIN_ECHO?=		
BIN_EGREP?=		
BIN_FALSE?=		
BIN_FILE?=		
BIN_FIND?=		
BIN_GREP?=		
BIN_GTAR?=		
BIN_GUNZIP?=		
BIN_GZCAT?=		
BIN_GZIP?=		
BIN_HEAD?=		
BIN_ID?=		
BIN_INSTALL?=		
BIN_LD?=		/usr/bin/ld
BIN_LDCONFIG?=		
BIN_LN?=		
BIN_LS?=		
BIN_MKDIR?=		
BIN_MTREE?=		
BIN_MV?=		
BIN_PATCH?=		
BIN_PAX?=		
BIN_PERL?=		
BIN_RANLIB?=		
BIN_RM?=		
BIN_RMDIR?=		
BIN_SED?=		
BIN_SETENV?=		
BIN_SH?=		
BIN_SORT?=		
BIN_SU?=		
BIN_TAIL?=		
BIN_TEST?=		
BIN_TIME?=		
BIN_TOUCH?=		
BIN_TR?=		
BIN_TRUE?=		
BIN_TYPE?=		
BIN_WC?=		
BIN_XARGS?=		
