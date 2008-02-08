# $TenDRA: tendra.install.mk 2151 2005-10-17 11:14:32Z stefanf $


# Various settings.
.if defined(USE_PATH_DEV)
INSTALL_PREFIX=		${INSTALL_DIR}/${VER_TENDRA}/${MACH_OS}/${MACH_VERS}/${MACH_CPU}
.else
INSTALL_PREFIX=		${INSTALL_DIR}
.endif

MAN_INSTALL_DIR?=	${PREFIX}/man

ARGS_CHMOD_BIN=		755
ARGS_CHMOD_DATA=	644


# Target handling.
.if defined(INSTALL_MACHINE)
INSTALL_TARGETS+=	install-machine
INSTALL_SUB+=		lib/machines ${MAKEDIR:C/^/lib\/machines\//}
.endif

.if defined(PROG)
INSTALL_TARGETS+=	install-bin
INSTALL_SUB+=		bin
.endif

.if defined(ADA_PROG) && !defined(DONT_INSTALL)
PROG=                   ${ADA_PROG}
INSTALL_TARGETS+=	install-bin
INSTALL_SUB+=		bin
.endif

.if defined(MKERR_SRC)
INSTALL_TARGETS+=	install-mkerr
INSTALL_SUB+=		bin
.endif

.if defined(STARTUP)
INSTALL_TARGETS+=	install-startup
.endif

.if defined(API)
INSTALL_TARGETS+=	install-api
.endif

.if defined(INSTALL_ENV)
INSTALL_TARGETS+=	install-env
INSTALL_SUB=		env
.endif

.if defined(SHLIB)
INSTALL_TARGETS+=	install-shlib
INSTALL_SUB+=		lib/sys
.endif

.if defined(INSTALL_LIB) || defined(INSTALL_LIB_SYS)
INSTALL_TARGETS+=	install-lib
INSTALL_SUB+=		lib lib/sys
.endif

.if defined(TL) && !defined(TL_DIAG)
INSTALL_TARGETS+=	install-tl
INSTALL_SUB+=		lib
.endif

.if defined(TL) && defined(TL_DIAG)
INSTALL_TARGETS+=	install-tl
INSTALL_SUB+=		lib/diag
.endif

.if defined(WRAPPERS)
INSTALL_TARGETS+=	install-wrapper
INSTALL_SUBBIN=		bin
WRAPPERS+=		tcc.sh tchk.sh
.endif

.if (defined(MAN1) || defined(MAN5)) && ${MAN_INSTALL} == "yes"
INSTALL_TARGETS+=	install-man
INSTALL_SUBMAN=		man1 man5
.endif


# Our main target.
install-all: install-dir ${INSTALL_TARGETS}


# Create our directories.
install-dir:
.for i in ${INSTALL_SUB}
	${BIN_MKDIR} ${ARGS_MKDIR} ${INSTALL_PREFIX}/$i
.endfor


# lib/machines: our machine dependent files.
install-machine:
.for i in ${DATA_INCLUDE}
	${BIN_CP} ${.OBJDIR}/${i} \
		${INSTALL_PREFIX}/lib/machines/${i:C/(.*)\/.*$/\1/}
	${BIN_CHMOD} ${ARGS_CHMOD_DATA} ${INSTALL_PREFIX}/lib/machines/${i}
.endfor
.for i in ${DATA_STARTUP}
	${BIN_CP} ${.OBJDIR}/${i} ${INSTALL_PREFIX}/lib/machines/startup
	${BIN_CHMOD} ${ARGS_CHMOD_DATA} ${INSTALL_PREFIX}/lib/machines/${i}
.endfor


# lib/startup: our startup files for compilation modes.
install-startup:
	${BIN_MKDIR} ${ARGS_MKDIR} ${INSTALL_PREFIX}/lib/startup/${STARTUP}
.for i in ${DATA}
	${BIN_CP} ${INSTALL_FLAGS} ${.OBJDIR}/${i} \
		${INSTALL_PREFIX}/lib/startup/${STARTUP}/${i}
	${BIN_CHMOD} ${ARGS_CHMOD_DATA} \
		${INSTALL_PREFIX}/lib/startup/${STARTUP}/${i}
.endfor


# lib/apis: our api files.
install-api:
	@echo HELLO ${OBJ_DIR}.api/Makefile
	@echo dirs: ${DIRS}
	@echo headers: ${HEADERS}
	for i in ${DIRS}; do \
		${BIN_MKDIR} -p ${INSTALL_PREFIX}/include/$${i}; \
	done
	for i in ${HEADERS}; do \
	${BIN_CP} ${OBJ_PREFIX}/lib/apis/$${i} ${INSTALL_PREFIX}/include/$${i}; \
	${BIN_CHMOD} ${ARGS_CHMOD_DATA} ${INSTALL_PREFIX}/include/$${i}; \
	done

# lib/apis/lib lib/apis/lib/diag: our tl files.
install-tl:
	${BIN_CP} ${.OBJDIR}/${TL} ${INSTALL_PREFIX}/${INSTALL_SUB}/${TL}
	${BIN_CHMOD} ${ARGS_CHMOD_DATA} ${INSTALL_PREFIX}/${INSTALL_SUB}/${TL}



# lib/env: our environment files.
install-env:
.for i in ${DATA_COMMON} ${DATA} ${DATA_DEFAULT}
	${BIN_CP} ${.OBJDIR}/${i} ${INSTALL_PREFIX}/env; \
		${BIN_CHMOD} ${ARGS_CHMOD_DATA} \
		${INSTALL_PREFIX}/${INSTALL_SUB}/${i}
.endfor


# installers/*, tools/*, utilities/*: our binary files.
install-bin:
	${BIN_CP} ${INSTALL_FLAGS} \
		${.OBJDIR}/${PROG} ${INSTALL_PREFIX}/${INSTALL_SUB}
	${BIN_CHMOD} ${ARGS_CHMOD_BIN} ${INSTALL_PREFIX}/bin/${PROG}


# utilities/mkerr
install-mkerr:
	${BIN_CP} ${INSTALL_FLAGS} \
		${.OBJDIR}/${MKERR_SRC:R} ${INSTALL_PREFIX}/${INSTALL_SUB}
	${BIN_CHMOD} ${ARGS_CHMOD_BIN} ${INSTALL_PREFIX}/bin/${MKERR_SRC:R}


# lib/sys: our shared libraries.
install-shlib:
	${BIN_CP} ${.OBJDIR}/${SHLIB} ${INSTALL_PREFIX}/${INSTALL_SUB}
	${BIN_CHMOD} ${ARGS_CHMOD_DATA} ${INSTALL_PREFIX}/lib/sys/${SHLIB}


# lib/machines/BUILD_OS/MACH_CPU/tokens, home of target_tok.tl and crtp_n.o
install-lib:
.for i in ${INSTALL_LIB}
	${BIN_CP} ${.OBJDIR}/${i} ${INSTALL_PREFIX}/lib
	${BIN_CHMOD} ${ARGS_CHMOD_DATA} ${INSTALL_PREFIX}/lib/${i}
.endfor
.for i in ${INSTALL_LIB_SYS}
	${BIN_CP} ${.OBJDIR}/${i} ${INSTALL_PREFIX}/lib/sys
	${BIN_CHMOD} ${ARGS_CHMOD_DATA} ${INSTALL_PREFIX}/lib/sys/${i}
.endfor


# wrappers: our sh wrapper scripts.
install-wrapper:
	${BIN_MKDIR} ${ARGS_MKDIR} ${PREFIX}/${INSTALL_SUBBIN}
.for i in ${WRAPPERS}
	${BIN_CP} ${.OBJDIR}/${i:R} ${PREFIX}/${INSTALL_SUBBIN}
	${BIN_CHMOD} ${ARGS_CHMOD_BIN} ${PREFIX}/${INSTALL_SUBBIN}/${i:R}
.endfor


# man: our manpages.
install-man:
.for i in ${INSTALL_SUBMAN}
	${BIN_MKDIR} ${ARGS_MKDIR} ${MAN_INSTALL_DIR}/man${i:C/.*(.)$/\1/}
.endfor
.for i in ${MAN1} ${MAN5}
	${BIN_CP} ${.OBJDIR}/${i} ${MAN_INSTALL_DIR}/man${i:C/.*(.)$/\1/}/${i}
	${BIN_CHMOD} ${ARGS_CHMOD_DATA} ${MAN_INSTALL_DIR}/man${i:C/.*(.)$/\1/}/${i}
.if ${MAN_COMPRESS} == "yes"
	${BIN_GZIP} ${ARGS_GZIP} ${MAN_INSTALL_DIR}/man${i:C/.*(.)$/\1/}/${i}
.endif
.endfor