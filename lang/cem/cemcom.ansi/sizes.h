/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */
/* VARIOUS TARGET MACHINE SIZE DESCRIPTORS */

#include "nocross.h"
#include "target_sizes.h"

#ifndef NOCROSS
extern arith
	short_size, word_size, dword_size, int_size, long_size,
	float_size, double_size, lngdbl_size,
	pointer_size;

extern arith max_int, max_unsigned;	/* cstoper.c	*/
#else NOCROSS
#define short_size	(SZ_SHORT)
#define word_size	(SZ_WORD)
#define dword_size	(2*SZ_WORD)
#define int_size	(SZ_INT)
#define long_size	(SZ_LONG)
#define float_size	(SZ_FLOAT)
#define double_size	(SZ_DOUBLE)
#define	lngdbl_size	(SZ_LNGDBL)
#define pointer_size	(SZ_POINTER)

#if int_size == 2
#define max_int		((arith)32767)
#define max_unsigned	((arith)65535)
#else /* int_size == 4 */
#define max_int		((arith)2147483647)
#define max_unsigned	((arith)4294967295)
#endif
#endif NOCROSS

