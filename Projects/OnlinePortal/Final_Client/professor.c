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
#include <errno.h>

#define BLEN 1024

struct professorCredential{
    char uName[1024];
    char pwd[1024];
}pCredential;

int  connectTCP(const char *host,const char *service);
int  errexit(const char *format, ...);
void uploadFile(int sock);
void loginToPortal(int sock,char resBuffer[]);
void uploadTest(int sock);
void uploadQuestions(int sock);
void uploadAnswers(int sock);
void insertStudentData(int sock);
void updateOfficeHours(int sock);


int main(int argc, char * argv[])
{
    char resBuffer[BLEN],reqBuffer[BLEN];
    char uInput;
    char *host;
    struct sockaddr_in ser_IP, ser_IP2;
    int sock  ,y;
    unsigned short service;
    
    
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == -1)
    {
        printf("socket creation failed\n");
        exit(1);
    }
    
    //Storing server details the command line arguments
    switch (argc)
    {
        case	3:
            host = argv[1];              /* server IP address (dotted quad) */
            service =atoi(argv[2]);
            break;
        default:
            errexit("Usage: %s <Server IP> <Server Port>\n", argv[0]);
    }
    
    ser_IP.sin_addr.s_addr = inet_addr("127.0.0.1");
    ser_IP.sin_family = AF_INET;
    ser_IP.sin_port = htons(service);
    
    ser_IP2.sin_addr.s_addr = inet_addr("10.0.0.11");
    ser_IP2.sin_family = AF_INET;
    ser_IP2.sin_port = htons( 6666 );
    

    int s =connect(sock , (struct sockaddr *)&ser_IP , sizeof(ser_IP)) ;
    
    if(s<0)
    {
        printf("connect to server 1 failed\n");
        int y =connect(sock , (struct sockaddr *)&ser_IP2 , sizeof(ser_IP2)) ;
        if(y>=0) { puts("\n\nConnect to server 2 Successfull\n");}
    }
    
    else if(s>=0)
    {
        puts("Connection established to Server 1 Successfull\n");
    }
    
    if(s>=0 || y>=0 )
    {
    
    memset(&reqBuffer[0],0,sizeof(reqBuffer));
    strcpy(reqBuffer,"Professor");
    send(sock,reqBuffer,sizeof(reqBuffer),0);
    memset(&reqBuffer[0],0,sizeof(reqBuffer));
    
    memset(&resBuffer[0],0,sizeof(resBuffer));
    recv(sock,resBuffer,sizeof(resBuffer),0);
    loginToPortal(sock,resBuffer);
    
    if(strcmp(resBuffer, "1")==0){
        printf("\n\nOptions available\na. Insert student data\nb. Update office hours\nc. Add new test\nd. Upload new document\n");
        printf("\nPlease select your option : ");
        scanf(" %c%*c",&uInput);
        
        switch(uInput){
            case 'a':
                insertStudentData(sock);
                break;
            case 'b':
                updateOfficeHours(sock);
                break;
            case 'c':
                uploadTest(sock);
                break;
            case 'd':
                uploadFile(sock);
                break;
                
            default:
                printf("Invalid user Input");
                break;
        }
    }
    else if(strcmp(resBuffer, "2")==0){
        
        errexit("Your password is invalid: %s\n",strerror(errno));
    }
    else if(strcmp(resBuffer, "3")==0){
        
        errexit("Your user name is invalid: %s\n",strerror(errno));
    }
    }
    
    else{printf("Server is down");}

    close(sock);
    
    return 0;
}

/***************************************************************************************************/
/*   Function for login procedure                                                                  */
/***************************************************************************************************/

void loginToPortal(int sock,char resBuffer[]){
    
    printf("%s",resBuffer);
    scanf("%s",pCredential.uName);
    
    strncpy(pCredential.pwd, getpass("Password: "), 1024);
    send(sock,(void *)&pCredential,sizeof(pCredential),0);
    
    memset(&resBuffer[0],0,BLEN);
    recv(sock,resBuffer,BLEN,0);
}

/***************************************************************************************************/
/*   Function to insert data                                                                       */
/***************************************************************************************************/

void insertStudentData(int sock)
{
    char reqBuffer[BLEN],resBuffer[BLEN];
    
    memset(&reqBuffer[0], 0, BLEN);
    strcpy(reqBuffer,"a");
    send(sock,reqBuffer,strlen(reqBuffer),0);
    memset(&reqBuffer[0], 0, BLEN);
    
    memset(&resBuffer[0], 0, BLEN);
    recv(sock,resBuffer,BLEN,0);
    
    printf("%s",resBuffer);
    scanf("%s",reqBuffer);
    
    //Send the filename to the server
    send(sock,reqBuffer,sizeof(reqBuffer),0);
    
    memset(&resBuffer[0], 0, BLEN);
    recv(sock,resBuffer,BLEN,0);
    
    printf("%s",resBuffer);
}

/***************************************************************************************************/
/*          Function to update office hours in file by Professor                                   */
/***************************************************************************************************/

void updateOfficeHours(int sock)
{
    char reqBuffer[BLEN],resBuffer[BLEN];
    char *day,*time,*i;
    
    day = (char *)malloc(100);
    time = (char *)malloc(100);
    i = (char *)malloc(100);
    
    memset(&reqBuffer[0], 0, BLEN);
    strcpy(reqBuffer,"b");
    send(sock,reqBuffer,strlen(reqBuffer),0);
    memset(&reqBuffer[0], 0, BLEN);
    
    memset(&resBuffer[0], 0, BLEN);
    recv(sock,resBuffer,sizeof(resBuffer),0);
    
    printf("%s",resBuffer);
    printf("Appointment time <Friday 10:00 AM> : ");
    scanf("%s %s %s",day,time,i);
    
    strcpy(reqBuffer,day);
    strcat(reqBuffer," ");
    strcat(reqBuffer,time);
    strcat(reqBuffer," ");
    strcat(reqBuffer,i);
    
    send(sock,reqBuffer,strlen(reqBuffer),0);
    memset(&reqBuffer[0], 0, BLEN);
    memset(&resBuffer[0], 0, BLEN);
    
    recv(sock,resBuffer,BLEN,0);
    
    printf("%s",resBuffer);
}

/***************************************************************************************************/
/*          Function for upload document by the  Professor                                         */
/***************************************************************************************************/

void uploadFile(int sock)
{
    char reqBuffer[BLEN],resBuffer[BLEN],path[100];
    size_t fs_block_sz;
    int blen=BLEN;
    long n;
    
    memset(&reqBuffer[0], 0, BLEN);
    strcpy(reqBuffer,"d");
    send(sock,reqBuffer,strlen(reqBuffer),0);
    memset(&reqBuffer[0], 0, BLEN);
    
    memset(&resBuffer[0], 0, BLEN);
    recv(sock,resBuffer,BLEN,0);
    
    printf("%s",resBuffer);
    scanf("%s",reqBuffer);
    
    //Send the filename to the server
    send(sock,reqBuffer,sizeof(reqBuffer),0);
    
    strcpy(path,".");
    strcat(path,"/Files/");
    strcat(path,reqBuffer);
    memset(&reqBuffer[0], 0, BLEN);
    memset(&resBuffer[0], 0, BLEN);
    
    FILE *fs = fopen(path, "r");
    
    if(fs == NULL)
    {
        errexit("File not found :%s\n",strerror(errno));
    }
    
    while((fs_block_sz = fread(reqBuffer, sizeof(char), BLEN, fs))>0)
    {
        if(send(sock, reqBuffer, fs_block_sz, 0) < 0)
        {
            errexit("Failed to send file :%s\n",strerror(errno));
            break;
        }
        memset(&reqBuffer[0], 0, BLEN);
        
    }
    
    char *resptr=resBuffer;
    
    //Receive data from server
    while((n=recv(sock, resptr, blen, 0))>0){
        resptr+=n;
        blen-=n;
    }
    printf("%s\n",resBuffer);
    memset(&resBuffer[0], 0, BLEN);
}

/***************************************************************************************************/
/*          Function for upload new online exam by the  Professor                                  */
/***************************************************************************************************/

void uploadTest(int sock)
{
    char reqBuffer[BLEN];
    
    memset(&reqBuffer[0], 0, BLEN);
    strcpy(reqBuffer,"c");
    send(sock,reqBuffer,strlen(reqBuffer),0);
    memset(&reqBuffer[0], 0, BLEN);
    
    uploadQuestions(sock);
}

/***************************************************************************************************/
/*          Function for upload new online exam by the  Professor                                  */
/***************************************************************************************************/

void uploadQuestions(int sock)
{
    
    size_t fs_block_sz;
    char resBuffer[BLEN],reqBuffer[BLEN],path[100];
    int blen=BLEN;
    
    
    memset(&resBuffer[0], 0, BLEN);
    recv(sock,resBuffer,BLEN,0);
    
    printf("%s",resBuffer);
    scanf("%s",reqBuffer);
    
    //Send the filename to the server
    send(sock,reqBuffer,sizeof(reqBuffer),0);
    
    strcpy(path,".");
    strcat(path,"/Files/Test/Question/");
    strcat(path,reqBuffer);
    memset(&reqBuffer[0], 0, BLEN);
    memset(&resBuffer[0], 0, BLEN);
    printf("opening the file\n");
    FILE *fs = fopen(path, "r");
    
    if(fs == NULL)
    {
        errexit("File not found :%s\n",strerror(errno));
    }
    
    while((fs_block_sz = fread(reqBuffer, sizeof(char), BLEN, fs))>0)
    {
        if(send(sock, reqBuffer, fs_block_sz, 0) < 0)
        {
            errexit("Failed to send file :%s\n",strerror(errno));
            break;
        }
        memset(&reqBuffer[0], 0, BLEN);
        
    }
    
    recv(sock, resBuffer, blen, 0);
    printf("%s",resBuffer);
    memset(&resBuffer[0], 0, BLEN);
    
    uploadAnswers(sock);
}

/***************************************************************************************************/
/*          Function for upload new online exam solution by the  Professor                         */
/***************************************************************************************************/

void uploadAnswers(int sock)
{
    size_t fs_block_sz;
    char resBuffer[BLEN],reqBuffer[BLEN],path[100];
    int blen=BLEN;
    long n;
    
    //printf("%s",resBuffer);
    scanf("%s",reqBuffer);
    
    //Send the filename to the server
    send(sock,reqBuffer,sizeof(reqBuffer),0);
    
    strcpy(path,".");
    strcat(path,"/Files/Test/Answer/");
    strcat(path,reqBuffer);
    memset(&reqBuffer[0], 0, BLEN);
    memset(&resBuffer[0], 0, BLEN);
    
    FILE *fs = fopen(path, "r");
    
    if(fs == NULL)
    {
        errexit("File not found :%s\n",strerror(errno));
    }
    
    while((fs_block_sz = fread(reqBuffer, sizeof(char), BLEN, fs))>0)
    {
        if(send(sock, reqBuffer, fs_block_sz, 0) < 0)
        {
            errexit("Failed to send file :%s\n",strerror(errno));
            break;
        }
        memset(&reqBuffer[0], 0, BLEN);
    }
    
    char *resptr=resBuffer;
    
    //Receive data from server
    while((n=recv(sock, resptr, blen, 0))>0)
    {
        resptr+=n;
        blen-=n;
    }
    printf("%s",resBuffer);
    memset(&resBuffer[0], 0, BLEN);
}


