// isr.c, 159
//changes made and working
#include "spede.h"
#include "type.h"
#include "isr.h"
#include "tool.h"
#include "extern.h"
#include "proc.h"
#include "syscall.h"

int wakingID;

void CreateISR(int pid) {
//cons_printf("in create ISR\n");
	if(pid!=0) {
	pcb[pid].mode = UMODE;   //mode should be set to UMODE
	pcb[pid].state = RUN;   //state should be set to RUN
	pcb[pid].runtime = pcb[pid].total_runtime = 0;   //both runtime counts should be set 0
	EnQ(pid, &run_q);  // if pid is not for Idle (0), enqueue pid to run queue
	}
	
	//added code
	MyBZero(stack[pid], STACK_SIZE); //erase stack
	//phase5
	MyBZero(mbox[pid], sizeof( mbox_t)); //clear the mbox

	
	pcb[pid].TF_ptr = (TF_t *)&stack[pid][STACK_SIZE];
	pcb[pid].TF_ptr--;
	//point above the stack then drop by sizeof(TF_t)
	if(pid == 0){
		pcb[pid].TF_ptr->eip = (unsigned int)Idle; // Idle process
	//	cons_printf("in idle\n");
	}
	else if(pid==1){
		pcb[pid].TF_ptr->eip = (unsigned int) Init;
	}
	else if(pid==2){
		pcb[pid].TF_ptr->eip = (unsigned int) PrintDriver;
	}
	else pcb[pid].TF_ptr->eip = (unsigned int) UserProc;

		//fillout trapframe of new proc
   	pcb[pid].TF_ptr->eflags = EF_DEFAULT_VALUE | EF_INTR;
 	pcb[pid].TF_ptr->cs = get_cs();
   	pcb[pid].TF_ptr->ds = get_ds();
   	pcb[pid].TF_ptr->es = get_es();
   	pcb[pid].TF_ptr->fs = get_fs();
   	pcb[pid].TF_ptr->gs = get_gs();
	
}

void TerminateISR() {

	if(CRP == 0 || CRP == -1) return;   //just return if CRP is 0 or -1 (Idle or not given)     
	else{
	pcb[CRP].state = NONE;   //change the state of CRP to NONE
	EnQ(CRP, &none_q);   //and queue it to none queue
	CRP = -1;   //set CRP to none (-1)
	}
}        

void TimerISR() {
//cons_printf("in timerISR\n");
	outportb(0x20, 0x60); 
	
	sys_time++;
	pcb[CRP].runtime++;   //upcount the runtime of CRP


	while(sleep_q.size != 0 && pcb[sleep_q.q[sleep_q.head]].wake_time <= sys_time) {
		wakingID = DeQ(&sleep_q);
		pcb[wakingID].state = RUN;
		EnQ(wakingID, &run_q);
	}
	

	if(CRP <= 0) return;   //just return if CRP is Idle (0) or less (-1)
	
	if(pcb[CRP].runtime == TIME_LIMIT){   //if the runtime reaches the set time limit
		pcb[CRP].total_runtime += TIME_LIMIT;      //simply total up the total runtime of CRP
		 pcb[CRP].runtime = 0;   //(need to rotate to the next in run queue)
		 pcb[CRP].state = RUN;
		 EnQ(CRP, &run_q);      //queue it to run queue
		 CRP = -1;     // reset CRP (to -1, means none)
	}
}

void SleepISR(int time_sleep){
	
	pcb[CRP].wake_time = (sys_time + time_sleep * 100);
	EnQ(CRP, &sleep_q);
	pcb[CRP].state = SLEEP;
	CRP = -1;
	return;
}

void GetPidISR(){
	pcb[CRP].TF_ptr->ebx = CRP;
	//EnQ(CRP, &run_q);
	//pcb[CRP].state = RUN;
	//GetPid();
	return;
}

void SemWaitISR(){
  //if semaphore queue is greater than 0 it will decrement the count
  int semaphoreID = pcb[CRP].TF_ptr->ebx;
  if(semaphore[semaphoreID].count > 0){
   semaphore[semaphoreID].count--;
  }
  //if semaphore queue is equal to 0 the current running process will be queued
  else {
    EnQ(CRP, &(semaphore[semaphoreID].wait_q));
    pcb[CRP].state = WAIT;
    CRP=-1;
  }
}

void SemPostISR(){
   int semaphoreID = pcb[CRP].TF_ptr->ebx;
   if(semaphore[semaphoreID].wait_q.size == 0){
     semaphore[semaphoreID].count++;
   } else {
     int temp = DeQ(&(semaphore[semaphoreID].wait_q));
     EnQ(temp, &run_q);				
     pcb[temp].state = RUN;     

   }
}

void SemGetISR(){
	//count in ebx
	int count = pcb[CRP].TF_ptr->ebx;
	int sid = DeQ(&semaphore_q);//semaphore_q
	if(sid>=0){
	MyBzero((char *)&semaphore[sid], sizeof(semaphore_t));
	semaphore[sid].count = count;
	pcb[CRP].TF_ptr->ecx= sid;
	}

}

void IRQ7ISR(){
//int pid;
int temp;
outportb(0x20, 0x67);
//pid = pcb[CRP].TF_ptr->ebx;
if(semaphore[print_semaphore].wait_q.size == 0) semaphore[print_semaphore].count++;
else{
	temp = DeQ(&(semaphore[print_semaphore].wait_q));
	EnQ(temp, &run_q);
	pcb[temp].state = RUN;
    }
}

void MsgSendISR(){
	int mid;
	msg_t *local_msg;
	local_msg = (msg_t *) pcb[CRP].TF_ptr ->ebx;
	
	if( !Emptyq(&mbox[CRP].wait_q) ){
		mbox[CRP].msg_q.msg.sender = CRP;
		mbox[CRP].msg_q.msg.time_stamp = sys_time;
		MsgEnQ()
	}
	
	
}

void MsgRecieveISR(){
	msg_t local_msg;	
	
	
}


