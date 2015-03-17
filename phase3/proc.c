// proc.c, 159
// processes are here

#include "spede.h"   // for IO_DELAY() needed here below
#include "extern.h"  // for current_run_pid needed here below
#include "proc.h"    // for Idle, SimpleProc, DispatchProc
#include "syscall.h"
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

void Producer(){
  int i;
  int pid, sec;
  pid = GetPid();
  while(1){
    sec = 4 - (pid%4);
    SemWait(product_semaphore); //sem-wait product semaphore
    cons_printf("Proc %d is producing...",pid); //show msg: "Proc (pid #) is producing ..."
    product += 100; //increment product by 100
    cons_printf("+++ product is now %d \n", product); //show msg: "+++ product is now (product #) \n"
    SemPost(product_semaphore); //sem-post product semaphore
    for(i=0;i<1666000; i++) IO_DELAY(); //IO_DELAY() for a second
    Sleep(sec);
  }
}

void Consumer() {
  int i;
  int pid, sec;
  pid = GetPid();
  while (1){
    sec = 4 - (pid%4);
    SemWait(product_semaphore); //sem-wait product semaphore
    cons_printf("Proc %d is consuming...", pid); //show msg: "Proc (pid #) is consuming..."
    product -= 100; //decrement product by 100
    cons_printf("--- product is now %d \n", product);//show msg: "--- product is now (product #)\n"
    SemPost(product_semaphore);//sem-post product semaphore
    for(i=0; i<1666000; i++) IO_DELAY();//IO_DELAY() for a second
    Sleep(sec);
  }
}
