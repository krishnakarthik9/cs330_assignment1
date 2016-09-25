
CS330 ASSIGNMENT1 README

New System calls are added in exception.cc and NachOSThread 
variables and functions are added in thread.cc and thread.h 
All The 12 system calls are explained below.

SYScall_GetReg

1)Read register number from Register 4(input register)
2)Write it to Register 2(return value register)

SYScall_GetPA

1)Check if all three given condition are satisfied
2)If not just set return_value=-1
3)If they are satisfied Use Translate function to translate virtual address into physical address

SYScall_Time

1)TotalTicks variable is defined in stats.h
2)Just access it by stats->TotalTicks

SYScall_GetPID

1)Created a global variable in thread.cc which keeps the number of pid's
allotted until now
2)We increment it every time a new thread is created(In constructor of NachOSThread class)
3)Just assign of pid of the new thread to be this variable
4)As pid is a public variable just access it by using currentThread->pid


SYScall_GetPPID

1)Parent is thread that would be calling The constructor of new Thread
2)So when a new thread is created currentThread would be its parent
3)So its ppid would be currentThread->pid
4)Handle boundary case i.e when thread pid=1 assign ppid=0
5)As ppid is a public variable just access it by using currentThread->ppid

SYScall_Sleep

1)advance PC at the start itself as the process will go to sleep later
2)Read sleepTime
3)if sleepTime is zero just Yield the process using Yield CPU
4)If not assign wakeupTime to this process(Global Time at which it should wake up)
5)Insert this thread along with wakeup Time into sleepThreadList Queue created in system.h
6)Turn interrupt off before putting this thread to sleep
7)put thread to sleep using PutThreadToSleep() function
8)Turn interrupts back on
9)Now we should handle these threads that are inserted in SleepThreadList
in TimerInterruptHandler function
10)Starting from the start of list Wake Up all the process with wakeupTime less than Current Time 
11)This can be done using ThreadIsReadyToRun() function defined

SYScall_Yield

1)advance PC at the start itself
2)Just Yield the process using YieldCPU() function

SYScall_Join

1)Created a two new arrays for each process which contain pid's of all its children and
status of them respectively
3)We will updating them during appropriate system calls(Fork,Exit etc.)
4)Check passed pid is the child of given process
5)if not just set return_value=-1
6)then check corresponding child's status if its not child_running it already exited
then set return value to -1
7)if it is Running then put parent to sleep every time it wakes Up using PutThreadToSleep in a while loop

SYScall_Exec

1)Use code from print_String system call to take address from register and converts to a string i.e filename
2)Use StartUserProcess() function code for Executing the file this function sets up
the address space for exec and then runs the executable using Run()

SYScall_Fork

1)First advance the program counter
2)create a new Thread called childThread
3)in the thread constructor assign its parent
4)add this child to array of children of parent and set its status to Child_Running 
using addChildToParent() defined in thread.cc
5)Create a new constructor for ProcessAddrSpace in addrspace.cc so that we can use it 
to set up address space of child
6)In this constructor make sure to copy pageTable of parent into child addressSpace
7)Also copy the MainMemory of parent to mainMemory of child
8)Make sure not to overwrite Parent Memory
9)A variable called currPages for that purpose 
10)set Register 2 to 0 for child to get return value of fork to be 0
11)Save state of child using SaveUserState() function
12)Allocate stack for child using AllocateThreadStack() method 
13)write a function called childExecutesHere() which is where childThread starts
executing from.
14)In this function Restore state of the thread we setup in exception.cc
15)Run the thread using Run()
16)Then turn off interrupts
17)Then schedule the thread using ThreadIsReadyToRun()
18)Turn back interrupts on

SYScall_Exit

1)Check if it's parent is waiting for it
2)If so then Set interrupts off and start running parent using ThreadIsReadyToRun() function
set interrupts back on
3)Make sure to set its childStatus with its exit status so that its parent knows it exited
4)Then Destroy the thread using FinishThread() function
5)Halt the system if thread to be executed has parent NULL.

SYScall_NumInstr

1)There is a function called OneInstruction() which is called for every instruction
2)for every thread create a variable called currenInstr in NachOSThread and update it 
in OneInstruction() function.