/**************************************************
2048 Game
**************************************************/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

#define WRT_ERROR "Error at writing\n"
#define READ_ERROR "Error at reading\n"
#define SIG_ERROR "sigaction configuration failed\n"

int terminateFlag = 0;

void SigusrHandler ();
void TerminateSignal(int sig);
void PrintFormat(char* currentState);
void BuildString(char** temp, char** buildString);

/***************************************************
 * Input: Signal descriptor identifier.                       *
 * Output: Void function.                          *
 * Description: Printing "BYE BYE" and exiting the *
 * program with 0 value.                           * 
 ***************************************************/
void TerminateSignal(int sig)
{
	//This will stop the loop.
	terminateFlag = 1;
    if((write(1,"BYE BYE\n",strlen("BYE BYE\n"))==-1))
    {
    	write(2,WRT_ERROR,strlen(WRT_ERROR));
    	exit(1);
    }
    exit(0);
}

/*************************************************************
 * Input: None                               *
 * Output: Void function.                                    *
 * Description: Reading from the buffer that the pipe holds, *
 * and printing the game by the specified format.            * 
 *************************************************************/
void SigusrHandler () {
	char currentState [80] = {0};
	//Reading from the pipe buffer to the array above.
	if((read(0,currentState,80))==-1)
	{
		write(2,READ_ERROR,strlen(READ_ERROR));
		exit(1);
	}
	//Printing by the specified format.
	PrintFormat(currentState);
}

/*************************************************************
 * Input: Two strings: temp holds data to add,                *
 * buildString is the destination string.                    *
 * Output: Void function.                                    *
 * Description: A function that will append to buildString   *
 * string the newly read data that is stored in temp string  * 
 *************************************************************/
void BuildString(char** temp, char** buildString) {
	int tempSize = strlen(*temp);
	*buildString = (char*)realloc(*buildString, strlen(*buildString)+ 8);
	//Checking if 0 is the case.
	if (*temp[0] == '0')
		tempSize = 0;
	//Choosing the right amount of zero padding if needed.
	switch(tempSize) {
		//A special case where temp holding 0.
		case 0:
			strcat(*buildString, "|      ");
			break;
		case 1:
			strcat(*buildString, "| 000");
			strcat(*buildString, *temp);
			strcat(*buildString, " ");
			break;
		case 2:
			strcat(*buildString, "| 00");
			strcat(*buildString, *temp);
			strcat(*buildString, " ");
			break;
		case 3:
			strcat(*buildString, "| 0");
			strcat(*buildString, *temp);
			strcat(*buildString, " ");
			break;
		case 4:
			strcat(*buildString, "| ");
			strcat(*buildString, *temp);
			strcat(*buildString, " ");
			break;
	}
}

/*************************************************************
 * Input: Two strings: temp holds data to add,                *
 * buildString is the destination string.                    *
 * Output: Void function.                                    *
 * Description: A function that will append to buildString   *
 * string the newly read data that is stored in temp string. * 
 *************************************************************/
void PrintFormat(char* currentState) {
	char* buildString = (char*)malloc(1);
	buildString[0] = '\0';
	char* temp = (char*)malloc(5);
	//char* temp = NULL;
	int i = 0;
	int commaCounter = 0;

	//The currect printing output building process.
	while (*currentState != '\0') {
		//temp = (char*)realloc(temp, i+1);

		//Adding the gathert digits in temp to buildString when comma found.
		if (*currentState == ',') {
			commaCounter++;

			temp[i] = '\0';
			//Adding process.
			BuildString(&temp, &buildString);

			//Adding '|' charecter at the end of each row.
			if (commaCounter % 4 == 0){
				buildString = (char*)realloc(buildString, strlen(buildString)+ 3);
				strcat(buildString, "|\n");
			}

			
			i = 0;
			currentState++;
			continue;
		}
		
		temp[i] = *currentState;
		i++;
		
		currentState++;
	}

	//Adding the last number to the buildString array.
	temp[i] = '\0';
	BuildString(&temp, &buildString);
	free(temp);
	temp = NULL;
	buildString = (char*)realloc(buildString, strlen(buildString)+ 4);
	strcat(buildString, "|\n\n");
	//Writing this to the pipe, that configured previously, buffer.
	if((write(1,buildString,strlen(buildString))==-1))
	{
		write(2,WRT_ERROR,strlen(WRT_ERROR));
		exit(1);
	}
	free(buildString);
	buildString = NULL;
}

/**********************************************************
 * Input: None.                                       *
 * Output: return 0 if normaly exited.                    *
 * Description: Configuring the sigaction struct          *
 * for terminate signal and for SIGUSR1 signal that will  *
 * excute the specified handler function for each signal. * 
 **********************************************************/
int main () {
	//Setting the signals that would be blocked for both sigaction structs.
	sigset_t blockedSignals;
	sigfillset(&blockedSignals);

	//Configuring the SIGINT (terminate signal) sigaction struct.
	struct sigaction terminateInfo;
    terminateInfo.sa_handler = TerminateSignal;
	terminateInfo.sa_mask = blockedSignals;
	terminateInfo.sa_flags = 0; 
	if((sigaction(SIGINT,&terminateInfo,NULL))==-1)
	{
		write(2,SIG_ERROR,strlen(SIG_ERROR));
	}

	//Configuring the SIGUSR1 (user specified) signal sigaction struct.
	struct sigaction sigusrInfo;
	sigusrInfo.sa_handler = SigusrHandler;
	sigusrInfo.sa_mask = blockedSignals;
	sigusrInfo.sa_flags = 0;
	if((sigaction(SIGUSR1,&sigusrInfo,NULL))==-1)
	{
		write(2,SIG_ERROR,strlen(SIG_ERROR));
	}
	//Making a loop that ends when the SIGINT signal is triggered.
	while(!terminateFlag)
	{
		pause();
	}
}