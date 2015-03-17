/****************************************
isr.c, phase 0, exercise 3 -- timer lab
****************************************/
#include <spede/flames.h>
#include <spede/machine/io.h>
#include <spede/machine/pic.h>
//2-byte (unsigned short) ptr to point to a VGA displayer char
//at row 10, col 40 (80 char per row), from base b8000
unsigned short *vid_mem_ptr = (unsigned short *) 0xB8000+10*80+39;
#define SPACE (0x0f00 + ' ' )
void TimerISR() {
	static int ch = SPACE;
	static int timer_count = 0;
	outportb(0x20, 0x60);
	if(++timer_count % 75 == 0 ){
		*vid_mem_ptr = ch;
		ch++;
		if(ch==SPACE +95);
			ch = SPACE;
	}
}
