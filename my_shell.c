#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<ctype.h>
#include<signal.h>
#include<fcntl.h>
void execute(char *command);
void shell(int);
void reset(void);
static void split(char *cmd);
static char* args[512];
char history[16][256];//宣告
int last=0; //假設7筆

int zone=0;
int fd[2];
pid_t pid;
int ret;
int fd_multiple=0;

void reset()
{
	zone=0;
	fd_multiple=0;
	fd[0]=0;
	fd[1]=0;
	args[0]=0;
}

static void removelastspace(char* s)
{
	char* tmp= strchr(s, '\0');
	tmp= tmp-1;
	if(strcmp(tmp," ")==0)
	{
		*tmp= '\0';
		removelastspace(s);
	}
}

void pipeSequence(char *args[], int pipesCount,int pipe_index,int k,int background) 
{
	ret=pipe(fd);
	if(ret==-1){
		printf("Unable to pipe\n");
		return;
	}
	pid=fork();
	if(pid==-1)
	{
		exit(EXIT_FAILURE);
	}
	else if (pid == 0)					// Child Process
	{
		dup2(fd_multiple,0);
		if(zone<pipesCount){
			dup2(fd[1],1);
		}
		close(fd[0]);

		for(int j=0;j<k;j++)
		{
			if(strcmp(args[j],">") == 0)
			{
				args[j]=NULL;
				j=j+1; 		//filename

				close(1);

				char fn[100];
				strncpy(fn,args[j],99);

				int lfd=open(fn, O_RDWR | O_TRUNC);
			
				if(lfd==-1)
				{
					open(fn, O_CREAT | O_RDWR | O_TRUNC);
				}
				args[j]=NULL;
			}	
			else if(strcmp(args[j],"<") == 0)
			{
				args[j]=NULL;
				j=j+1;			//filename
				
				close(0);
				
				char fn[100];
				strncpy(fn,args[j],99);

				int lfd=open(fn, O_RDONLY);
				
				if(lfd==-1)
				{
					open(fn, O_CREAT | O_RDWR | O_TRUNC);
				}
				args[j]=NULL;
			}		
		}
		if (strcmp(args[0],"help") == 0){
			printf("\n\t============================================\n");
			printf("\t              Type program names and arguments, and hit enter\n");
			printf("\t--------------------------------------------\n");
			printf("\t             The following are built-in function info\n");
			printf("\t             help:\tshow all build-in function info\n");
			printf("\t             cd:\tchange directory\n");
			printf("\t             echo:\techo yhe strings to standard output\n");	
			printf("\t             record:\tshow last-16 cmds you typed in\n");	
			printf("\t             replay:\tre-execute the cmd showed in record\n");	
			printf("\t             mypid:\tfind and print process-ids\n");	
			printf("\t             exit:\texit shell\n");														
			printf("\t============================================\n");
			printf("\n\n");
			exit(0);

		}	
		else if (strcmp(args[0],"echo") == 0){
			int index = 1;
			int file = 0;
			char concated[100];
			memset(concated, 0, 100);
			while ((args[index]) != NULL){
				if(strcmp(args[1],"-n") == 0 && index == 1){
					index++;
					continue;
				}
				if(strcmp(args[1],"-n") != 0 && index == 1){
					strcat(concated, args[index]);
					index++;
					continue;
				}else if(strcmp(args[1],"-n") == 0 && index == 2){
					strcat(concated, args[index]);
					index++;
					continue;				
				}
				if(strcmp(args[index],">") == 0){
					file = 1;
					break;
				}					
				//printf("///////////////////");
				strcat(concated, " ");
				strcat(concated, args[index]);
				index++;	


			}
			///if(file == 0){
				printf("%s",concated);			
				if(strcmp(args[1],"-n") != 0 )printf("\n");
				memset(concated, 0, 100);

			// }
			// if(file == 1){
			// 	if(strcmp(args[1],"-n") != 0 )strcat(concated, "\n");
			// 	int f;
			// 	f = open(args[index+1], O_CREAT | O_TRUNC | O_WRONLY, 0700); /* 開個名叫hello的檔 */
			// 	write(f,concated, strlen(concated)); /* 用write()把hello world寫進hello這個檔裡 */
			// 	close(f); /* 關檔 */
			// 	memset(concated, 0, 100);

			// }
			exit(0);


		}			
		
		else if (strcmp(args[0],"record") == 0) {
			int file = 0;
			// if(k > 1){
			// 	file = 1;
			// }
			//if(file == 0){
				for(int index = 0;index < 16 && history[index] != NULL ; index++){
					printf("%d: ",index%16+1);
					for(int j = 0; j < sizeof(history[index]); j++){
						if(history[index][j]!=NULL)printf("%c",history[index][j]);
						else continue;
					}
					printf("\n");
					
				}
					
			exit(0);
		}   	
		else if(strcmp(args[0], "mypid")==0)
		{
			if(strcmp(args[1], "-i")==0){
				printf("%d\n",getpid());
				
			}
			else if(strcmp(args[1], "-p")==0){
				char filename[1000];
				sprintf(filename, "/proc/%s/stat", args[2]);	
				if (access(filename,0)){
					printf("process id not exit\n");
					exit(0);
				}						
				FILE *f = fopen(filename, "r");
				int unused;
				char comm[1000];
				char state;
				int ppid;
				fscanf(f, "%d %s %c %d", &unused, comm, &state, &ppid);
				printf("%d\n", ppid);		
				fclose(f);				
			}	
			else if(strcmp(args[1], "-c")==0){
				char filename[1000];
				char buf[1024]={"\0"};
				sprintf(filename, "/proc/%s/task/%s/children", args[2],args[2]);	
				int f;
				f = open(filename, O_RDONLY); /* 開個名叫hello的檔 */														

				read(f,buf,1024);					
	
				printf("%s\n",buf);
				fclose(f);		
				
			}											
			exit(0);
		}				
		else {ret=execvp(args[0], args);}
		if(ret==-1)
		{
			printf("Unable to execute\n");	
			exit(0);	
		}
	}
    else				// Parent Process
    {
		if (background == 0 || pipe_index < pipesCount){
			//wait(NULL);
			waitpid(pid,NULL,0);
			close(fd[1]);
			fd_multiple=fd[0];
		}else{
			// In order to create a background process, the current process
			// should just skip the call to wait. The SIGCHILD handler
			// signalHandler_child will take care of the returning values
			// of the childs.
			if(pipe_index == pipesCount)
				printf("[pid:] %d\n",pid);
			
		}		
     
    }
}
int changeDirectory(char* args[]){
	// If we write no path (only 'cd'), then go to the home directory
	if (args[1] == NULL) {
		chdir(getenv("HOME")); 
		return 1;
	}
	// Else we change the directory to the one specified by the 
	// argument, if possible
	else{ 
		if (chdir(args[1]) == -1) {
			printf(" %s: no such directory\n", args[1]);
            return -1;
		}
	}
	return 0;
}
int main(){
	printf("Welcome to basic shell\n");

	signal(SIGINT, shell);
	shell(0);
	return 0;
}
char *lsh_read_line(void){
	char *line = NULL;
	ssize_t bufsize = 0; // 利用 getline 幫助我們分配緩衝區
	getline(&line, &bufsize, stdin);
	return line;
}


void shell(int signal)
{
		char command[1024];
        char * tokens[256];
		int flag=1;
        int numTokens;
		int background;
		
		//printf("Back to Shell\n");

		do
		{
            memset(command, 0, 1024);

			printf(">>> $ ");
            //command = lsh_read_line();

            
            scanf("%[^\n]%*c",command);
            setbuf(stdin, NULL);
            //if((tokens[0] = strtok(command," \n\t")) == NULL) continue;



			//removelastspace(command);
            if(command[0]==' ' || command[0]=='\t' || command[0]=='\n' || command[0]=='\r')continue;
			//printf("%c",command[0]);
            memset(history[last%16], 0, 100);
			for(int i=0;i<strlen(command);i++){
                history[last%16][i] = command[i];
			}          
            last++;            
			execute(command);	


	}while(flag==1);
}
int file = 0;
void execute(char* command){
			int	background = 0;
			int pipe_index = 0;
            int pipesCount=0;
			file = 0;
			for(int i=0;i<strlen(command);i++){
				if(command[i]=='|'){
					pipesCount++;			
				}
				if(command[i]=='>' || command[i]=='<'){
					file = 1;
				}				
				if(command[i]=='&'){
					background=1;
					command[i]=NULL;
					command[i-1]=NULL;
				}
			}
			//printf("Number of pipes : %d\n", pipesCount);
            //
			char pipeCommands[pipesCount+1][100];
			int j=0,k=0;
			for(int i=0;i<strlen(command);i++){
				if(command[i]=='|'){
					pipeCommands[j][k-1]='\0';
					j++;
					k=0;									
				}
				else if(command[i]==' '){
					pipeCommands[j][k]='$';
					k++;			
				}
				else{
					pipeCommands[j][k]=command[i];
					k++;
	
				}			
			}
			pipeCommands[j][k]='\0';

			for(int i=0; i<pipesCount+1;i++)			// Loop for each pipe sequence
			{
				//printf("Pipe command : %s\n", pipeCommands[i]);
				char *args[10];	
				int k=0;
				if(i==0){								// First command doesn't has starting symbol $
					args[0]=&pipeCommands[i][0];
					k++;
				}

				unsigned int size=strlen(pipeCommands[i]);
				for(int j=0;j<size;j++)
				{
					if(pipeCommands[i][j]=='$'){
							pipeCommands[i][j]='\0';
							args[k]=&pipeCommands[i][j+1];
							k++;			
					}
				}
				args[k]=NULL;
                
				if(strcmp(args[0], "exit")==0)
				{
                    printf("Good Bye!\n");
					exit(0);
				}
                else if ((strcmp(args[0],"help") == 0) && background==0 && pipesCount==0 && file == 0){
                    printf("\n\t============================================\n");
                    printf("\t              Type program names and arguments, and hit enter\n");
                    printf("\t--------------------------------------------\n");
                    printf("\t             The following are built-in function info\n");
                    printf("\t             help:\tshow all build-in function info\n");
                    printf("\t             cd:\tchange directory\n");
                    printf("\t             echo:\techo yhe strings to standard output\n");	
                    printf("\t             record:\tshow last-16 cmds you typed in\n");	
                    printf("\t             replay:\tre-execute the cmd showed in record\n");	
                    printf("\t             mypid:\tfind and print process-ids\n");	
                    printf("\t             exit:\texit shell\n");														
                    printf("\t============================================\n");
                    printf("\n\n");
                    continue;

                }
                if (strcmp(args[0],"cd") == 0) {
                    changeDirectory(args);
                    continue;
                }
                else if ((strcmp(args[0],"echo") == 0) && background==0 && pipesCount==0){
                    int index = 1;
                    int file = 0;
                    char concated[100];
                    memset(concated, 0, 100);
                    while ((args[index]) != NULL){
                        if(strcmp(args[1],"-n") == 0 && index == 1){
                            index++;
                            continue;
                        }
                        if(strcmp(args[1],"-n") != 0 && index == 1){
                            strcat(concated, args[index]);
                            index++;
                            continue;
                        }else if(strcmp(args[1],"-n") == 0 && index == 2){
                            strcat(concated, args[index]);
                            index++;
                            continue;				
                        }
                        if(strcmp(args[index],">") == 0){
                            file = 1;
                            break;
                        }					
                        //printf("///////////////////");
                        strcat(concated, " ");
                        strcat(concated, args[index]);
                        index++;	


                    }
                    if(file == 0){
                        printf("%s",concated);			
                        if(strcmp(args[1],"-n") != 0 )printf("\n");
                        memset(concated, 0, 100);

                    }
                    if(file == 1){
                        if(strcmp(args[1],"-n") != 0 )strcat(concated, "\n");
                        int f;
                        f = open(args[index+1], O_CREAT | O_TRUNC | O_WRONLY, 0700); /* 開個名叫hello的檔 */
                        write(f,concated, strlen(concated)); /* 用write()把hello world寫進hello這個檔裡 */
                        close(f); /* 關檔 */
                        memset(concated, 0, 100);

                    }
                    continue;


                }
                else if ((strcmp(args[0],"record") == 0) && background==0 && pipesCount==0 && file == 0) {
					
					if(k > 1){
						file = 1;
					}
                    if(file == 0){
						for(int index = 0;index < 16 && history[index] != NULL ; index++){
							printf("%d: ",index%16+1);
							for(int j = 0; j < sizeof(history[index]); j++){
								printf("%c",history[index][j]);
							}
							printf("\n");
							
						}

                    }
                    if(file == 1){
                        int f;
                        f = open(args[2], O_CREAT | O_TRUNC | O_WRONLY, 0700); /* 開個名叫hello的檔 */						
						for(int index = 0;index < 16 && history[index] != NULL ; index++){

							
							write(f,history[index], strlen(history[index]));

							write(f,"\n", 1);
							
						}						


					
                         /* 用write()把hello world寫進hello這個檔裡 */
                        close(f); /* 關檔 */

                    }					
                    continue;
                }   
                else if (strcmp(args[0],"replay") == 0) {
					int index = 0;
					char new_command[1024];
					memset(new_command, 0, 1024);

					for(int i=0;i<strlen(history[atoi(args[1])-1]) && history[atoi(args[1])-1][i] != NULL;i++){
						new_command[i] = history[atoi(args[1])-1][i];	
						index++;					
					}  	

					// for(int i = 0;i < strlen(command); i++){
					// 	printf("%d %c\n",i,command[i]);
					// }				
					removelastspace(new_command);
					if(pipesCount > 0 || file == 1 || background == 1){
						new_command[index]=' ';
						index++;						
						int i = strlen(args[0]) + strlen(args[1]) + 2;
						while(command[i]!=NULL){
							new_command[index]=command[i];
							index++;
							i++;
						}

					}	
					
					if(background == 1){
						// new_command[index] = ' ';
						// index++;
						new_command[index] = '&';
						index++;						
					}			
					// for(int i = 0;i < strlen(new_command)&&new_command[i]!=NULL; i++){
					// 	printf("%d %c\n",i,new_command[i]);
					// }	
					memset(history[(last-1)%16], 0, 100);
					for(int i=0;i<strlen(new_command) && new_command[i]!=NULL;i++){
						history[(last-1)%16][i] = new_command[i];
					} 					
					execute(new_command);

                    continue;
                } 

				if((strcmp(args[0], "mypid")==0) && background==0 && pipesCount==0 && file == 0)
				{
					for(int j=0;j<k;j++)
					{
						if(strcmp(args[j],">") == 0)
						{
							args[j]=NULL;
							j=j+1; 		//filename

							close(1);

							char fn[100];
							strncpy(fn,args[j],99);

							int lfd=open(fn, O_RDWR | O_TRUNC);
						
							if(lfd==-1)
							{
								open(fn, O_CREAT | O_RDWR | O_TRUNC);
							}
							args[j]=NULL;
						}	
						else if(strcmp(args[j],"<") == 0)
						{
							args[j]=NULL;
							j=j+1;			//filename
							
							close(0);
							
							char fn[100];
							strncpy(fn,args[j],99);

							int lfd=open(fn, O_RDONLY);
							
							if(lfd==-1)
							{
								open(fn, O_CREAT | O_RDWR | O_TRUNC);
							}
							args[j]=NULL;
						}		
					}					
					if(strcmp(args[1], "-i")==0){
						printf("%d\n",getpid());
						
					}
					else if(strcmp(args[1], "-p")==0){
						char filename[1000];
						sprintf(filename, "/proc/%s/stat", args[2]);	
						if (access(filename,0)){
							printf("process id not exit\n");
							continue;
						}						
						FILE *f = fopen(filename, "r");
						int unused;
						char comm[1000];
						char state;
						int ppid;
						fscanf(f, "%d %s %c %d", &unused, comm, &state, &ppid);
						printf("%d\n", ppid);		
						fclose(f);				
					}	
					else if(strcmp(args[1], "-c")==0){
						char filename[1000];
						char buf[1024]={"\0"};
						sprintf(filename, "/proc/%s/task/%s/children", args[2],args[2]);	
                        int f;
                        f = open(filename, O_RDONLY); /* 開個名叫hello的檔 */														
	
						read(f,buf,1024);					
			
						printf("%s\n",buf);	
						
					}											
					continue;
				}				                                            

				pipeSequence(args, pipesCount,pipe_index,k,background);
				pipe_index++;
				zone++;
			}

			reset();		
}
