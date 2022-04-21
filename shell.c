#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#define DELIMITER " "

char* read_shell();
char** tokenize(char*);
int execute(char**);

extern int errno ;

int main()
{
    char* line;
    char** tokenList;
    int status=1;

    do
    {
       printf("$");
       line=read_shell();//Reading the shell command
       //printf("%s\n",line);
       tokenList=tokenize(line);
       status=execute(tokenList);

       free(line);
       //free(line);

    } while (status);

}

char* read_shell()
{
    int size=256;
    int index=0;
    char* buffer=malloc(sizeof(char)*size);

    if (buffer==NULL)
    {
        //If malloc fails, return with an error
        printf("Value of errno: %d\n", errno);
        printf("Error Message: %s\n", strerror(errno));
        free(buffer);
        exit(EXIT_FAILURE);
    }
    while (1)
    {
        char ch=getchar();
        if (ch==EOF)
        {
            //If ch==EOF, print error message and return
            printf( "Value of errno: %d\n", errno);
            printf("Error Message: %s\n", strerror(errno));
            free(buffer);
            exit(EXIT_FAILURE);
        }
        if (ch =='\n')
            {
            //if enter is pressed, the command ends and therefore buffer is returned
            buffer[index++]='\0';
            return buffer;
            }
        else
            buffer[index++]=ch;

        if (index >= size)
        {
            //Resized size. The realloc() function shall deallocate the old object pointed to
            //by ptr and return a pointer to a new object that has the size
            //specified by size.

            size+=256;
            buffer=realloc(buffer,sizeof(char)*size);
        }
    }
}

char** tokenize(char* line)
{
    int wordSize=256;
    char* token;
    char** tokenList=malloc(wordSize*sizeof(char*));
    int index=0;

    if (tokenList==NULL)
    {
        printf("Value of errno: %d\n", errno);
        printf("Error Message: %s\n", strerror(errno));
        free(tokenList);
        exit(EXIT_FAILURE);
    }

    //The strtok() function breaks a string into a sequence of zero or
    //   more nonempty tokens.Each call to strtok() returns a pointer to a null-terminated
    //   string containing the next token.  This string does not include
    //   the delimiting byte.  If no more tokens are found, strtok()
    //   returns NULL.
    
    token=strtok(line,DELIMITER);

    while(token != NULL)
    {
        tokenList[index++]=token;

        if (index >=wordSize)
        {
            wordSize=wordSize+256;
            tokenList=realloc(tokenList,sizeof(char*)*wordSize);

            if (tokenList==NULL)
            {
                printf("Value of errno: %d\n", errno);
                printf("Error Message: %s\n", strerror(errno));
                free(token);
                free(tokenList);
                exit(EXIT_FAILURE);
            }

        }

        token=strtok(NULL,DELIMITER);
    }
    tokenList[index]=NULL;

    free(token);
    free(tokenList);

    return tokenList;
}

int execute(char** tokenList)
{
int pid;
int status;

if (strcmp(tokenList[0],"exit")==0)
{   //If the exit token is entered, exit  the program
    return 0;
}
pid=fork();//fork the parent process
if (pid<0)
{   //fork returns a pid<0 in cases of error
    printf("%s\n","Error forking!");
    exit(EXIT_FAILURE);
}
else if (pid==0)
{
    int execStatus=execvp(tokenList[0],tokenList);//load the token command and execute
    if ( execStatus==-1 )
    {
    printf("%s\n","Operation not supported!");
    exit(EXIT_FAILURE);
    }
}
else
    waitpid(pid, &status, WUNTRACED);
return 1;
}


