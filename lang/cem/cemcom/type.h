/* $Header$ */
/* TYPE DESCRIPTOR */

#include	"nobitfield.h"

struct type	{
	struct type *next;	/* used only with ARRAY */
	short tp_fund;		/* fundamental type */
	char tp_unsigned;
	int tp_align;
	arith tp_size;		/* -1 if declared but not defined */
	struct idf *tp_idf;	/* name of STRUCT, UNION or ENUM */
	struct sdef *tp_sdef;	/* to first selector */
	struct type *tp_up;	/* from FIELD, POINTER, ARRAY
					or FUNCTION to fund. */
	struct field *tp_field;	/* field descriptor if fund == FIELD	*/
	struct type *tp_pointer;/* to POINTER */
	struct type *tp_array;	/* to ARRAY */
	struct type *tp_function;/* to FUNCTION */
};

extern struct type
	*create_type(), *standard_type(), *construct_type(), *pointer_to(),
	*array_of(), *function_of();

#ifndef NOBITFIELD
extern struct type *field_of();
#endif NOBITFIELD

extern struct type
	*char_type, *uchar_type,
	*short_type, *ushort_type,
	*word_type, *uword_type,
	*int_type, *uint_type,
	*long_type, *ulong_type,
	*float_type, *double_type,
	*void_type, *label_type,
	*string_type, *funint_type, *error_type;

extern struct type *pa_type;	/* type.c	*/

extern arith size_of_type(), align();


/* allocation definitions of struct type */
/* ALLOCDEF "type" */
extern char *st_alloc();
extern struct type *h_type;
#define	new_type() ((struct type *) \
		st_alloc((char **)&h_type, sizeof(struct type)))
#define	free_type(p) st_free(p, h_type, sizeof(struct type))

