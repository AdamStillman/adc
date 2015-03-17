#ifndef _ENTRY_H_
#define _ENTRY_H_
#define TIMER_INTR 0x32
#ifndef ASSEMBLER //skip if ASSEMBLER defined(in assembly code)
void TimerEntry();//defined in entry.S assembly wont take this
#endif
#endif
