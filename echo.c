#include<stdio.h>

void echo_func(char **final, char shell[])
{
	 int u;
	 for(u=1;u<100;u++)
	 {
	 	if(final[u] != NULL)
		{
			printf("%s ",final[u]);
		}
	 }
	 printf("\n");
}

