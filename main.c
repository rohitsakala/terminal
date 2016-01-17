#include<stdio.h>
#include<echo.h>
#include<ps.h>
#include<string.h>
#include<signal.h>
#include<unistd.h>
#include<stdlib.h>
#include<fcntl.h>
#include<errno.h>
#include<sys/types.h>

// Global Variables
int pids[100],pidcount=0;
char *pidsname[100];
int flag,j=0;
int inin = 0,inout = 0,kin = -1,kout = -1;
int double_re = 0;
char shell[1024];
char *final[100];
int shell_terminal;	
int shell_pgid;
pid_t pid;
int print_n = 0;

// Main run command function
void runthecommand(int print_n,char homedirectory[],char username[],char systemname[])
{
	/* Set the handling for job control signals back to the default.  */
	if(pid == 0)
	{
		signal (SIGINT, SIG_DFL);
	    signal (SIGTTIN, SIG_DFL);
	    signal (SIGTTOU, SIG_DFL);
	}
   	signal (SIGCHLD, SIG_DFL);
	signal (SIGQUIT, SIG_DFL);
	signal (SIGTSTP, SIG_DFL);
	int tempv = j;
	while(j<10)
	{
		final[j]=NULL;
		j++;
	}
	if(final[0] != NULL)
	{
		int stdin_copy = dup(0);
		int stdout_copy = dup(1);
		if(inin != 0)
		{
			close(0);
			if(strlen(final[inin])==0)
			{
				write(1,"bash: syntax error near unexpected token 'newline'\n",51);
				return;
			}
			kin=open(final[inin],O_RDONLY);
		}
		if(inout != 0)
		{
			if (strlen(final[inout])==0)
			{
				write(1,"bash: syntax error near unexpected token `newline'\n",51);
				return;
			}
			close(1);
			if(double_re == 0)
			{
				kout=open(final[inout],O_CREAT|O_WRONLY|O_TRUNC,S_IRWXU);
			}
			else
			{
				kout=open(final[inout],O_CREAT|O_WRONLY|O_APPEND,S_IRWXU);
			}
			if(kout<0)
			{
				write(1,"bash: syntax error near unexpected token `newline'\n",51);
			}
		}
		if(pid<0)
		{
			perror("Child Proc. not created\n");
			_exit(-1);
		}
		else if(pid==0)
		{
			if(flag == 0)
			{
			pid = getpid ();
			int pgid = getpgrp();
      		if (pgid == 0) pgid = pid;
      		setpgid (pid, pgid);
      		tcsetpgrp (shell_terminal, pgid);
      		signal (SIGINT, SIG_DFL);
      		signal (SIGQUIT, SIG_DFL);
      		signal (SIGTSTP, SIG_DFL);
      		signal (SIGTTIN, SIG_DFL);
      		signal (SIGTTOU, SIG_DFL);
      		signal (SIGCHLD, SIG_DFL);
			kill (- getpgid(getpid()), SIGCONT);
			}
			else
			{
				setpgid(pid,pid);
				kill (- getpgid(getpid()), SIGCONT);
			}
			int ret=0;
			int u = 0;
			if(inin != 0)
			{
				for(u = inin; u < tempv; u++)
				{
					final[u] = NULL;
				}
			}
			if(inout != 0)
			{
				for(u = inout; u < tempv; u++)
				{
					final[u] = NULL;
				}
			}
			if(strcmp(final[0],"cd") == 0)
			{
				int result;
				if(final[1] != NULL)
				{
					result = chdir(final[1]);
				}
				else
				{
					result = chdir(getenv("HOME"));
				}
				if(result == 0)
				{
					char temp[1024];
					getcwd(temp,1024);
					int a,b,c;
					a = strlen(temp);
					b = strlen(homedirectory);
					c = a - b;
					if(c > 0)
					{
						sprintf(shell, "<%s@%s:~%s>",username,systemname,temp + strlen(temp) - c);	
					}
					else if(c < 0)
					{
						sprintf(shell, "<%s@%s:%.*s>",username,systemname,-c,homedirectory + strlen(homedirectory) + c);
					}
					else
					{
						sprintf(shell, "<%s@%s:~>",username,systemname);
					}
				}
				else
				{
					perror("couldn't execute command\n");
				}
			}
			else if(strcmp(final[0],"pwd") == 0)
			{
				char temp[1024];
				getcwd(temp,1024);
				printf("%s\n",temp);
				_exit(0);
			}
			else if(strcmp(final[0],"echo") == 0)
			{
				echo_func(final,shell);
				_exit(0);
			}
			else if(strcmp(final[0],"pinfo") == 0)
			{
				pinfo_func(final,shell);
				_exit(0);
			}
			else if(strcmp(final[0],"jobs") == 0)
			{
				if(final[1] != NULL && final[2] != NULL && final[3] != NULL)
				{
					if(strcmp(final[1],"-kjob") == 0)
					{
						int jobid = atoi (final[2]);
						int sigid = atoi (final[3]);
						if(sigid == 9)
						{
							int l;
							for(l=0;l<pidcount;l++)
							{	
								if(l == jobid)
								{
									kill(pids[l],SIGKILL);
								}
							}
						}
						if(sigid == 2)
						{
							int l;
							for(l=0;l<pidcount;l++)
							{	
								if(l == jobid)
								{
									 kill(pids[l],SIGINT);
								}
							}
						}
						if(sigid == 3)
						{
							int l;
							for(l=0;l<pidcount;l++)
							{	
								if(l == jobid)
								{
									 kill(pids[l],SIGQUIT);
								}
							}
						}
					}
				}
				else if(final[1] != NULL && final[2] != NULL && final[3] == NULL)
				{
					if(strcmp(final[1],"-fg") == 0)
					{
						int jobid = atoi (final[2]);
						int l;
						for(l=0;l<pidcount;l++)
						{
							if(jobid == l)
							{
								tcsetpgrp (shell_terminal,getpgid(pids[l]));
								kill(pids[l],SIGCONT);
							}
						}
					}
				}
				else if(final[1] != NULL && final[2] == NULL && final[3] == NULL)
				{
					if(strcmp(final[1],"-overkill") == 0)
					{
						int l;
						for(l=0;l<pidcount;l++)
						{	
							kill(pids[l],SIGKILL);
						}
					}
				}
				else
				{
					int l;
					for(l=0;l<pidcount;l++)
					{
						int status;
						pid_t result = waitpid(pids[l], &status, WUNTRACED|WNOHANG);
						if (result == -1) {
							printf("[%d]  %d  %s  Running\n",l,pids[l],pidsname[l]);
						}	
					}
				}
				exit(0);
			}
			else
			{
				ret=execvp(final[0],final);
				if(ret<0)
				{
					perror("couldn't execute command\n");
					_exit(-1);
				}
				printf("Giddy! Up..\n");
				_exit(0);
			}
		}
		if(kout != -1)
		{
			close(kout);
			dup2(stdout_copy, 1);
			close(stdout_copy);
		}
		if(kin != -1)
		{
			close(kin);
			dup2(stdin_copy,0);
			close(stdin_copy);
		}
	}
	else
	{
		printf("%s",shell);
	}
	j = 0;
	// Reseting varibales for redirection
	kin = -1;
	kout = -1;
	inin = 0;
	inout = 0;
	
}



int main()	
{

	char homedirectory[1024];
	getcwd(homedirectory, 1024);
	char home[1024] = "HOME=";
	strcat(home,homedirectory);
	putenv(home);
	char *username = getlogin();
	char systemname[1024];
	gethostname(systemname,1024);
	char command[100][100];
	int i=0;
	int pipecount = 0;
	sprintf(shell, "<%s@%s:~>",username,systemname);
	printf("<%s@%s:~>",username,systemname);
	shell_terminal = STDIN_FILENO;
	while (tcgetpgrp (shell_terminal) != (shell_pgid = getpgrp ()))
	{
        kill (- shell_pgid, SIGTTIN);
    }
    signal (SIGINT, SIG_IGN);
    signal (SIGQUIT, SIG_IGN);
    signal (SIGTSTP, SIG_IGN);
    signal (SIGTTIN, SIG_IGN);
    signal (SIGTTOU, SIG_IGN);
    signal (SIGCHLD, SIG_IGN);
    shell_pgid = getpid ();
    if (setpgid (shell_pgid, shell_pgid) < 0)
    {
        perror ("Couldn't put the shell in its own process group");
        exit (1);
    }
    tcsetpgrp (shell_terminal, shell_pgid);
	while(1)
	{
		i=0;
		j=0;
		while(1)
		{
			scanf("%c",&command[i][j]);
			if(command[i][j] == ' ' || command[i][j] == '	')
			{
				command[i][j] = '\0';
				i++;
				j=0;
			}
			else if(command[i][j] == '\n')
			{
				command[i][j] = '\0';
				i++;
				j=0;
				command[i][j++]=';';
				command[i][j]='\0';
				break;
			}
			else if(command[i][j] == ';')
			{
				command[i][j] = '\0';
				i++;
				j=0;
				command[i][j++]=';';
				command[i][j]='\0';
				i++;
				j=0;
			}
			else if(command[i][j] == '|')
			{
				pipecount++;
				j++;
			}
			else
			{
				j++;
			}
		}	
		i=0;
		j=0;
		double_re = 0;
		flag = 0;
		int pipeline[pipecount][2];
		for(i=0;i<pipecount;i++)
		{
			pipe(pipeline[i]);
		}
		i=0;
		int countc = 0;
		int flagcounter[10],counter = 0;
		while(i<12)
		{
			if(strcmp(command[0],"") != 0)
			{
				if(strcmp(command[i],";") != 0)
				{
					if(strlen(command[i]) > 0)
					{
						if(strcmp(command[i],"&") == 0)
						{
							flag = 1;
						}
						else
						{
							final[j] = strdup(command[i]);
							if(strcmp(final[j],"<") == 0)
							{
								inin = j;
							}
							else if(strcmp(final[j],"|") == 0)
							{
								flagcounter[counter++] = j;
							}
							else if(strcmp(final[j],">") == 0 || strcmp(final[j],">>") == 0)
							{
								inout = j;
								if(strcmp(final[j],">>") == 0)
								{
									double_re = 1;
								}
							}
							else
							{
								j=j+1;
							}
						}
					}
				}
				else
				{
					int stdin_copy = dup(0);
					int stdout_copy = dup(1);
					int x = 0;
					if(pidcount != 0)
					{
						int l;
						int tempid[100];
						char *tempname[100];
						int tempcount=0;
						for(l=0;l<pidcount;l++)
						{
							int status;
							pid_t result = waitpid(pids[l], &status, WNOHANG);
							if (result == 0) {
								tempid[tempcount] = pids[l];
								tempname[tempcount] = pidsname[l];
								tempcount++;
							} else if (result == -1) {
							} else {
								printf("[%d]  %d  %s  Done\n",l,pids[l],pidsname[l]);
								pidcount--;
							}
						}
						int p;
						for(p=0;p<tempcount;p++)
						{
							pids[p] = tempid[p];
							pidsname[p] = tempname[p];
						}
						pidcount = tempcount;
					}
					if(counter != 0)
					{
						pid = fork();
						if(pid == 0)
						{
							close(pipeline[countc][0]);
							dup2(pipeline[countc][1], 1);
							int kk,ll;
							for(kk = 0;kk<pipecount;kk++)
							{
								for(ll = 0;ll<2;ll++)
								{
									close(pipeline[kk][ll]);
								}
							}
							print_n = 0;
							for(kk=flagcounter[x];kk<j;kk++)
							{
								final[kk] =NULL;
							}
							if(inin != 0)
							{
								if(inin >= flagcounter[x])
								{
									inin = 0;
								}
							}
							if(inout != 0)
							{
								if(inout >= flagcounter[x])
								{
									inout = 0;
								}
							}
							runthecommand(print_n,homedirectory,username,systemname);
						}
						else
						{
							while(x < counter-1)
							{
								x++;
								pid = fork();
								if(pid == 0)
								{
									close(pipeline[countc][1]);
									dup2(pipeline[countc][0], 0);
									countc++;
									close(pipeline[countc][0]);
									dup2(pipeline[countc][1], 1);
									int kk,ll;
									for(kk = 0;kk<pipecount;kk++)
									{
										for(ll = 0;ll<2;ll++)
										{
											close(pipeline[kk][ll]);
										}
									}
									print_n = 0;
									char *temp[100];
									int tempcounter = 0;
									for(kk=flagcounter[x-1];kk<flagcounter[x];kk++)
									{
										temp[tempcounter++] = final[kk];
									}
									for (kk = 0; kk < tempcounter; kk++) 
									{
							     	 	final[kk] = temp[kk];
							   		}
							 	  	for(kk=tempcounter;kk<j;kk++)	
									{
										final[kk] =NULL;
									}
									if(inin != 0)
									{	
										if(inin >= flagcounter[x] || inin <= flagcounter[x-1])
										{
											inin = 0;
										}
										else
										{
											inin = inin - flagcounter[x-1];
										}
									}
									if(inout != 0)
									{
										if(inout >= flagcounter[x] || inout <= flagcounter[x-1])
										{
											inout = 0;
										}
										else
										{
											inout = inout - flagcounter[x-1];
										}
									}
									runthecommand(print_n,homedirectory,username,systemname);
								}
								else
								{
									countc++;
								}
							}
							pid = fork();
							if(pid == 0)
							{
								close(pipeline[countc][1]);
								dup2(pipeline[countc][0], 0);
								int kk,ll;
								for(kk = 0;kk<pipecount;kk++)
								{
									for(ll = 0;ll<2;ll++)
									{
										close(pipeline[kk][ll]);
									}
								}
								print_n = 0;
								char *temp[100];
								int tempcounter = 0;
								for(kk=flagcounter[x];kk<j;kk++)
								{
									temp[tempcounter++] = final[kk];
								}
								for (kk = 0; kk < tempcounter; kk++) {
							      	final[kk] = temp[kk];
							   	}
							   	for(kk=tempcounter;kk<j;kk++)
								{
									final[kk] =NULL;
								}
								if(inin != 0)
								{	
									if(inin <= flagcounter[x])
									{
										inin = 0;
									}
									else
									{
										inin = inin - flagcounter[x];
									}
								}
								if(inout != 0)
								{
									if(inout <= flagcounter[x])
									{
										inout = 0;
									}
									else
									{
										inout = inout - flagcounter[x];
									}
								}
								runthecommand(print_n,homedirectory,username,systemname);
							}
						}
					}
					else
					{
						pid=fork();
						if(pid == 0)
						{
							if(countc > 0)
							{
								dup2(pipeline[countc][0],0);
								int kk,ll;
								for(kk = 0;kk<pipecount;kk++)
								{
									for(ll = 0;ll<2;ll++)
									{
										close(pipeline[kk][ll]);
									}
								}
							}
							print_n = 0;
							runthecommand(print_n,homedirectory,username,systemname);
						}
						else
						{
							print_n = 1;
							runthecommand(print_n,homedirectory,username,systemname);
						}
					}
					int kk,ll;
					for(kk = 0;kk<pipecount;kk++)
					{
						for(ll = 0;ll<2;ll++)
						{
							close(pipeline[kk][ll]);
						}
					}
					close(kout);
					close(kin);
					dup2(stdin_copy,0);
					dup2(stdout_copy,1);
					close(stdin_copy);
					close(stdout_copy);
					signal(SIGTTOU, SIG_IGN);
					if(flag == 1)
					{
						kill(pid,SIGCONT);
						tcsetpgrp (shell_terminal, shell_pgid);
						pids[pidcount] = pid;
						pidsname[pidcount] = final[0];
						pidcount++;
						printf("%s",shell);
					}
					else
					{
						if(counter > 0)
						{
							int kk;
							for(kk = 0;kk< counter+1;kk++)
							{
								wait();
							}
						}
						else
						{
							if(final[1] != NULL)
							{
								if(strcmp(final[0],"jobs") == 0 && strcmp(final[1],"-fg") == 0)
								{
							
									wait();
								}
							}
					
							wait();
							tcsetpgrp (shell_terminal, getpid());
						}
						printf("%s",shell);
					}
					j = 0;
					// Reseting varibales for redirection
					kin = -1;
					kout = -1;
					inin = 0;
					inout = 0;
				}
			}
			else
			{
				printf("%s",shell );
				break;
			}
			i++;
		}
		memset(command, 0, sizeof(command[0][0]) * 100 * 100);
	}
	return 0;
}
