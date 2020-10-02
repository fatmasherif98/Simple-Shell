#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>

#define MAX_CHARS 100  //maximum size of argument string
#define MAX_ARGS 10 //maximum size of arguments

FILE *f;
void appendToLogFile(int pid){ //func appends to log file
f = fopen("traceFile.log", "a+");
if( f == NULL)
{
    printf("error creating log file\n");
}
else{
    fprintf(f, "%d\n", pid); //prints the terminated child's id
    fclose(f);
    }
}

void childTerminated(int signal_no){
pid_t pid = wait(NULL); //signal handler
appendToLogFile(pid);

}
int main()
{

f = fopen("traceFile.log", "w"); //clear the log file
if( f  == NULL)
    printf("error opening log file");
else
    fclose(f);
pid_t pid;
int waitFlag; //flag for waiting for child
printf("\t\t\t<<SHELL LAUNCHED>>\n");
    char *arguments[MAX_ARGS]; //array of args
    char command[MAX_CHARS]; //command input as a String
    char *temp;
    char directory_string[MAX_CHARS]; //holds the current directory

    while( 1)
    {
    printf("\n");
    waitFlag = 1; //default value is 1 for flag, for "&" set it to 0
    memset(command, 0 , MAX_CHARS); //clear command buffer
    memset(directory_string, 0 , MAX_CHARS);//clear directory buffer
    printf("%s>", getcwd( directory_string, MAX_CHARS)); //print our current directory
    fgets(command, MAX_CHARS, stdin); //get input from user
    strcpy(command,strtok(command, "\n") ); //remove newline character
    temp = strtok(command, " ");

    if( strcasecmp(temp, "exit") == 0)
    return 0;//for exit terminate parent process

    int i=0;
    while ( temp != NULL)
    {
        if( strcasecmp(temp, "&") == 0)
            { //check for "&"
            waitFlag = 0;
            temp = strtok(NULL, " ");
            continue;
            }

        arguments[i++] = temp;
        temp = strtok(NULL, " ");
    }
    while( i < MAX_ARGS)
        arguments[i++] = NULL; //set the rest of the array to NULL




    if( strcasecmp( arguments[0], "cd") == 0)
    { //for cd, change directory and print current directory
        if( chdir(arguments[1]) < 0 )
        {
            printf("No such file or directory");
        } else
        {
            memset(directory_string, 0, MAX_CHARS);
            printf("%s\n", getcwd( directory_string, MAX_CHARS));
        }
    } else


    {  //if command is not cd, Fork and call execvp
        pid = fork();
        if( pid == 0) //childProcess
        {


        if( execvp(arguments[0], arguments ) < 0)
            printf("FAILED"); //if execvp fails
        exit(0); //exiting from child
        } else if( pid < 0)
        {
            printf("Error creating child process\n");
        }
    //parent code
        if( waitFlag)
        {//to prevent calling wait twice
        //when child is terminated, return value
        //from wait is printed to log file
            pid_t childID = waitpid(pid);
            appendToLogFile(childID);
        }
        else //call signal handler to print child ID
            signal(SIGCHLD, childTerminated );

        }

}
    return 0;
}
