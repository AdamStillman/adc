// isr.c, 159

#include "spede.h"
#include "type.h"
#include "isr.h"
#include "tool.h"
#include "extern.h"
#include "proc.h"
//#include "Tf.h"

void CreateISR(int pid) {
//cons_printf("in create ISR\n");
	if(pid!=0) EnQ(pid, &run_q);  // if pid is not for Idle (0), enqueue pid to run queue
	pcb[pid].mode = UMODE;   //mode should be set to UMODE
	pcb[pid].state = RUN;   //state should be set to RUN
	pcb[pid].runtime = pcb[pid].total_runtime = 0;   //both runtime counts should be set 0
	
	
	//added code
	MyBzero(stack[pid], STACK_SIZE); //erase stack
	
	pcb[pid].TF_ptr = (TF_t *)&stack[pid][STACK_SIZE];
	pcb[pid].TF_ptr--;
	//point above the stack then drop by sizeof(TF_t)
	if(pid == 0){
		pcb[pid].TF_ptr->eip = (unsigned int)Idle; // Idle process
	//	cons_printf("in idle\n");
	}
	else {
		pcb[pid].TF_ptr->eip = (unsigned int)UserProc; // other new process
	//	cons_printf("in user process\n");
	}	
		//fillout trapframe of new proc
   pcb[pid].TF_ptr->eflags = EF_DEFAULT_VALUE | EF_INTR;
   pcb[pid].TF_ptr->cs = get_cs();
   pcb[pid].TF_ptr->ds = get_ds();
   pcb[pid].TF_ptr->es = get_es();
   pcb[pid].TF_ptr->fs = get_fs();
   pcb[pid].TF_ptr->gs = get_gs();
	
}

void TerminateISR() {
	if(CRP < 1) return;   //just return if CRP is 0 or -1 (Idle or not given)     
	pcb[CRP].state = NONE;   //change the state of CRP to NONE
	EnQ(CRP, &none_q);   //and queue it to none queue
	CRP = -1;   //set CRP to none (-1)
}        

void TimerISR() {
//cons_printf("in timerISR\n");
	outportb(0x20, 0x60); 

	if(CRP == 0) return;   //just return if CRP is Idle (0) or less (-1)

	pcb[CRP].runtime++;   //upcount the runtime of CRP

	if(pcb[CRP].runtime == TIME_LIMIT){   //if the runtime reaches the set time limit
	pcb[CRP].runtime = 0;   //(need to rotate to the next in run queue)
	pcb[CRP].total_runtime += TIME_LIMIT;      //simply total up the total runtime of CRP
	EnQ(CRP, &run_q);      //queue it to run queue
	pcb[CRP].state = RUN;      //change its state to RUN
	CRP = -1;     // reset CRP (to -1, means none)
	}
}
