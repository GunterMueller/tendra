/*
 * Copyright 2011, The TenDRA Project.
 * Copyright 1997, United Kingdom Secretary of State for Defence.
 *
 * See doc/copyright/ for the full copyright terms.
 */

/*
 * Types for usable addressing modes are defined here:
 * baseoff, instore, freg, ans, where
 */

#ifndef ADDR_H
#define ADDR_H

#include <local/ash.h>

#include <construct/installtypes.h>

typedef struct {
	int lab;
	int regmove;
} makeans;

#define NOREG 100

typedef struct {
	int base;    /* base register for addressing */
	long offset; /* offset in words */
} baseoff;

typedef struct {
	baseoff b;
	bool adval; /* if true then b is itself an address value i.e not a pointer to a value */
} instore;

typedef struct {
	int fr;    /* floating point reg (actually fr*2) */
	bool dble; /* double precision */
} freg;

typedef struct {
	int *fr;
	bool dble;
} somefreg;

/* extra struct is to get over bug in MIPS cc */
typedef struct {
	int *r;
} somereg;

/*
 * This type used as union of a fixpnt reg, a float reg and an instore value
 */
typedef struct {
	/* use to discriminate the .val union type */
	enum {
		inreg,
		infreg,
		notinreg,
		insomereg,
		insomefreg
	} discrim;

	union {
		int regans; /* register number */
		freg fregans;
		instore instoreans;
		somefreg somefregans;
		somereg someregans;
	} val;
} ans;

typedef struct {
	ans answhere; /* reg or store position */
	ash ashwhere; /* size and alignment, see ashtypes.h */
} where;

typedef struct {
	long maxi;
	long mini;
	char *fmt;
} mm;

/* macros for ease of use of unions, allow 'hiding' of discriminator. */
#define regalt(x)            (x).val.regans
#define fregalt(x)           (x).val.fregans
#define insalt(x)            (x).val.instoreans
#define someregalt(x)        (x).val.someregans.r;
#define somefregalt(x)       (x).val.somefregans;

/* assign to field of union, discriminator set as appropriate */
#define setregalt(x, y)      (x).val.regans       = (y); (x).discrim = inreg
#define setfregalt(x, y)     (x).val.fregans      = (y); (x).discrim = infreg
#define setinsalt(x, y)      (x).val.instoreans   = (y); (x).discrim = notinreg
#define setsomeregalt(x, y)  (x).val.someregans.r = (y); (x).discrim = insomereg
#define setsomefregalt(x, y) (x).val.somefregans  = (y); (x).discrim = insomefreg

#endif

