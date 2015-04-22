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
			case 'p':MsgSend(&msg);  break;
			case 'b':breakpoint(); break;
			case 'q': exit(0);
			}
		}

	}//while
}
/*------------------------------Phase 6 -----------------------------------------------------*/
void shell () {
   int BAUD_RATE, divisor;         // for serial port
   msg_t msg;                      // local message space
   char login[101], password[101]; // login and password strings entered
   int STDIN = 4, STDOUT = 5;      // PID's of these processes

  // initialize terminal interface data structure (below)
   MyBzero((char *) &terminal.TX_q,sizeof(q_t));
   MyBzero((char *) &terminal.RX_q,sizeof(q_t));
   MyBzero((char *) &terminal.echo_q,sizeof(q_t));//clear 3 queues: TX_q, RX_q, echo_q
   terminal.TX_sem = SemGet(Q_SIZE);   //get a semaphore to set TX_sem, count Q_SIZE (char space to terminal video)
   terminal.RX_sem = SemGet(0);   //get a semaphore to set RX_sem, count 0 (no char from terminal KB)
   terminal.echo = 1;   //set echo to 1 (default is to echo back whatever typed from terminal)
   terminal.TX_extra = 1;   //set TX_extra to 1 (an IRQ3 TXRDY event missed)


    /*
   // COM1-8_IOBASE: 0x3f8 0x2f8 0x3e8 0x2e8 0x2f0 0x3e0 0x2e0 0x260
   // transmit speed 9600 bauds, clear IER, start TXRDY and RXRDY
   // Data communication acronyms:
   //    IIR Intr Indicator Reg
   //    IER Intr Enable Reg
   //    ETXRDY Enable Xmit Ready
   //    ERXRDY Enable Recv Ready
   //    MSR Modem Status Reg
   //    MCR Modem Control Reg
   //    LSR Line Status Reg
   //    CFCR Char Format Control Reg
   //    LSR_TSRE Line Status Reg, Xmit+Shift Regs Empty
   */
   // set baud rate 9600
   BAUD_RATE = 9600;              // Mr. Baud invented this
   divisor = 115200 / BAUD_RATE;  // time period of each baud
   outportb(COM2_IOBASE+CFCR, CFCR_DLAB);          // CFCR_DLAB 0x80
   outportb(COM2_IOBASE+BAUDLO, LOBYTE(divisor));
   outportb(COM2_IOBASE+BAUDHI, HIBYTE(divisor));
   // set CFCR: 7-E-1 (7 data bits, even parity, 1 stop bit)
   outportb(COM2_IOBASE+CFCR, CFCR_PEVEN|CFCR_PENAB|CFCR_7BITS);
   outportb(COM2_IOBASE+IER, 0);
   // raise DTR, RTS of the serial port to start read/write
   outportb(COM2_IOBASE+MCR, MCR_DTR|MCR_RTS|MCR_IENABLE);
   IO_DELAY();
   outportb(COM2_IOBASE+IER, IER_ERXRDY|IER_ETXRDY); // enable TX, RX events
   IO_DELAY();	
   
   while(1){ //inifite loop: 
   
     while(1){ // loop a
     
      // prompt valid commands (send msg to STDOUT, receive reply)
       MyStrCpy(msg.data, "whoami, bye");
       msg.recipient = STDOUT;
       MsgSend(&msg);
       MsgRecieve(&msg);
      
       //  prompt for login (send msg to STDOUT, receive reply)
       MyStrCpy(msg.data, "login: ");
       msg.recipient = STDOUT;
       MsgSend(&msg);
       MsgRecieve(&msg);
       
       //  get login entered (send msg to STDIN, receive reply)
	msg.recipient = STDIN;
	MsgSend(&msg);
	MsgRecieve(&msg);
	MyStrCpy(login, msg.data);
	
       //  prompt for password (same as above)
       MyStrCpy(msg.data, "password: ");
       msg.recipient = STDOUT;
       MsgSend(&msg);
       MsgRecieve(&msg);
       
       //  get password entered (same as above)
       msg.recipient = STDIN;
       MsgSend(&msg);
       MsgRecieve(&msg);
       MyStrCpy(password, msg.data);
       
        
       if(MyStrCmp(login, password)) break; // string-compare login and password; if same, break loop A
       
       else {  //(else) prompt "Invalid login!\n\0"
       	MyStrCpy(msg.data, "Invalid login! ");
       	msg.recipient = STDOUT;
       	MsgSend(&msg);
       	MsgRecieve(&msg);
       }
       
     } //end of loop a
     while(1) {//loop B:
     //prompt for entering command string
     MyStrCpy(msg.data, "enter command: "); 
     msg.recipient = STDOUT;
     MsgSend(&msg);
     MsgRecieve(&msg);
    
     //get command string entered
     msg.recipient = STDIN;
     MsgSend(&msg);
     MsgRecieve(&msg);
     //   if command string is empty, then continue (loop B)
      if(MyStrLen(msg.data) == 0) continue;
      else if(MyStrCmp(msg.data, "bye\0")) break; //if command string is "bye", then break (loop B)
      else if (MyStrCmp(msg.data, "whoami\0")) { //if command string is "whoami"
       //show login string,
       MyStrCpy(msg.data,login);
       msg.recipient = STDOUT;//not sure if this is needed here
       MsgSend(&msg);
       MsgRecieve(&msg);
       
       //and an additional "\n\0" (for aesthetics)
       msg.recipient = STDOUT;//doubly sure
       MyStrCpy(msg.data, "\n\0");
       MsgSend(&msg);
       MsgRecieve(&msg);
       continue; // continue (loop B)
      } //if command string is "whoami" (end)
      else { //else other strings are entered in command string
   	//show "Command not found!\n\0"
   	MyStrCpy(msg.data, "Command not found!\n\0");
   	msg.recipient = STDOUT;
   	MsgSend(&msg);
   	MsgRecieve(&msg);
      }
     }// if command string is empty block (end)
   }//  repeat loop B
} //  repeat infinite loop
   


void STDIN(){
char *p, ch;
msg_t msg;
	while(1){
		MsgRecieve(&msg);
		p=msg.data;
		while(1){
			SemWait(terminal.RX_sem);
			ch = (char)DeQ(&terminal.RX_q);
			if(ch=='\r') break;
			*p++ = ch;
		}
		*p = '\0';
		msg.recipient = msg.sender;
		MsgSend(&msg);
	}

}
	
void STDOUT(){
char *p;
msg_t msg;
	while(1){
		MsgRecieve(&msg);
		*p = msg.data;
		while(1){
			SemWait(terminal.TX_sem);
			EnQ((int) p, &terminal.TX_q);
			TipIRQ3();
			if(*p=='\n'){
				SemWait(terminal.TX_sem);
				EnQ('\r', &terminal.TX_q);
			}		
		p++;
		}
	msg.recipient = msg.sender;
	MsgSend(&msg);
	}
	
}
