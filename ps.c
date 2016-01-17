#include<stdio.h>
#include<echo.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>
#include<fcntl.h>
#include<errno.h>
#include<sys/types.h>

void pinfo_func(char **final, char shell[])
{
	if(final[1] != NULL)
	{
		char comm[1024] = "/proc/";
		char comme[1024] = "/proc/";
		char commstat[1024] = "/status";
		char commexec[1024] = "/cmdline";
		char mem[100],temp2[100],temp3[100];
		strcat(comm,final[1]);
		strcat(comme,final[1]);
		strcat(comm,commstat);
		strcat(comme,commexec);
		size_t buflen = 1024;
		char buff[buflen];
		char status[100];
		FILE * fp;
		fp = fopen(comme,"r");
		fgets(buff,1024,fp);
		fclose(fp); 
		fp = fopen (comm, "r");
		char temp5[100];
		int temp = 0; 
		while(temp != 1)
		{
			fgets(status,100,fp);
			temp++;
		}
		fscanf(fp,"%s %s",mem,temp2);
		while(1)
		{
			fgets(mem,100,fp);
			if(strstr(mem,"VmPeak") !=NULL)
			{
				break;
			}
		}
		fscanf(fp,"%s %s",status,temp3);
		printf("Pid : %s\nStatus : %s\nMemory : %s\nExecutable Path : %s\n",final[1],temp2,temp3,buff);
		fclose(fp);
//		printf("%s",shell);
	}
	else
	{
		FILE * fp;
		char buff[1024] = "~/./a.out";
		char status[100];
		char mem[100];
		int pid_curr = getpid();
		fp = fopen ("/proc/self/status", "r");
		char temp2[100],temp3[100],temp4[100],temp5[100];
		int temp = 0;
		while(temp != 1)
		{
			fgets(status,100,fp);
			temp++;
		}
		fscanf(fp,"%s %s",status,temp2);
		while(1)
		{
			fgets(mem,100,fp);
			if(strstr(mem,"VmPeak") != NULL)
			{
				break;
			}
		}
		fscanf(fp, "%s %s",mem,temp3);
		printf("Pid : %d\nStatus : %s\nMemory : %s\nExecutable Path : %s\n",pid_curr,temp2,temp3,buff);
		fclose(fp);
//		printf("%s",shell);
	}
}
