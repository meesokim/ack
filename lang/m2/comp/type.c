/*	T Y P E   D E F I N I T I O N   M E C H A N I S M	 */

#ifndef NORCSID
static char *RcsId = "$Header$";
#endif

#include	"target_sizes.h"
#include	"debug.h"
#include	"maxset.h"

#include	<assert.h>
#include	<alloc.h>
#include	<em_arith.h>
#include	<em_label.h>
#include	<em_code.h>

#include	"def.h"
#include	"type.h"
#include	"idf.h"
#include	"LLlex.h"
#include	"node.h"
#include	"const.h"
#include	"scope.h"
#include	"walk.h"

int
	word_align = AL_WORD,
	int_align = AL_INT,
	long_align = AL_LONG,
	float_align = AL_FLOAT,
	double_align = AL_DOUBLE,
	pointer_align = AL_POINTER,
	struct_align = AL_STRUCT;

arith
	word_size = SZ_WORD,
	dword_size = 2 * SZ_WORD,
	int_size = SZ_INT,
	long_size = SZ_LONG,
	float_size = SZ_FLOAT,
	double_size = SZ_DOUBLE,
	pointer_size = SZ_POINTER;

struct type
	*bool_type,
	*char_type,
	*int_type,
	*card_type,
	*longint_type,
	*real_type,
	*longreal_type,
	*word_type,
	*address_type,
	*intorcard_type,
	*bitset_type,
	*std_type,
	*error_type;

struct paramlist *h_paramlist;
#ifdef DEBUG
int	cnt_paramlist;
#endif

struct type *h_type;
#ifdef DEBUG
int	cnt_type;
#endif

struct type *
create_type(fund)
	int fund;
{
	/*	A brand new struct type is created, and its tp_fund set
		to fund.
	*/
	register struct type *ntp = new_type();

	clear((char *)ntp, sizeof(struct type));
	ntp->tp_fund = fund;

	return ntp;
}

struct type *
construct_type(fund, tp)
	int fund;
	register struct type *tp;
{
	/*	fund must be a type constructor.
		The pointer to the constructed type is returned.
	*/
	register struct type *dtp = create_type(fund);

	switch (fund)	{
	case T_PROCEDURE:
	case T_POINTER:
	case T_HIDDEN:
		dtp->tp_align = pointer_align;
		dtp->tp_size = pointer_size;
		break;

	case T_SET:
		dtp->tp_align = word_align;
		break;

	case T_ARRAY:
		dtp->tp_align = tp->tp_align;
		break;

	case T_SUBRANGE:
		dtp->tp_align = tp->tp_align;
		dtp->tp_size = tp->tp_size;
		break;

	default:
		crash("funny type constructor");
	}

	dtp->next = tp;
	return dtp;
}

arith
align(pos, al)
	arith pos;
	int al;
{
	return ((pos + al - 1) / al) * al;
}

struct type *
standard_type(fund, align, size)
	int fund;
	int align;
	arith size;
{
	register struct type *tp = create_type(fund);

	tp->tp_align = align;
	tp->tp_size = size;

	return tp;
}

InitTypes()
{
	/*	Initialize the predefined types
	*/
	register struct type *tp;

	/* first, do some checking
	*/
	if (int_size != word_size) {
		fatal("integer size not equal to word size");
	}

	if (int_size != pointer_size) {
		fatal("cardinal size not equal to pointer size");
	}

	if (long_size < int_size || long_size % word_size != 0) {
		fatal("illegal long integer size");
	}

	if (double_size < float_size) {
		fatal("long real size smaller than real size");
	}

	if (!pointer_size || pointer_size % word_size != 0) {
		fatal("illegal pointer size");
	}

	/* character type
	*/
	char_type = standard_type(T_CHAR, 1, (arith) 1);
	char_type->enm_ncst = 256;
	
	/* boolean type
	*/
	bool_type = standard_type(T_ENUMERATION, 1, (arith) 1);
	bool_type->enm_ncst = 2;

	/* integer types, also a "intorcard", for integer constants between
	   0 and MAX(INTEGER)
	*/
	int_type = standard_type(T_INTEGER, int_align, int_size);
	longint_type = standard_type(T_INTEGER, long_align, long_size);
	card_type = standard_type(T_CARDINAL, int_align, int_size);
	intorcard_type = standard_type(T_INTORCARD, int_align, int_size);

	/* floating types
	*/
	real_type = standard_type(T_REAL, float_align, float_size);
	longreal_type = standard_type(T_REAL, double_align, double_size);

	/* SYSTEM types
	*/
	word_type = standard_type(T_WORD, word_align, word_size);
	address_type = construct_type(T_POINTER, word_type);

	/* create BITSET type
	   TYPE BITSET = SET OF [0..W-1];
	   The subrange is a subrange of type cardinal, because the lower bound
	   is a non-negative integer (See Rep. 6.3)
	*/
	tp = construct_type(T_SUBRANGE, card_type);
	tp->sub_lb = 0;
	tp->sub_ub = word_size * 8 - 1;
	bitset_type = set_type(tp);

	/* a unique type for standard procedures and functions
	*/
	std_type = construct_type(T_PROCEDURE, NULLTYPE);

	/* a unique type indicating an error
	*/
	error_type = standard_type(T_CHAR, 1, (arith) 1);
}

chk_basesubrange(tp, base)
	register struct type *tp, *base;
{
	/*	A subrange had a specified base. Check that the bases conform.
	*/

	if (base->tp_fund == T_SUBRANGE) {
		/* Check that the bounds of "tp" fall within the range
		   of "base".
		*/
		if (base->sub_lb > tp->sub_lb || base->sub_ub < tp->sub_ub) {
			error("Base type has insufficient range");
		}
		base = BaseType(base);
	}

	if (base->tp_fund & (T_ENUMERATION|T_CHAR)) {
		if (BaseType(tp) != base) {
			error("Specified base does not conform");
		}
	}
	else if (base != card_type && base != int_type) {
		error("Illegal base for a subrange");
	}
	else if (base == int_type && BaseType(tp) == card_type &&
		 (tp->sub_ub > max_int || tp->sub_ub < 0)) {
		error("Upperbound to large for type INTEGER");
	}
	else if (base != BaseType(tp) && base != int_type) {
		error("Specified base does not conform");
	}

	tp->next = base;
	tp->tp_size = base->tp_size;
	tp->tp_align = base->tp_align;
}

struct type *
subr_type(lb, ub)
	struct node *lb, *ub;
{
	/*	Construct a subrange type from the constant expressions
		indicated by "lb" and "ub", but first perform some
		checks
	*/
	register struct type *tp = BaseType(lb->nd_type), *res;

	if (!TstCompat(lb->nd_type, ub->nd_type)) {
		node_error(ub, "Types of subrange bounds not equal");
		return error_type;
	}

	if (tp == intorcard_type) {
		/* Lower bound >= 0; in this case, the base type is CARDINAL,
		   according to the language definition, par. 6.3
		*/
		assert(lb->nd_INT >= 0);
		tp = card_type;
	}

	/* Check base type
	*/
	if (! (tp->tp_fund & T_DISCRETE)) {
		node_error(ub, "Illegal base type for subrange");
		return error_type;
	}

	/* Check bounds
	*/
	if (lb->nd_INT > ub->nd_INT) {
		node_error(ub, "Lower bound exceeds upper bound");
	}

	/* Now construct resulting type
	*/
	res = construct_type(T_SUBRANGE, tp);
	res->sub_lb = lb->nd_INT;
	res->sub_ub = ub->nd_INT;
	res->tp_size = tp->tp_size;
	res->tp_align = tp->tp_align;
	return res;
}

genrck(tp)
	register struct type *tp;
{
	/*	generate a range check descriptor for type "tp" when
		neccessary. Return its label.
	*/
	arith lb, ub;
	label ol, l;

	getbounds(tp, &lb, &ub);

	if (tp->tp_fund == T_SUBRANGE) {
		if (!(ol = tp->sub_rck)) {
			tp->sub_rck = l = ++data_label;
		}
	}
	else if (!(ol = tp->enm_rck)) {
		tp->enm_rck = l = ++data_label;
	}
	if (!ol) {
		ol = l;
		C_df_dlb(ol);
		C_rom_cst(lb);
		C_rom_cst(ub);
	}
	C_lae_dlb(ol, (arith) 0);
	C_rck(word_size);
}

getbounds(tp, plo, phi)
	register struct type *tp;
	arith *plo, *phi;
{
	/*	Get the bounds of a bounded type
	*/

	assert(bounded(tp));

	if (tp->tp_fund == T_SUBRANGE) {
		*plo = tp->sub_lb;
		*phi = tp->sub_ub;
	}
	else {
		*plo = 0;
		*phi = tp->enm_ncst - 1;
	}
}

struct type *
set_type(tp)
	register struct type *tp;
{
	/*	Construct a set type with base type "tp", but first
		perform some checks
	*/
	arith lb, ub;

	if (! bounded(tp)) {
		error("illegal base type for set");
		return error_type;
	}

	getbounds(tp, &lb, &ub);

	if (lb < 0 || ub > MAXSET-1) {
		error("Set type limits exceeded");
		return error_type;
	}

	tp = construct_type(T_SET, tp);
	tp->tp_size = WA(((ub - lb) + 8)/8);
	return tp;
}

arith
ArrayElSize(tp)
	register struct type *tp;
{
	/* Align element size to alignment requirement of element type.
	   Also make sure that its size is either a dividor of the word_size,
	   or a multiple of it.
	*/
	arith algn;

	if (tp->tp_fund == T_ARRAY) ArraySizes(tp);
	algn = align(tp->tp_size, tp->tp_align);
	if (word_size % algn != 0) {
		/* algn is not a dividor of the word size, so make sure it
		   is a multiple
		*/
		algn = WA(algn);
	}
	return algn;
}

ArraySizes(tp)
	register struct type *tp;
{
	/*	Assign sizes to an array type, and check index type
	*/
	register struct type *index_type = IndexType(tp);
	register struct type *elem_type = tp->arr_elem;
	arith lo, hi;

	tp->arr_elsize = ArrayElSize(elem_type);
	tp->tp_align = elem_type->tp_align;

	/* check index type
	*/
	if (! bounded(index_type)) {
		error("Illegal index type");
		tp->tp_size = 0;
		return;
	}

	getbounds(index_type, &lo, &hi);

	tp->tp_size = WA((hi - lo + 1) * tp->arr_elsize);

	/* generate descriptor and remember label.
	*/
	tp->arr_descr = ++data_label;
	C_df_dlb(tp->arr_descr);
	C_rom_cst(lo);
	C_rom_cst(hi - lo);
	C_rom_cst(tp->arr_elsize);
}

FreeType(tp)
	struct type *tp;
{
	/*	Release type structures indicated by "tp".
		This procedure is only called for types, constructed with
		T_PROCEDURE.
	*/
	register struct paramlist *pr, *pr1;

	assert(tp->tp_fund == T_PROCEDURE);

	pr = ParamList(tp);
	while (pr) {
		pr1 = pr;
		pr = pr->next;
		free_paramlist(pr1);
	}

	free_type(tp);
}

int
gcd(m, n)
	register int m, n;
{
	/*	Greatest Common Divisor
 	*/
	register int r;

	while (n)	{
		r = m % n;
		m = n;
		n = r;
	}
	return m;
}

int
lcm(m, n)
	int m, n;
{
	/*	Least Common Multiple
 	*/
	return m * (n / gcd(m, n));
}

#ifdef DEBUG
DumpType(tp)
	register struct type *tp;
{
	if (!tp) return;

	print(" a:%d; s:%ld;", tp->tp_align, (long) tp->tp_size);
	if (tp->next && tp->tp_fund != T_POINTER) {
		/* Avoid printing recursive types!
		*/
		print(" n:(");
		DumpType(tp->next);
		print(")");
	}

	print(" f:");
	switch(tp->tp_fund) {
	case T_RECORD:
		print("RECORD"); break;
	case T_ENUMERATION:
		print("ENUMERATION; n:%d", tp->enm_ncst); break;
	case T_INTEGER:
		print("INTEGER"); break;
	case T_CARDINAL:
		print("CARDINAL"); break;
	case T_REAL:
		print("REAL"); break;
	case T_POINTER:
		print("POINTER"); break;
	case T_CHAR:
		print("CHAR"); break;
	case T_WORD:
		print("WORD"); break;
	case T_SET:
		print("SET"); break;
	case T_SUBRANGE:
		print("SUBRANGE %ld-%ld", (long) tp->sub_lb, (long) tp->sub_ub);
		break;
	case T_PROCEDURE:
		{
		register struct paramlist *par = ParamList(tp);

		print("PROCEDURE");
		if (par) {
			print("; p:");
			while(par) {
				if (IsVarParam(par)) print("VAR ");
				DumpType(TypeOfParam(par));
				par = par->next;
			}
		}
		break;
		}
	case T_ARRAY:
		print("ARRAY");
		print("; el:");
		DumpType(tp->arr_elem);
		print("; index:");
		DumpType(tp->next);
		break;
	case T_STRING:
		print("STRING"); break;
	case T_INTORCARD:
		print("INTORCARD"); break;
	default:
		crash("DumpType");
	}
	print(";");
}
#endif
