/*#define DEBUG 1	/* define when debugging */

#define BYTES_REVERSED
#define WORDS_REVERSED

#ifndef WINDOWSIZE
#define WINDOWSIZE	(16*4)
#endif

#define ONE_BYTE        int
#define TWO_BYTES	int
#define FOUR_BYTES	long

#define EM_WSIZE	4
#define EM_PSIZE	4
#define EM_FSIZE	4
#define EM_DSIZE	8
#define EM_BSIZE	REGSAV

#define	BSS_INIT	0

#ifdef __solaris__
#define NAME_FMT	"$%s"
#define DNAM_FMT	"$%s"
#define DLB_FMT		".L_%ld"
#define	ILB_FMT		".L%x_%lx"
#define GENLAB		'.'
#else
#define NAME_FMT	"_%s"
#define DNAM_FMT	"_%s"
#define DLB_FMT		"L_%ld"
#define	ILB_FMT		"L%x_%lx"
#define GENLAB		'L'
#endif
#define HOL_FMT		"hol%d"
#define STR_FMT		".ascii\t"


#define	ALIGN_FMT		".align 4\n"

#define	BYTE_FMT		".byte %ld\n"
#define	WORD_FMT		".half %ld\n"
#define	LONG_FMT  		".word %ld\n"
#define	BSS_FMT			".skip %ld\n"

#ifdef __solaris__
#define	SEGTXT_FMT  		".section \".text\"\n"
#define	SEGDAT_FMT		".section \".data\"\n"
#define	SEGBSS_FMT		".section \".bss\"\n"
#else
#define	SEGTXT_FMT  		".seg \"text\"\n"
#define	SEGDAT_FMT		".seg \"data\"\n"
#define	SEGBSS_FMT		".seg \"bss\"\n"
#endif

#define	SYMBOL_DEF_FMT		"%s:\n"
#define	GLOBAL_FMT		".global %s\n"
#ifdef __solaris__
#define COMM_FMT		".reserve %s, %ld, \".bss\"\n"
#else
#define COMM_FMT		".reserve %s, %ld, \"bss\"\n"
#endif
#define	LOCAL_FMT		""

#define	RELOC1_FMT		"ONLY LONGS CAN BE RELOCATED!"
#define	RELOC2_FMT		"ONLY LONGS CAN BE RELOCATED!"
#ifdef __solaris__
#define	RELOC4_FMT		".section \".data\"\n.align 4\n.word %s+%d\n"
#else
#define	RELOC4_FMT		".seg \"data\"\n.align 4\n.word %s+%d\n"
#endif

#define ALIGN_GAP	4
#define FLOATTRANS	8
#define MAX_NR_FLT_REGS	16
#define FLTSAV		(MAX_NR_FLT_REGS * 4)
#define FLTSAV_OFFSET	(WINDOWSIZE + FLOATTRANS)
#define REGSAV		(WINDOWSIZE + FLOATTRANS + ALIGN_GAP + FLTSAV)

/*#define MATH_DIVIDE	1	/* define when using mathematical def of / */

#ifndef DEBUG
#undef arg_error
#define arg_error(s,i)
#define RESOLV_debug 1
#endif
