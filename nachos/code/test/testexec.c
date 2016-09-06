#include "syscall.h"

int
main()
{
    system_call_PrintString("Before calling Exec.\n");
    system_call_Exec("../test/vectorsum");
    system_call_PrintString("Returned from Exec.\n"); // Should never return
    return 0;
}
