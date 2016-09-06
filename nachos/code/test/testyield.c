#include "syscall.h"

int
main()
{
    int x, i;

    x = system_call_Fork();
    for (i=0; i<5; i++) {
       system_call_PrintString("*** thread ");
       system_call_PrintInt(system_call_GetPID());
       system_call_PrintString(" looped ");
       system_call_PrintInt(i);
       system_call_PrintString(" times.\n");
       system_call_Yield();
    }
    if (x != 0) {
       system_call_PrintString("Before join.\n");
       system_call_Join(x);
       system_call_PrintString("After join.\n");
    }
    return 0;
}
