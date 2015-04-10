// syscall.h

#ifndef _SYSCALL_H_
#define _SYSCALL_H_
#include "type.h"

int GetPid();      // no input, 1 return
void Sleep(int);   // 1 input, no return
void SemWait(int);
void SemPost(int);
int SemGet(int);
void MsgSend(int, msg_t *);
void MsgRecieve(msg_t *);

#endif
