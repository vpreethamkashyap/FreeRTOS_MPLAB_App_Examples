#include <p32xxxx.h>
#include <sys/asm.h>
#include "ISR_Support.h"

	.set	nomips16
 	.set 	noreorder
 	
// 	.extern vU2InterruptHandler
 	.extern vU1InterruptHandler
	.extern xISRStackTop
// 	.global	vU2InterruptWrapper
 	.global	vU1InterruptWrapper

	.set		noreorder
	.set 		noat
//	.ent		vU2InterruptWrapper

	.ent		vU1InterruptWrapper
/*
vU2InterruptWrapper:

	portSAVE_CONTEXT
	jal vU2InterruptHandler
	nop
	portRESTORE_CONTEXT

	.end		vU2InterruptWrapper
*/
vU1InterruptWrapper:

	portSAVE_CONTEXT
	jal vU1InterruptHandler
	nop
	portRESTORE_CONTEXT

	.end		vU1InterruptWrapper

