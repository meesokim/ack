#include "em_private.h"

/* $Header$ */

void
CC_opilb(opcode, ilb)
	label ilb;
{
	/*	Instruction with instruction label argument
		Argument types: b
	*/
	OP(opcode);
	CILB(ilb);
	NL();
}
