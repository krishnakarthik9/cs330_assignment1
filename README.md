# cs330
krishna's and gowtham's assignments for cs330 course in iitk 2016

New System calls are added in exception.cc and NachOSThread 
variables and functions are added in thread.cc and thread.h 


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
4)Handle boundary case i.e when thread pid=1 assing ppid=0
5)As ppid is a public variable just access it by using currentThread->ppid

SYScall_Sleep

1)advance PC at the start itself as the process will go to sleep later
2)Read sleepTime
3)if sleepTime is zero just Yield the process using Yield CPU
4)If not assign wakeupTime to this process(Global Time at which it should wake up)
5)Insert this thread along with wakeup Time into sleepThreadList Queue created in system.h
6)Turn interrupt off before putting this thread to sleep
7)put thread to sleep using PutThreadToSleep() function
8)Turn interupts back on
9)Now we should handle these threads that are inserted in SleepThreadList
in TimerInterruptHandler function
10)Starting from the start of list Wake Up all the process with wakeupTime less than Current Time 
11)This can be done using ThreadIsReadyToRun() function defined

SYScall_Yield

1)advance PC at the start itself
2)Just Yield the process using YieldCPU() function



