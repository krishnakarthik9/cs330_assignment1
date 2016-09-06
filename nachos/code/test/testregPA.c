#include "syscall.h"
#define SIZE 100
int
main()
{
    int array[SIZE], i, sum=0;

    system_call_PrintString("Starting physical address of array: ");
    system_call_PrintInt(system_call_GetPA((unsigned)array));
    system_call_PrintChar('\n');
    system_call_PrintString("Physical address of array[50]: ");
    system_call_PrintInt(system_call_GetPA(&array[50]));
    system_call_PrintChar('\n');
    system_call_PrintString("Current physical address of stack top: ");
    system_call_PrintInt(system_call_GetPA(system_call_GetReg(29)));
    system_call_PrintChar('\n');
    for (i=0; i<SIZE; i++) array[i] = i;
    system_call_PrintString("We are currently at PC: ");
    system_call_PrintIntHex(system_call_GetReg(34));
    system_call_PrintChar('\n');
    for (i=0; i<SIZE; i++) sum += array[i];
    system_call_PrintString("Total sum: ");
    system_call_PrintInt(sum);
    system_call_PrintChar('\n');
    system_call_Exit(0);
    return 0;
}
