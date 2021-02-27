/**************************************************
2048 Game
**************************************************/
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define WRT_ERROR "Error at writing\n"
#define SIG_ERROR "sigaction configuration failed\n"
#define KILL_ERROR "Failed trigger the process\n"

#define MATRIX_SIZE 4
#define LIMIT 100
int matrix [MATRIX_SIZE][MATRIX_SIZE];
int terminateFlag = 0;
int ex2_inpPid;

void InitializeMatrix();
void RandomTwos();
void UpdateToRight();
void MoveRight();
void MergeRight();
void UpdateToLeft ();
void MoveLeft();
void UpdateToUp ();
void MoveUp();
void MergeUp ();
void UpdateToDown ();
void MoveDown ();
void MergeDown ();
void RandomTwos ();
void MatrixToString ();
void AlarmSignal(int sig);
void TerminateSignal(int sig);

void PrintMatrix();

/*****************************************************
 * Input: None.                                      *
 * Output: Void function.                            *
 * Description: Converting a matrix to a string      *
 * representation and than writing it to the buffer. * 
 *****************************************************/
void MatrixToString () {

	char* stringRep = (char*)malloc(1);
	stringRep[0] = '\0';
	char temp[5] = {0};
	int length;
	int i,j;

	for (i = 0; i < MATRIX_SIZE; i++) {
		for (j = 0; j < MATRIX_SIZE; j++) {
			sprintf(temp,"%d",matrix[i][j]);
			stringRep = (char*)realloc(stringRep, strlen(stringRep) + strlen(temp) + 2);
			strcat(stringRep, temp);
			strcat(stringRep, ",");
		}
	}
	length = strlen(stringRep);
	stringRep[length-1] = '\0';

	if((write(1,stringRep,strlen(stringRep)+1)==-1))
	{
		write(2,WRT_ERROR,strlen(WRT_ERROR));
		exit(1);
	}
}

/***************************************************
 * Input: Signal descriptor identifier.            *
 * Output: Void function.                          *
 * Description: Handler function for the SIGALARM  *
 * signal trying to generate an empty spot in the  *
 * game and update it with a number 2, than call   *
 * to the first process to print it.               * 
 ***************************************************/
void AlarmSignal(int sig)
{
	srand(time(NULL));
	int i,j;
	int iteration = 0;
	do {
		if (iteration >= LIMIT) {
			break;
		}
		i=rand()% 4;
		j=rand()% 4;
		iteration++;
	} while(matrix[i][j]!=0);
	matrix[i][j]=2;
}

/***************************************************
 * Input: Signal descriptor identifier             *
 * Output: Void function.                          *
 * Description: Handler function for the SIGINT    *
 * signal, turns on the terminating flag.          * 
 ***************************************************/
void TerminateSignal(int sig) {
	terminateFlag = 1;
}

/***************************************************
 * Input: None.                                    *
 * Output: Void function.                          *
 * Description: Placing random two numbers of 2    *
 * on the matrix right after we build a matrix     * 
 ***************************************************/
void RandomTwos() {
	//Improving the randomness.
	srand (time(NULL));
	int i = rand() % 4;
	int j = rand() % 4;
	int k = rand() % 4;
	int l = rand() % 4;

	//Checking that the 2 locations on the matrix are different.
	while (i==k && j==l) {
		k = rand() % 4;
		l = rand() % 4;
	}
	//Assigning the numbers on the matrix.
	matrix[i][j] = 2;
	matrix[k][l] = 2;
}

/*******************************************************
 * Input: None.                                        *
 * Output: Void function.                              *
 * Description: Initializing a two dimantional array   * 
 * of 4x4 size with zeros                              * 
 *******************************************************/
void InitializeMatrix() {
	int i,j;
	for (i = 0; i < MATRIX_SIZE; i++) {
		for (j = 0; j < MATRIX_SIZE; j++) {
			matrix[i][j] = 0;
		}
	}
}

/*****************************************************
 * Input: None.                                      *
 * Output: Void function.                            *
 * Description: This function will move all the game *
 * matrix values to the right side this algorithm    *
 * is very similar to the bubble sort.               * 
 *****************************************************/
void MoveRight() {
	int counter;
	int i,j;
	//Shift all the matrix right
	for (i = 0; i < MATRIX_SIZE; i++) {
		counter = 0;
		while (counter < MATRIX_SIZE) {
			for (j = 1; j < MATRIX_SIZE; j++) {
				if (matrix[i][j]==0) {
					matrix[i][j] = matrix[i][j-1];
					matrix[i][j-1] = 0;
				}
			}
			counter++;
		}
	}
}

/*********************************************************
 * Input: None.                                          *
 * Output: Void function.                                *
 * Description: This function will get two equal values  *
 * that close to each other and add them to a new one.   * 
 *********************************************************/
void MergeRight() {
	int i,j;
	for (i = 0; i < MATRIX_SIZE; i++) {
		for (j = 3; j > 0; j--) {
			if (matrix[i][j] == matrix[i][j-1]) {
				matrix[i][j] *= 2;
				matrix[i][j-1] = 0;
			}	
		}
	}	
}
//See above.
void MoveLeft (){
	int counter;
	int i,j;
	//Shift all the matrix left
	for (i = 0; i < MATRIX_SIZE; i++) {
		counter = 0;
		while (counter < MATRIX_SIZE) {
			for (j = 2; j >= 0; j--) {
				if (matrix[i][j]==0) {
					matrix[i][j] = matrix[i][j+1];
					matrix[i][j+1] = 0;
				}
			}
			counter++;
		}
	}
}
//See above.
void MergeLeft() {
	int i,j;
	for (i = 0; i < MATRIX_SIZE; i++) {
		for (j = 0; j < MATRIX_SIZE - 1; j++) {
			if (matrix[i][j] == matrix[i][j+1]) {
				matrix[i][j] *= 2;
				matrix[i][j+1] = 0;
			}	
		}
	}	
}
//See above.
void MoveUp() {
	int counter;
	int i,j;
	//Shift all the matrix up
	for (i = 0; i < MATRIX_SIZE; i++) {
		counter = 0;
		while (counter < MATRIX_SIZE) {
			for (j = 2; j >= 0; j--) {
				if (matrix[j][i]==0) {
					matrix[j][i] = matrix[j+1][i];
					matrix[j+1][i] = 0;
				}
			}
			counter++;
		}
	}
}
//See above.
void MergeUp (){
	int i,j;
	for (i = 0; i < MATRIX_SIZE; i++) {
		for (j = 0; j < MATRIX_SIZE - 1; j++) {
			if (matrix[j][i] == matrix[j+1][i]) {
				matrix[j][i] *= 2;
				matrix[j+1][i] = 0;
			}	
		}
	}	
}
//See above.
void MoveDown (){
	int counter;
	int i,j;
	//Shift all the matrix down
	for (i = 0; i < MATRIX_SIZE; i++) {
		counter = 0;
		while (counter < MATRIX_SIZE) {
			for (j = 1; j < MATRIX_SIZE; j++) {
				if (matrix[j][i]==0) {
					matrix[j][i] = matrix[j-1][i];
					matrix[j-1][i] = 0;
				}
			}
			counter++;
		}
	}
}
//See above.
void MergeDown (){
	int i,j;
	for (i = 0; i < MATRIX_SIZE; i++) {
		for (j = 3; j > 0; j--) {
			if (matrix[j][i] == matrix[j-1][i]) {
				matrix[j][i] *= 2;
				matrix[j-1][i] = 0;
			}	
		}
	}	
}

/********************************************************
 * Input: No input.                                     *
 * Output: Void function.                               *
 * Description: This function will start the alghoritm  *
 * that evantually will move and merge all the values   *
 * to the right.                                        * 
 ********************************************************/
void UpdateToRight () {
	MoveRight();
	MergeRight();
	MoveRight();
}

//See above.
void UpdateToLeft () {
	MoveLeft();
	MergeLeft();
	MoveLeft();
}

//See above.
void UpdateToUp () {
	MoveUp();
	MergeUp();
	MoveUp();
}

//See above.
void UpdateToDown () {
	MoveDown();
	MergeDown();
	MoveDown();
}

void PrintMatrix() {
	int i, j;
	for (i = 0; i < MATRIX_SIZE; i++) {
		for (j = 0; j < MATRIX_SIZE; j++) {
			printf("%d", matrix[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}

/**************************************************************
 * Input: None.                                               *
 * Output: return 0 if normaly exited.                        *
 * Description: Configuring the sigaction struct              *
 * for terminate and alarm(SIGINT/SIGALRM) signals that will  *
 * excute the specified handler function for each signal.     * 
 **************************************************************/
int main (int argc, char** argv) {
	/*
	 * This process will hold the pid of the first one.
	 * Converting the process info from string to int.
	 */
	sscanf(argv[1], "%d", &ex2_inpPid);
	//Adding to the block struct all the possible signals.
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

	//Configuring the SIGALRM (alarm signal) sigaction struct.
	struct sigaction alarmInfo;
	alarmInfo.sa_handler = AlarmSignal;
	alarmInfo.sa_mask = blockedSignals;
	alarmInfo.sa_flags = 0;
	if((sigaction(SIGALRM,&alarmInfo,NULL))==-1)
	{
		write(2,SIG_ERROR,strlen(SIG_ERROR));
	}

	//Initializing the matrix global two dimentional array with zeros.
	InitializeMatrix();
	//Setting two numbers of 2 to that matrix.
	RandomTwos();
	//setting the alarm to be a random number between 1 to 5.
	alarm((rand()%5)+1);
	//Converting to a known pattern for the first process, and updating the buffer.
	MatrixToString ();
	//Triggering the first process to read the buffer and print the result.
	if((kill(ex2_inpPid,SIGUSR1))==-1)
	{
		write(2,KILL_ERROR,strlen(KILL_ERROR));
		exit(1);
	}
	//This command will be closed at the end of the ex2.c file, when done.
	system("stty cbreak -echo");

	//Terminating handled function can turn on the flag and stop the loop.
	while (!terminateFlag) {

		char input;
		
		input = getchar();

		switch (input) {
			case 'D':
				UpdateToRight();
				break;
			case 'A':
				UpdateToLeft();
				break;
			case 'W':
				UpdateToUp();
				break;
			case 'X':
				UpdateToDown();
				break;
			case 'S':
				InitializeMatrix();
				RandomTwos();
				break;		
		}	
		//The alarm will be setted again here.
		alarm((rand()%5)+1);
		//The matrix updated, we need to convert it to string and save it to buffer.
		MatrixToString ();
		//Call the first process to print it.
		if((kill(ex2_inpPid,SIGUSR1))==-1)
		{
			write(2,KILL_ERROR,strlen(KILL_ERROR));
			exit(1);
		}
	}
}