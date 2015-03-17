// main.c, 159
// simulated kernel
//
// Team Name: TSLK (Members: SEAN KENNEDY and ADAM STILLMAN)

#include "spede.h"      // spede stuff
#include "main.h"       // main stuff
#include "isr.h"        // ISR's     
#include "tool.h"       // handy functions
#include "proc.h"       // processes such as Init()
#include "type.h"       // processes such as Init()
#include "entry.h"
//#include "global.h"
//#include <spede/stdio.h>
//#include <spede/flames.h>
//#include <spede/machine/io.h>
//#include <spede/machine/proc_reg.h>
//#include <spede/machine/seg.h>
//#include <spede/machine/pic.h>
// kernel data structure:
int CRP, sys_time;                // current running PID, -1 means no process
q_t run_q, none_q, sleep_q;      // processes ready to run and not used
pcb_t pcb[MAX_PROC];    // process table

char stack[MAX_PROC][STACK_SIZE]; // run-time stacks for processes
struct i386_gate *IDT_ptr;
void InitIDT();
void SetEntry(int entry_num, func_ptr_t func_ptr) {
	struct i386_gate *gateptr = &IDT_ptr[entry_num];
	//cons_printf("in set enty\n");	
	fill_gate(gateptr, (int)func_ptr, get_cs(), ACC_INTR_GATE,0);
	//cons_printf("laeving set entry\n");
}

int main() {
	
	InitData(); // to initialize kernel data
	CreateISR(0); //to create Idle process (PID 0)
	InitIDT();
	Dispatch(pcb[0].TF_ptr);
	
   return 0;
}




void InitData() {
	int i;
//cons_printf("initializng data\n");
    //initialize 2 queues (use MyBzero() call)
	MyBzero(&run_q,0);
	MyBzero(&none_q,0);
	MyBzero(&sleep_q,0); 
 	
	//set CRP to 0
	sys_time = 0;
	CRP = 0;
	for(i=1; i<MAX_PROC; i++){//thats correct
		pcb[i].state = NONE; //set state to NONE in pcb[1~19]
		EnQ(i, &none_q );// queue PID's 1~19 (skip 0) into none_q (not used PID's)		   
   
	}
	

	//cons_printf("done init data\n");
}
//new code
void InitIDT(){
	IDT_ptr = get_idt_base(); 	//get where idt is
//	cons_printf("in idt, IDT is at memory location %u. \n", IDT_ptr);
	SetEntry(32, TimerEntry);	//prime IDT entry
	SetEntry(48 , SleepEntry);
	SetEntry(49 , GetPidEntry);
	outportb(0x21, ~0x01);		//0x21 is a PIC mask, ~1 is mask
//	cons_printf("The interrupt has been set leaving idt \n");
}


void SelectCRP() {       // pick PID as CRP
//cons_printf("in selectCRP\n");			//   simply return if CRP is greater than 0
				//   (continue only when CRP is Idle (0) or none (-1)
	if(CRP > 0) return; 

				//   if it's Idle, change its state to RUN
	if(CRP ==0) pcb[0].state = RUN;

				//   if no processes to run (check size in run queue, is it zero)
				//      set CRP to Idle process ID
	if(EmptyQ(&run_q)) CRP = 0; 

				//   else set CRP to first in run queue (dequeue it)
	else 
	CRP = DeQ(&run_q);

				//   change mode of CRP to UMODE
	pcb[CRP].mode= UMODE;
//	cons_printf("changed mode to umode\n");
				//   change state of CRP to RUNNING
	pcb[CRP].state= RUNNING;
//	cons_printf("setting state to rnunning\n");
//cons_printf("leaving crp\n");
}

void Kernel(TF_t *TF_ptr) {
	
   int pid;
   
//   change state of CRP to kernel mode
//cons_printf("setting kmode\n");
	pcb[CRP].mode = KMODE;
//cons_printf("in kernel now\n");
//save tf_t to pcb[crp].tf_t
	pcb[CRP].TF_ptr = TF_ptr;
	
//   call timer ISR to service timer interrupt (as if it just occurred)
	switch(TF_ptr->intr_num){
		case TIMER_INTR: TimerISR(); break;
		case SLEEP_INTR:
			SleepISR(TF_ptr->ebx);
			break;
		case GETPID_INTR:
			GetPidISR();		
			break;
		default: 
		printf("dont PANIC!!!!!!!");
		//breakpoint();
		break;
	
	}

//cons_printf("just before if statement\n");     
if(cons_kbhit()){
	char key = cons_getchar();
	switch(key){
		case 'n':
			if(EmptyQ(&none_q) ) cons_printf("No more process!\n");
			else {
				pid = DeQ(&none_q); 
				CreateISR(pid);
			}
	   		break;	
		case 't':TerminateISR(); break;
		case 'b':breakpoint(); break;
		case 'q': exit(0); 
	}		
}  
//   call SelectCRP() to settle/determine for next CRP
//cons_printf("calling crp\n");
	SelectCRP();
	//breakpoint();
//cons_printf("calling dispatch\n");
	cons_printf("CRP %d\n", CRP);
	Dispatch(pcb[CRP].TF_ptr);
//cons_printf("dispatch called\n");
}

