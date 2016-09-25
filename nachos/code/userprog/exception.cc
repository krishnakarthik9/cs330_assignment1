// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "console.h"
#include "synch.h"

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------
static Semaphore *readAvail;
static Semaphore *writeDone;
static void ReadAvail(int arg) {
	readAvail->V();
}
static void WriteDone(int arg) {
	writeDone->V();
}

static void ConvertIntToHex(unsigned v, Console *console) {
	unsigned x;
	if (v == 0)
		return;
	ConvertIntToHex(v / 16, console);
	x = v % 16;
	if (x < 10) {
		writeDone->P();
		console->PutChar('0' + x);
	} else {
		writeDone->P();
		console->PutChar('a' + x - 10);
	}
}

void ExceptionHandler(ExceptionType which) {
	int type = machine->ReadRegister(2);
	int memval, vaddr, printval, tempval, exp;
	unsigned printvalus;        // Used for printing in hex
	if (!initializedConsoleSemaphores) {
		readAvail = new Semaphore("read avail", 0);
		writeDone = new Semaphore("write done", 1);
		initializedConsoleSemaphores = true;
	}
	Console *console = new Console(NULL, NULL, ReadAvail, WriteDone, 0);
	;
	currentThread->currentInstr++;
	if ((which == SyscallException) && (type == SYScall_Halt)) {
		DEBUG('a', "Shutdown, initiated by user program.\n");
		interrupt->Halt();
	} else if ((which == SyscallException) && (type == SYScall_PrintInt)) {
		printval = machine->ReadRegister(4);
		if (printval == 0) {
			writeDone->P();
			console->PutChar('0');
		} else {
			if (printval < 0) {
				writeDone->P();
				console->PutChar('-');
				printval = -printval;
			}
			tempval = printval;
			exp = 1;
			while (tempval != 0) {
				tempval = tempval / 10;
				exp = exp * 10;
			}
			exp = exp / 10;
			while (exp > 0) {
				writeDone->P();
				console->PutChar('0' + (printval / exp));
				printval = printval % exp;
				exp = exp / 10;
			}
		}
		// Advance program counters.
		machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
		machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
		machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg) + 4);
	} else if ((which == SyscallException) && (type == SYScall_PrintChar)) {
		writeDone->P();
		console->PutChar(machine->ReadRegister(4));   // echo it!
		// Advance program counters.
		machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
		machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
		machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg) + 4);
	} else if ((which == SyscallException) && (type == SYScall_PrintString)) {
		vaddr = machine->ReadRegister(4);
		machine->ReadMem(vaddr, 1, &memval);
		while ((*(char*) &memval) != '\0') {
			writeDone->P();
			console->PutChar(*(char*) &memval);
			vaddr++;
			machine->ReadMem(vaddr, 1, &memval);
		}
		// Advance program counters.
		machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
		machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
		machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg) + 4);
	} else if ((which == SyscallException) && (type == SYScall_PrintIntHex)) {
		printvalus = (unsigned) machine->ReadRegister(4);
		writeDone->P();
		console->PutChar('0');
		writeDone->P();
		console->PutChar('x');
		if (printvalus == 0) {
			writeDone->P();
			console->PutChar('0');
		} else {
			ConvertIntToHex(printvalus, console);
		}
		// Advance program counters.
		machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
		machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
		machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg) + 4);
	}

	else if ((which == SyscallException) && (type == SYScall_GetReg)) {
		//TODO: test
		int reg_num = (machine->ReadRegister(4));
		int curr_reg = (machine->ReadRegister(reg_num));
		machine->WriteRegister(2, curr_reg);
		// Advance program counters.
		machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
		machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
		machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg) + 4);
	} else if ((which == SyscallException) && (type == SYScall_GetPA)) {
		//TODO: test
		int virtual_address = (machine->ReadRegister(4));
		int virt_page_num = (virtual_address) / (PageSize);
		int return_value;
		if ((virt_page_num <= (machine->pageTableSize))
				&& (machine->NachOSpageTable[virt_page_num].valid == TRUE)
				&& (machine->NachOSpageTable[virt_page_num].physicalPage
						<= NumPhysPages)) {
			//TODO: verify
			machine->Translate(virtual_address, &return_value, 4, false);
			//virtual address to physical address should go here(def in translate method)
		} else {
			return_value = -1;
		}
		machine->WriteRegister(2, return_value);
		//int curr_reg=(int)(machine->ReadRegister(reg_num));
		//machine->WriteRegister(2,curr_reg);
		// Advance program counters.
		machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
		machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
		machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg) + 4);
	} else if ((which == SyscallException) && (type == SYScall_Time)) {
		//TODO: test
		int curr_ticks = (stats->totalTicks);
		machine->WriteRegister(2, curr_ticks);
		// Advance program counters.
		machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
		machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
		machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg) + 4);
	} else if ((which == SyscallException) && (type == SYScall_GetPID)) {
		//TODO: test
		int curr_pid = (currentThread->pid);
		machine->WriteRegister(2, curr_pid);
		// Advance program counters.
		machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
		machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
		machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg) + 4);
	} else if ((which == SyscallException) && (type == SYScall_GetPPID)) {
		//TODO: test
		int curr_ppid = (currentThread->ppid);
		machine->WriteRegister(2, curr_ppid);
		// Advance program counters.
		machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
		machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
		machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg) + 4);
	} else if ((which == SyscallException) && (type == SYScall_Sleep)) {
		//TODO: test
		//Advance PC first as the curret process will go to sleep later and we will not be able advance PC
		machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
		machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
		machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg) + 4);
		int SleepTime = (machine->ReadRegister(4));
		if (SleepTime == 0) {
			currentThread->YieldCPU();
		} else {
			int wakeUpTime = (SleepTime + (stats->totalTicks));
			sleepThreadList->SortedInsert((void *) currentThread, wakeUpTime);
			interrupt->SetLevel(IntOff);
			// printf("wakeUpTime=%d\n", wakeUpTime);
			currentThread->PutThreadToSleep();
			// printf("wakeUpTime=%d\n", wakeUpTime);
			interrupt->SetLevel(IntOn);
			// printf("wakeUpTime=%d\n", wakeUpTime);
		}

	} else if ((which == SyscallException) && (type == SYScall_Yield)) {
		//TODO:test
		machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
		machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
		machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg) + 4);

		currentThread->YieldCPU();
	} else if ((which == SyscallException) && (type == SYScall_Join)) {
		//TODO: test
		int childPID = (machine->ReadRegister(4));
		int returnValue;
		if (currentThread->getChildIndex(childPID) == -1) {
			returnValue = -1;
		} else {
			//TODO: complete this
			int status = currentThread->getChildStatus(childPID);
			if (status == Child_Running) {
				currentThread->setChildStatus(childPID, Parent_Waiting);
				interrupt->SetLevel(IntOff);
				currentThread->PutThreadToSleep();
				interrupt->SetLevel(IntOn);
			} else {
				returnValue = -1; //pid for child created but it already exited before
			}

			//Now we need to make sure that when ever this thread wakes for some reason and and childStatus is still Parent_Waiting ie., child did not exit

			status = currentThread->getChildStatus(childPID); //Could be exit status instead of Parent_Waiting

			while (status == Parent_Waiting) {
				interrupt->SetLevel(IntOff);
				currentThread->PutThreadToSleep();
				interrupt->SetLevel(IntOn);
				status = currentThread->getChildStatus(childPID);
			}

		}

		machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
		machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
		machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg) + 4);

	} else if ((which == SyscallException) && (type == SYScall_Exec)) {
		//TODO:test		
		//changing code from Print_String
		// printf("entered exec\n");
		char filename[100];
		int i=0;
		vaddr = machine->ReadRegister(4);
		machine->ReadMem(vaddr, 1, &memval);
		while ((*(char*) &memval) != '\0') {
			filename[i]=char(memval);
			i++;
			vaddr++;
			machine->ReadMem(vaddr, 1, &memval);
		}


		filename[i]=char(memval);

		
		//Code from start user process
		OpenFile *executable = fileSystem->Open(filename);
		ProcessAddrSpace *space;

		if (executable == NULL) {
			printf("Unable to open file %s\n", filename);
			return;
		}
		space = new ProcessAddrSpace(executable);
		currentThread->space = space;

		delete executable;			// close file

		space->InitUserCPURegisters();		// set the initial register values
		space->RestoreStateOnSwitch();		// load page table register
		machine->Run();			// jump to the user progam
		ASSERT(FALSE);			// machine->Run never returns;
		// the address space exits
		// by doing the syscall "exit"

	} else if ((which == SyscallException) && (type == SYScall_Fork)) {
		//TODO: test
		machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
		machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
		machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg) + 4);

		NachOSThread * childThread = new NachOSThread("child thread");//constructor of thread class
		// printf("child=%d",childThread->pid);
		currentThread->addChildToParent(childThread->pid, Child_Running);
		childThread->space=new ProcessAddrSpace(currentThread->space->numPagesInVM,currentThread->space->NachOSpageTable[0].physicalPage);//TODO change Process Adrress space as required
		machine->WriteRegister(2,0);
		// printf("pid of parent%d\n",childThread->parent->pid);
		// printf("physicalPage=%d\t\n",currentThread->space->NachOSpageTable[0].physicalPage);
		childThread->SaveUserState();
		machine->WriteRegister(2, childThread->pid);
		childThread->AllocateThreadStack(&childExecutesHere,0);
		
		// printf("childstatus=%d",currentThread->getChildStatus(2));
		// printf("numchild=%d\n",currentThread->numChild);
		interrupt->SetLevel(IntOff);
		// printf("childppid********=%d",childThread->ppid);

		scheduler->ThreadIsReadyToRun(childThread);
		// printf("childaaaaaaaaaaaaa=%d",childThread->pid);
		interrupt->SetLevel(IntOn);
		// printf("childphysaddr=%d\t\n",currentThread->space->NachOSpageTable[0].physicalPage);

	} else if ((which == SyscallException) && (type == SYScall_Exit)) {
		//TODO: test
		//TODO: complete this
		// printf("thread pid calling exit = %d\n", currentThread->pid);
		int status = (machine->ReadRegister(4));
		// printf("status=%d\n", status);

		if (currentThread->parent != NULL) {
			// printf("entered first if\n");
			int threadStatusAsChild = (currentThread->parent->getChildStatus(
					currentThread->pid));
			// printf("threadStatusAsChild=%d\n", threadStatusAsChild);

			if (threadStatusAsChild == Parent_Waiting) {
				// printf("entered second if\n");
				interrupt->SetLevel(IntOff);
				scheduler->ThreadIsReadyToRun(currentThread->parent);
				interrupt->SetLevel(IntOn);
			}
			currentThread->parent->setChildStatus(currentThread->pid, status);
		}
		else
		{
			interrupt->Halt();
		}
		// printf("out of if\n");
		//set its exit status so parent knows it exited
		currentThread->FinishThread(); //detailed in draft

	} else if ((which == SyscallException) && (type == SYScall_NumInstr)) {
		//TODO:test		
		machine->WriteRegister(2, currentThread->currentInstr); //TODO check if each has its own PC
		// printf("number of instructions=%d\n", (machine->ReadRegister(PCReg) - currentThread->startPC) / 4);
		// printf("old PrevPCReg=%d\n", machine->ReadRegister(PrevPCReg));
		machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
		// printf("new PrevPCReg=%d\n", machine->ReadRegister(PrevPCReg));
		// printf("old PCReg=%d\n", machine->ReadRegister(PCReg));
		machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
		// printf("PCReg=%d\n", machine->ReadRegister(PCReg));
		// printf("old NextPCReg=%d\n", machine->ReadRegister(NextPCReg));
		machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg) + 4);
		// printf("new NextPCReg=%d\n", machine->ReadRegister(NextPCReg));
	} else {
		currentThread->currentInstr--;
		printf("Unexpected user mode exception %d %d\n", which, type);
		ASSERT(FALSE);
	}
}
