#include "syscall.h"

int
main()
{
    int x;
    int sleep_start, sleep_end;

    system_call_PrintString("Parent PID: ");
    system_call_PrintInt(system_call_GetPID());
    system_call_PrintChar('\n');
    x = system_call_Fork();
    if (x == 0) {
       system_call_PrintString("Child PID: ");
       system_call_PrintInt(system_call_GetPID());
       system_call_PrintChar('\n');
       system_call_PrintString("Child's parent PID: ");
       system_call_PrintInt(system_call_GetPPID());
       system_call_PrintChar('\n');
       sleep_start = system_call_GetTime();
       system_call_Sleep(100);
       sleep_end = system_call_GetTime();
       system_call_PrintString("Child called sleep at time: ");
       system_call_PrintInt(sleep_start);
       system_call_PrintChar('\n');
       system_call_PrintString("Child returned from sleep at time: ");
       system_call_PrintInt(sleep_end);
       system_call_PrintChar('\n');
       system_call_PrintString("Child executed ");
       system_call_PrintInt(system_call_GetNumInstr());
       system_call_PrintString(" instructions.\n");
    }
    else {
       system_call_PrintString("Parent after fork waiting for child: ");
       system_call_PrintInt(x);
       system_call_PrintChar('\n');
       system_call_Join(x);
       system_call_PrintString("Parent executed ");
       system_call_PrintInt(system_call_GetNumInstr());
       system_call_PrintString(" instructions.\n");
    }
    return 0;
}
