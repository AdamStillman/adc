// isr.h, 159
#ifndef _ISR_H_
#define _ISR_H_

void CreateISR(int);
void TerminateISR();
void TimerISR();
void SleepISR();
void GetPidISR();
void SemWaitISR();
void SemPostISR(int);
void SemGetISR();
void IRQ7ISR();
void IRQ3ISR();
void MsgSendISR();
void MsgRecieveISR();
void IRQ3TX();
void IRQ3RX();

#endif
