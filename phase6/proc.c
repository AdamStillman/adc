// proc.c, 159
// processes are here

#include "spede.h"   // for IO_DELAY() needed here below
#include "extern.h"  // for current_run_pid needed here below
#include "proc.h"    // for Idle, SimpleProc, DispatchProc
#include "syscall.h"
#include <spede/machine/parallel.h>
#include "tool.h"

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
char *p;
msg_t local_msg;
//int TIME_OUT=3*1666000;                   // time out in 3 sec
print_semaphore = SemGet(-1);//should be -1 but depends on IRQISR

outportb(LPT1_BASE+LPT_CONTROL, PC_SLCTIN);// printer control select interrup
code = inportb(LPT1_BASE+LPT_STATUS);
for(i=0; i<50; i++) IO_DELAY();
outportb(LPT1_BASE+LPT_CONTROL, PC_INIT|PC_SLCTIN|PC_IRQEN);
Sleep(1);
while(1){
	cons_printf("my print pid is: %d: \n", GetPid() );
	Sleep(1);
	MsgRecieve(&local_msg);
		p = local_msg.data;
		while(*p){
			outportb(LPT1_BASE+LPT_DATA, *p);      // send char to data reg
			code = inportb(LPT1_BASE+LPT_CONTROL); // read control reg
			outportb(LPT1_BASE+LPT_CONTROL, code|PC_STROBE); // send with added strobe
			for(i=0; i<20; i++) IO_DELAY();        // delay for EPSON LP-571 printer
			outportb(LPT1_BASE+LPT_CONTROL, code); // send original control code
			SemWait(print_semaphore);
			p++;
		}//while p
}//while1



}//print driver

void Init(){
	int pid, a;
	char key;
	char my_msg[] = "Hello, my Team is called TSLK \n\0";
	msg_t msg;       // local, in process space
	msg.recipient=2;
	MyStrCpy(msg.data, my_msg); //to put greeting message (to be printed) into the msg
	while(1){// infinite loop:
     		pid=GetPid();
         	cons_printf("%d ", pid);		// print 0 on PC            show my PID
		for(a=0; a<1666000; a++) IO_DELAY();   //delay  1 sec               and sleep for 1 second ...
        
        if(cons_kbhit()){		//check if key hit
		key = cons_getchar();
			switch(key){
			//phase5
			case 'p':MsgSend(2, &msg);  break;
			case 'b':breakpoint(); break;
			case 'q': exit(0);
			}
		}

	}//while
}

void shell () {
   int BAUD_RATE, divisor;         // for serial port
   msg_t msg;                      // local message space
   char login[101], password[101]; // login and password strings entered
   int STDIN = 4, STDOUT = 5;      // PID's of these processes

  // 1st initialize terminal interface data structure (below)
  // then initialize serial port (below)

//	while(1){
   //
     // looopa:
      /// prompt valid commands (send msg to STDOUT, receive reply)
	//  prompt for login (send msg to STDOUT, receive reply)
       //  get login entered (send msg to STDIN, receive reply)
       //  prompt for password (same as above)
       //  get password entered (same as above)
        // string-compare login and password; if same, break loop A
       //  (else) prompt "Invalid login!\n\0"
   //   repeat loop A
   //   loop B:
     //    prompt for entering command string
   //      get command string entered
      //   if command string is empty {
     //       continue (loop B)
     //    }
    //     if command string is "bye" {
     //       break (loop B)
    //     }
     //    if command string is "whoami" {
       //     show login string,
       //     and an additional "\n\0" (for aesthetics)
      //      continue (loop B)
    //     }
   //      other strings {
   //         show "Command not found!\n\0"
   //      }
    //  repeat loop B
 //  repeat infinite loop
  }//loopb
}//wile1


void STDIN(){
cha *p, ch;
msg_t msg;
	while(1){
		MsgRecieve(&msg);
		*p=msg.data;
		while(1){
			SemWait(terminal.RX_sem);
			ch = DeQ(RX_q);
			if(ch=='\r') break;
			*p++ = ch;
		}
		*p = '\0';
		msg.recipient = msg.sender;
		MsgSend(&msg);
	}

}
	
void STDOUT(){
	
	
	
}
