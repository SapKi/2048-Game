/**************************************************
2048 Game Main
**************************************************/
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define PIPE_ERROR "Pipe creation failed\n"
#define FORK_ERROR "Fork creation failed\n"
#define CLOSE_ERROR "Error while closing\n"
#define DUP_ERROR "dup2 function error\n"
#define EXEC_ERROR "Execution error\n"
#define KILL_ERROR "Failed to trigger the process\n"

/**********************************************************
 * Input: To the arguments we pass the time               *
 * that the game will run.                                *
 * Output: return 0 if normaly exited.                    *
 * Description: Making a pipe to communicate              *
 * between processes, configuring both of the processes   *
 * which side of the pipe they will use, passing to the   *
 * arguments of the second process the pid of the first   *
 * the father process will wait till the game ends        *
 * and trigger the SIGINT signal in both of the processes *
 * and eventually end the game.                           *
 **********************************************************/
int main (int argc, char** argv) {

	//pipe read/write descriptors.
	int fd[2];
	//First and second processes.
	pid_t ex2_inpPid, ex2_updPid; 
	//Time left till the father process will terminate the game.
	int timeLeft;
	int status=0;

	sscanf(argv[1], "%d", &timeLeft);

	//Pipe creation.
	if((pipe(fd)) == -1)
	{
		write(2,PIPE_ERROR,strlen(PIPE_ERROR));
		exit(1);
	}

	//Run the first process (we will need its pid for the second process later).
	if((ex2_inpPid=fork()) == -1)
	{
		write(2,FORK_ERROR,strlen(FORK_ERROR));
		exit(1);
	}
	//The son - the ex2_inp process.
	else if(ex2_inpPid==0)
	{
		//Closing wrting side too when done.
		if((close(fd[1])) == -1)
		{
			write(2,CLOSE_ERROR,strlen(CLOSE_ERROR));
		}
		//Using input redirection to the reading side of pipe.
		if((dup2(fd[0],0)) == -1)
		{
			write(2,DUP_ERROR,strlen(DUP_ERROR));
		}
		char *args[]={"./ex2_inp.out",NULL};
		//Executing the first program.
		if(execvp(args[0],args)==-1)
		{
			write(2,EXEC_ERROR,strlen(EXEC_ERROR));
		}
		//Closing the reading side of pipe.
		if((close(fd[0])) == -1)
		{
			write(2,CLOSE_ERROR,strlen(CLOSE_ERROR));
		}
		
	}
	//Father process - through this process we will open the second process.
	else if(ex2_inpPid>0)
	{
		char stringInpPid[64];
		sprintf(stringInpPid,"%d",ex2_inpPid);

		if((ex2_updPid=fork()) == -1)
		{	
			write(2,FORK_ERROR,strlen(FORK_ERROR));
		    exit(1);
		}
		//Second son process.
		else if(ex2_updPid==0)
		{
			//Closing the reading side of pipe.
			if((close(fd[0]))==-1)
			{		
				write(2,CLOSE_ERROR,strlen(CLOSE_ERROR));
		    }
		    //Using output redirection to the writing side of pipe.
			if((dup2(fd[1],1))==-1)
			{
				write(2,DUP_ERROR,strlen(DUP_ERROR));
		    }
		    char *args[]={"./ex2_upd.out",stringInpPid,NULL};
			//Executing the second program.
			if(execvp(args[0],args)==-1)
			{
				write(2,EXEC_ERROR,strlen(EXEC_ERROR));
				exit(1);
			}
		    //Closing wrting side too when done.
			if((close(fd[1]))==-1)
			{
				write(2,CLOSE_ERROR,strlen(CLOSE_ERROR));
		    }
			
		}

	    /*
		 *Back to the father process again that will stop the game 
	     *after several amount of time.
		 */
		if((ex2_updPid=fork())>0)
		{
			//Waiting till the time given is passed.
		    sleep(timeLeft);
		    //Closing the reading side of the pipe.
			if((close(fd[0]))==-1)
			{
				write(2,CLOSE_ERROR,strlen(CLOSE_ERROR));
		    }
		    //Closing the writing side of the pipe.
			if((close(fd[1]))==-1) 
			{
				write(2,CLOSE_ERROR,strlen(CLOSE_ERROR));
		    }
		    //Sending terminating signal to the first process.
			if((kill(ex2_inpPid,SIGINT))==-1)
			{
				write(2,KILL_ERROR,strlen(KILL_ERROR));
				exit(1);
	    	}
	    	//Make a small stop for the consistent kill function invokation.
			sleep(1);
			//Sending terminating signal to the second process.
			if((kill(ex2_updPid,SIGINT))==-1)
			{
				write(2,KILL_ERROR,strlen(KILL_ERROR));
				exit(1);
	      	}
		
			//Waiting for both child processes to exit if they not than will also return.
			waitpid(ex2_updPid, &status, WNOHANG);
	        waitpid(ex2_inpPid, &status, WNOHANG);
	        system("stty cooked echo");
		}
	}
	return 0;
}