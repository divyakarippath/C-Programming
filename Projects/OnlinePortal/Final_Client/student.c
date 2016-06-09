
/***************************************************************************************************/
/*                                                                                                 */
/*                                                                                                 */
/*                                                                                                 */
/*                                   Online Student Portal                                         */
/*                                   =====================                                         */
/*                                                                                                 */
/*                                                                                                 */
/*                                                                                                 */
/***************************************************************************************************/



#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <errno.h>

#include "errexit.c"

#define BLEN 1024

struct studentCredential{
    int sId;
    char pwd[1024];
}sCredential;

int  connectTCP(const char *host,const char *service);
int  errexit(const char *format, ...);
void downloadFile(int csd);
void loginToPortal(int csd,char resBuffer[]);
void onlinetest(int csd);
void appointment(int csd);

int main(int argc, char * argv[])
{
    int csd;
    char resBuffer[BLEN],reqBuffer[BLEN],sbuffer[BLEN];
    char uInput;
    char *host,*service,*chatName;
    int pId;
    
    //Storing server details the command line arguments
    switch (argc)
     {
     case	3:
        host = argv[1];              /* server IP address (dotted quad) */
        service = argv[2];
     break;
     default:
     errexit("Usage: %s <Server IP> <Server Port>\n", argv[0]);
     }
    
    csd = connectTCP(host,service);
    
    memset(&reqBuffer[0],0,sizeof(reqBuffer));
    strcpy(reqBuffer,"Student");
    send(csd,reqBuffer,sizeof(reqBuffer),0);
    memset(&reqBuffer[0],0,sizeof(reqBuffer));
    
    memset(&resBuffer[0],0,sizeof(resBuffer));
    recv(csd,resBuffer,sizeof(resBuffer),0);
    loginToPortal(csd,resBuffer);
    
    if(strcmp(resBuffer, "1")==0){
        printf("\n\nOptions available:\n\na. Appointment with the professor\nb. Take online exam\nc. Download the reference materials\nd. Connect with your fellow classmates\n\n\n");
        printf("Please select your option : ");
        scanf(" %c%*c",&uInput);
        
        switch(uInput){
            case 'a':
                appointment(csd);
                break;
            case 'b':
                onlinetest(csd);
                break;
            case 'c':
                downloadFile(csd);
                break;
            case 'd':
                chatName = malloc(100);
                printf("Please enter your name \n");
                
                scanf("%s",chatName);
                sleep(10);
                bzero(sbuffer,1024);
                strcpy(sbuffer,"d");
                send(csd,sbuffer,strlen(sbuffer),0);
                bzero(sbuffer,1024);
                
                pId = fork();
                if(pId==0)
                    execv("./chatclient",(char *[]){"./chatclient",chatName,"9999"});
                break;

            default:
                printf("Invalid user Input\n");
                break;
        }
    }
    else if(strcmp(resBuffer, "2")==0){
        
        errexit("Your password is invalid: %s\n",strerror(errno));
        
    }
    else if(strcmp(resBuffer, "3")==0){
        
        errexit("Your student id is invalid: %s\n",strerror(errno));
    }
    
    close(csd);
    
    return 0;
}

/***************************************************************************************************/
/*          Function for student login  option                                                     */
/***************************************************************************************************/

void loginToPortal(int csd,char resBuffer[])
{
    printf("%s",resBuffer);
    scanf("%d",&sCredential.sId);
    
    strncpy(sCredential.pwd, getpass("Password: "), 1024);
    //scanf("%s",sCredential.pwd);
    send(csd,(void *)&sCredential,sizeof(sCredential),0);
    
    memset(&resBuffer[0],0,BLEN);
    recv(csd,resBuffer,BLEN,0);
}

/***************************************************************************************************/
/*          Function for student to select appointment option                                      */
/***************************************************************************************************/

void appointment(int csd)
{
    char sbuffer[BLEN];
    char rbuffer[BLEN];
    char message[5000];
    char input;
    
    bzero(sbuffer,1024);
    strcpy(sbuffer,"a");
    send(csd,sbuffer,strlen(sbuffer),0);
    bzero(sbuffer,1024);
    
    bzero(rbuffer,1024);
    recv(csd,rbuffer,1024,0);
    //rbuffer[sizeof(rbuffer) - 1] = '\0';
    printf("%s",rbuffer);
    bzero(rbuffer,1024);
    
    scanf(" %c%*c",&input);
    if(input=='1')
    {
        bzero(sbuffer,1024);
        strcpy(sbuffer,"1");
        send(csd,sbuffer,strlen(sbuffer),0);
        bzero(sbuffer,1024);
        
        recv(csd,rbuffer,1024,0);
        printf("%s",rbuffer);
        scanf("%s",message);
        //printf("%s\n",message);
        if(write(csd,message,1024) < 0)
            errexit("write_failed: %s\n", strerror(errno));
        bzero(rbuffer,1024);
        recv(csd,rbuffer,1024,0);
        rbuffer[sizeof(rbuffer) - 1] = '\0';
        printf("%s\n",rbuffer);
        bzero(rbuffer,1024);
    }
    else if(input=='2')
    {
        bzero(sbuffer,1024);
        strcpy(sbuffer,"2");
        send(csd,sbuffer,strlen(sbuffer),0);
        bzero(sbuffer,1024);
        
        recv(csd,rbuffer,1024,0);
        
        if(strcmp(rbuffer,"0")==0)
        {
            printf("There are no slots booked by you\n");
        }
        else
        {
            printf("%s",rbuffer);
            scanf("%s",message);
            //printf("%s\n",message);
            if(write(csd,message,1024) < 0)
                errexit("write_failed: %s\n", strerror(errno));
            bzero(rbuffer,1024);
            recv(csd,rbuffer,1024,0);
            rbuffer[sizeof(rbuffer) - 1] = '\0';
            printf("%s\n",rbuffer);
            bzero(rbuffer,1024);
        }
    }
}

/***************************************************************************************************/
/*          Function for download the reference  material from the portal                          */
/***************************************************************************************************/

void downloadFile(int csd)
{
    char reqBuffer[BLEN],resBuffer[BLEN],path[100];
    long fr_block_sz;
    int j,i,flag=0;
    char **fNames=NULL;
    memset(&reqBuffer[0], 0, BLEN);
    strcpy(reqBuffer,"c");
    send(csd,reqBuffer,strlen(reqBuffer),0);
    memset(&reqBuffer[0], 0, BLEN);
    
    memset(&resBuffer[0], 0, BLEN);
    recv(csd,resBuffer,sizeof(resBuffer),0);
    int count = atoi(resBuffer)-1;
    memset(&resBuffer[0], 0, BLEN);
    
    printf("\n\nPlease select a file name :\n\n");
    for(j=0;j<count;j++)
    {
        recv(csd, resBuffer, sizeof(resBuffer), 0);
        printf("%s\n",resBuffer);
        fNames = (char **)realloc(fNames, (j + 1) * sizeof(char *));
        fNames[j] = strdup(resBuffer);
        memset(&resBuffer[0],0,sizeof(resBuffer));
    }
    printf("\nEnter the filename : ");
    scanf("%s",reqBuffer);
    
    for(i = 0; i < j; i++){
        
        if(strcmp(reqBuffer,fNames[i])==0){
            flag=1;
            break;
        }
    }
    
    for(i = 1; i < j; i++)
    {
        free(fNames[i]);
    }
    free(fNames);
    if(flag==1){
        //Send the filename to the server
        send(csd,reqBuffer,strlen(reqBuffer),0);
        
        memset(&path[0],0,sizeof(path));
        strcpy(path,".");
        strcat(path,"/Files/Student/Downloads/");
        strcat(path,reqBuffer);
        
        
        FILE *fr = fopen(path, "a");
        
        if(fr==NULL)
        {
            errexit("The file name entered '%s' is incorrect: %s\n",strerror(errno));
        }
        else
        {
            memset(&resBuffer[0], 0, sizeof(resBuffer));
            while((fr_block_sz = recv(csd, resBuffer, BLEN, 0))>0)
            {
                size_t write_sz = fwrite(resBuffer, sizeof(char), fr_block_sz, fr);
                if(write_sz < fr_block_sz)
                {
                    errexit("File write failed: %s\n",strerror(errno));
                }
                memset(&resBuffer[0], 0, sizeof(resBuffer));
                if (fr_block_sz == 0 || fr_block_sz != BLEN)
                {
                    break;
                }
            }
            
            if(fr_block_sz<0){
                errexit("Error receiving file from server: %s\n",strerror(errno));
            }
            
            fclose(fr);
        }
        memset(&reqBuffer[0], 0, BLEN);
        memset(&path[0],0,sizeof(path));
        printf("File downloaded successfully\n");
    }
    else{
        errexit("Invalid file name: %s\n",strerror(errno));
    }
}

/***************************************************************************************************/
/*          Function for to take online exam by the  Student                                       */
/***************************************************************************************************/

void onlinetest(int csd)
{
    int i_dir=0,j_dir=0,marks;
    char buffer1[2048],buffer2[2048],buffer3[2048],reqBuffer[BLEN];
    
    memset(&reqBuffer[0], 0, BLEN);
    strcpy(reqBuffer,"b");
    send(csd,reqBuffer,strlen(reqBuffer),0);
    memset(&reqBuffer[0], 0, BLEN);
    
    memset(&buffer2[0],0,sizeof(buffer2));
    recv(csd,buffer2,sizeof(buffer2),0);
    printf("%s\n",buffer2);
    
    memset(&buffer2[0],0,sizeof(buffer2));
    recv(csd,buffer2,sizeof(buffer2),0);
    j_dir=atoi(buffer2)-1;
    
    for(i_dir=0;i_dir<j_dir;i_dir++)
    {
        recv(csd,buffer3,sizeof(buffer3),0);
        printf("\n%s",buffer3);
        memset(&buffer3[0],0,sizeof(buffer3));
    }
    
    printf("\n\nEnter the test complete test name you desire to take (Example: test_1.txt) : ");
    scanf("%s",buffer1);
    send(csd,buffer1,sizeof(buffer1),0);
    /**Receiving the file contents**/
    memset(&buffer2[0],0,sizeof(buffer2));
    recv(csd,buffer2,sizeof(buffer2),0);
    printf("\n%s",buffer2);
    
    /**Entering the corresponding answers**/
    memset(&buffer1[0],0,sizeof(buffer1));
    printf("\n\nEnter your answers in a continuous form :");
    scanf("%s",buffer1);
    send(csd,buffer1,sizeof(buffer1),0);
    
    /**Giving score**/
    memset(&buffer2[0],0,sizeof(buffer2));
    recv(csd,buffer2,sizeof(buffer2),0);
    marks=atoi(buffer2);
    printf("\nYou scored: %d\n",marks);
}

