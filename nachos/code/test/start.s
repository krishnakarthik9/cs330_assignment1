/* Start.s 
 *	Assembly language assist for user programs running on top of Nachos.
 *
 *	Since we don't want to pull in the entire C library, we define
 *	what we need for a user program here, namely Start and the system
 *	calls.
 */

#define IN_ASM
#include "syscall.h"

        .text   
        .align  2

/* -------------------------------------------------------------
 * __start
 *	Initialize running a C program, by calling "main". 
 *
 * 	NOTE: This has to be first, so that it gets loaded at location 0.
 *	The Nachos kernel always starts a program by jumping to location 0.
 * -------------------------------------------------------------
 */

	.globl __start
	.ent	__start
__start:
	jal	main
	move	$4,$0		
	jal	system_call_Exit	 /* if we return from main, exit(0) */
	.end __start

/* -------------------------------------------------------------
 * System call stubs:
 *	Assembly language assist to make system calls to the Nachos kernel.
 *	There is one stub per system call, that places the code for the
 *	system call into register r2, and leaves the arguments to the
 *	system call alone (in other words, arg1 is in r4, arg2 is 
 *	in r5, arg3 is in r6, arg4 is in r7)
 *
 * 	The return value is in r2. This follows the standard C calling
 * 	convention on the MIPS.
 * -------------------------------------------------------------
 */

	.globl system_call_Halt
	.ent	system_call_Halt
system_call_Halt:
	addiu $2,$0,SYScall_Halt
	syscall
	j	$31
	.end system_call_Halt

	.globl system_call_Exit
	.ent	system_call_Exit
system_call_Exit:
	addiu $2,$0,SYScall_Exit
	syscall
	j	$31
	.end system_call_Exit

	.globl system_call_Exec
	.ent	system_call_Exec
system_call_Exec:
	addiu $2,$0,SYScall_Exec
	syscall
	j	$31
	.end system_call_Exec

	.globl system_call_Join
	.ent	system_call_Join
system_call_Join:
	addiu $2,$0,SYScall_Join
	syscall
	j	$31
	.end system_call_Join

	.globl system_call_Create
	.ent	system_call_Create
system_call_Create:
	addiu $2,$0,SYScall_Create
	syscall
	j	$31
	.end system_call_Create

	.globl system_call_Open
	.ent	system_call_Open
system_call_Open:
	addiu $2,$0,SYScall_Open
	syscall
	j	$31
	.end system_call_Open

	.globl system_call_Read
	.ent	system_call_Read
system_call_Read:
	addiu $2,$0,SYScall_Read
	syscall
	j	$31
	.end system_call_Read

	.globl system_call_Write
	.ent	system_call_Write
system_call_Write:
	addiu $2,$0,SYScall_Write
	syscall
	j	$31
	.end system_call_Write

	.globl system_call_Close
	.ent	system_call_Close
system_call_Close:
	addiu $2,$0,SYScall_Close
	syscall
	j	$31
	.end system_call_Close

	.globl system_call_Fork
	.ent	system_call_Fork
system_call_Fork:
	addiu $2,$0,SYScall_Fork
	syscall
	j	$31
	.end system_call_Fork

	.globl system_call_Yield
	.ent	system_call_Yield
system_call_Yield:
	addiu $2,$0,SYScall_Yield
	syscall
	j	$31
	.end system_call_Yield

	.globl system_call_PrintInt
	.ent    system_call_PrintInt
system_call_PrintInt:
        addiu $2,$0,SYScall_PrintInt
        syscall
        j       $31
	.end system_call_PrintInt

	.globl system_call_PrintChar
	.ent    system_call_PrintChar
system_call_PrintChar:
        addiu $2,$0,SYScall_PrintChar
        syscall
        j       $31
	.end system_call_PrintChar

	.globl system_call_PrintString
	.ent    system_call_PrintString
system_call_PrintString:
        addiu $2,$0,SYScall_PrintString
        syscall
        j       $31
	.end system_call_PrintString

	.globl system_call_GetReg
	.ent    system_callGetReg
system_call_GetReg:
	addiu $2,$0,SYScall_GetReg
	syscall
	j       $31
	.end system_call_GetReg

	.globl system_call_GetPA
	.ent    system_call_GetPA
system_call_GetPA:
	addiu $2,$0,SYScall_GetPA
	syscall
	j       $31
	.end system_call_GetPA

	.globl system_call_GetPID
	.ent    system_call_GetPID
system_call_GetPID:
	addiu $2,$0,SYScall_GetPID
	syscall
	j       $31
	.end system_call_GetPID

	.globl system_call_GetPPID
	.ent    system_call_GetPPID
system_call_GetPPID:
	addiu $2,$0,SYScall_GetPPID
	syscall
	j       $31
	.end system_call_GetPPID

	.globl system_call_Sleep
	.ent    system_call_Sleep
system_call_Sleep:
	addiu $2,$0,SYScall_Sleep
	syscall
	j       $31
	.end system_call_Sleep

	.globl system_call_GetTime
	.ent    system_call_GetTime
system_call_GetTime:
	addiu $2,$0,SYScall_Time
	syscall
	j       $31
	.end system_call_GetTime

	.globl system_call_GetNumInstr
	.ent    system_call_GetNumInstr
system_call_GetNumInstr:
	addiu $2,$0,SYScall_NumInstr
	syscall
	j	$31
	.end system_call_GetNumInstr

	.globl system_call_PrintIntHex
	.ent    system_call_PrintIntHex
system_call_PrintIntHex:
	addiu $2,$0,SYScall_PrintIntHex
	syscall
	j	$31
	.end system_call_PrintIntHex

/* dummy function to keep gcc happy */
        .globl  __main
        .ent    __main
__main:
        j       $31
        .end    __main

