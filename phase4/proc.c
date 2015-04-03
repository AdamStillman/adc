// proc.c, 159
// processes are here

#include "spede.h"   // for IO_DELAY() needed here below
#include "extern.h"  // for current_run_pid needed here below
#include "proc.h"    // for Idle, SimpleProc, DispatchProc
#include "syscall.h"
#include <spede/machine/parallel.h>

void Idle() {
	int i =0;
	int a;
	for(;;){
	cons_printf("%d ", i);		// print 0 on PC
	for(a=0; a<1666000; a++) IO_DELAY();   //busy-loop delay for about 1 sec
	}
} 

void UserProc() {
  int sleep;
	int p = GetPid();
	for(;;){
	 
		cons_printf("%d ", p);  // print its pid (CRP) on PC	
		sleep = 4 - (p%4);	
		Sleep(sleep);
	}
}

void PrintDriver(){
int i, code;
char str[] = "Hello, my Team is called TSLK \n\0";
char *p;
//int TIME_OUT=3*1666000;                   // time out in 3 sec
print_semaphore = SemGet(-1);//should be -1 but depends on IRQISR

outportb(LPT1_BASE+LPT_CONTROL, PC_SLCTIN);// printer control select interrup
code = inportb(LPT1_BASE+LPT_STATUS);
for(i=0; i<50; i++) IO_DELAY();
outportb(LPT1_BASE+LPT_CONTROL, PC_INIT|PC_SLCTIN|PC_IRQEN);
Sleep(1);
while(1){
	cons_printf("my pid is: %d: \n", GetPid() );
	Sleep(1);
	if(print_it ==1){
		p = str-1;
		while(*p){
			outportb(LPT1_BASE+LPT_DATA, *p);      // send char to data reg
			code = inportb(LPT1_BASE+LPT_CONTROL); // read control reg
			outportb(LPT1_BASE+LPT_CONTROL, code|PC_STROBE); // send with added strobe
			for(i=0; i<20; i++) IO_DELAY();        // delay for EPSON LP-571 printer
			outportb(LPT1_BASE+LPT_CONTROL, code); // send original control code
			SemWait(print_semaphore);
			*p++;
		}//while p
	}// if print
	print_it=0;
}//while1



}//print driver


