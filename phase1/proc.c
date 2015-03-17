// proc.c, 159
// processes are here

#include "spede.h"   // for IO_DELAY() needed here below
#include "extern.h"  // for current_run_pid needed here below
#include "proc.h"    // for Idle, SimpleProc, DispatchProc


void Idle() {
	int i =0;
	int a;
	while(1){
	cons_printf("%d ", i);		// print 0 on PC
	for(a=0; a<1666000; a++) IO_DELAY();   //busy-loop delay for about 1 sec
	}
} 

void UserProc() {
	int i;
	while(1){
	cons_printf("%d ", CRP);  // print its pid (CRP) on PC
	for(i=0; i<1666000; i++) IO_DELAY();  // busy-loop delay for about 1 sec

	}
}
