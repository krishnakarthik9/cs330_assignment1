#include "syscall.h"
#define SIZE 100
int
main()
{
    int array[SIZE], i, sum=0;

    for (i=0; i<SIZE; i++) array[i] = i;
    for (i=0; i<SIZE; i++) sum += array[i];
    system_call_PrintString("Total sum: ");
    system_call_PrintInt(sum);
    system_call_PrintChar('\n');
    system_call_PrintString("Executed instruction count: ");
    system_call_PrintInt(system_call_GetNumInstr());
    system_call_PrintChar('\n');
    system_call_Exit(0);
    return 0;
}
