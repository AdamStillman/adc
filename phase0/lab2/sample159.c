//sample cs159
#include <spede/stdio.h>
#include <spede/flames.h>
void DisplayMsg();

	int main(void)
	{	
	long i=111;
	int a=0;	
	for(a=0; a<5; a++)
		{
		printf("%d Hello wolrd %d \nECS",i,i*2);
		cons_printf("---> Hello World <---\nCPE/CSC"); 
		i++;
		}
	DisplayMsg();
	return 0;	
	}
	

	void DisplayMsg()
	{
		printf("subroutine\n");
		cons_printf("subroutine\n");
		return 0;
	}
