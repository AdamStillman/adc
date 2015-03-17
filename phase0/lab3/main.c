/**********************************
Adam Stillman
Phase 0, Exercise 3
main.c
**********************************/
#include <spede/stdio.h>
#include <spede/flames.h>
#include <spede/machine/io.h>
#include <spede/machine/proc_reg.h>
#include <spede/machine/seg.h>
#include <spede/machine/pic.h>
#include "entry.h" // need addr of TimerEntry

typedef void(* func_ptr_t)();
struct i386_gate *IDT_ptr;

void SetEntry(int entry_num, func_ptr_t func_ptr) {
	struct i386_gate *gateptr = &IDT_ptr[entry_num];
	fill_gate(gateptr, (int)func_ptr, get_cs(), ACC_INTR_GATE,0);
}

int main() {
	int i;
	char ch;

	if(cons_kbhit()) ch =getchar(); //clear any keys pressed

	IDT_ptr = get_idt_base(); 	//get where idt is
	cons_printf("IDT is at %u. \n", IDT_ptr);

	SetEntry(32, TimerEntry);	//prime IDT entry
	outportb(0x21, ~1);		//0x21 is a PIC mask, ~1 is mask
	asm("sti");			//set/enable intr in cpu EFLAGS

	while(1) {			//infinite loop
		for(i=0; i<1666000; i++) IO_DELAY(); //delay cpu 1 sec
		cons_putchar('X');	//show and x
		if(cons_kbhit()) break; // iff pressed break loop
	}
	return 0; 			//end main
}
