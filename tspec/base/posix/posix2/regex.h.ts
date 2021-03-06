# Copyright 2002-2011, The TenDRA Project.
# Copyright 1997, United Kingdom Secretary of State for Defence.
#
# See doc/copyright/ for the full copyright terms.


+USE "c/c89", "stddef.h.ts", "size_t" (!?) ;

+CONST int REG_EXTENDED, REG_ICASE, REG_NOSUB, REG_NEWLINE, REG_NOTBOL ;
+CONST int REG_NOTEOL, REG_NOMATCH, REG_BADPAT, REG_ECOLLATE, REG_ECTYPE ;
+CONST int REG_EESCAPE, REG_ESUBREG, REG_EBRACK, REG_EPAREN, REG_EBRACE ;
+CONST int REG_BADBR, REG_ERANGE, REG_ESPACE, REG_BADRPT, REG_ENOSYS ;

+FIELD ( struct ) regex_t {
    size_t re_nsub ;
} ;

+TYPE ( signed ) regoff_t ;

+FIELD ( struct ) regmatch_t {
    regoff_t rm_so ;
    regoff_t rm_eo ;
} ;

+FUNC int regcomp ( regex_t *, const char *, int ) ;
+FUNC int regexec ( const regex_t *, const char *, size_t, regmatch_t *, int ) ;
+FUNC size_t regerror ( int, const regex_t *, char *, size_t ) ;
+FUNC void regfree ( regex_t * ) ;
