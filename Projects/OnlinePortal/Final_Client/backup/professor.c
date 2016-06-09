//
//  ex1_client.c
//  HW1
//
//  Created by Divya Vannilaparambath Karippath on 9/24/15.
//  Copyright © 2015 Divya Vannilaparambath Karippath. All rights reserved.
//


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

int connectTCP(const char *host,const char *service);
void uploadFile(int csd);
void loginToPortal(int csd,char resBuffer[]);
void uploadTest(int csd);
void uploadQuestions(int csd);
void uploadAnswers(int csd);
void insertStudentData(int csd);
void updateOfficeHours(int csd);
int errexit(const char *format, ...);

int main(int argc, char * argv[]) {
    
    int csd;
    char resBuffer[BLEN],reqBuffer[BLEN];
    char uInput;
    char *host,*service;
    
    host = argv[1];              /* server IP address (dotted quad) */
    service = argv[2];
    
    csd = connectTCP(host,service);
    
    memset(&reqBuffer[0],0,sizeof(reqBuffer));
    strcpy(reqBuffer,"Professor");
    send(csd,reqBuffer,sizeof(reqBuffer),0);
    memset(&reqBuffer[0],0,sizeof(reqBuffer));
    
    memset(&resBuffer[0],0,sizeof(resBuffer));
    recv(csd,resBuffer,sizeof(resBuffer),0);
    loginToPortal(csd,resBuffer);
    
    if(strcmp(resBuffer, "1")==0){
        printf("Options available\na. Insert student data\nb. Update office hours\nc. Add new test\nd. Upload new document\n");
        printf("Please select your option : ");
        scanf(" %c%*c",&uInput);
        
        switch(uInput){
            case 'a':
                insertStudentData(csd);
                break;
            case 'b':
                updateOfficeHours(csd);
                break;
            case 'c':
                uploadTest(csd);
                break;
            case 'd':
                uploadFile(csd);
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
    
    close(csd);
    
    return 0;
}


/*int connectTCP(const char *addr,const char *port){
 
 struct sockaddr_in serveraddr;
 serveraddr.sin_family = AF_INET;
 serveraddr.sin_port = htons(atoi(port));
 serveraddr.sin_addr.s_addr = inet_addr(addr);
 //sin.sin_addr.s_addr = INADDR_ANY;
 
 
 //Create socket
 
 int csd = socket(PF_INET, SOCK_STREAM, 0);
 
 if(csd<0){
 handleerror("Error in creating socket for client");
 }
 
 //Establish connection
 if (connect(csd, (struct sockaddr *)&serveraddr, sizeof(serveraddr))<0){
 
 handleerror("connection attempt to server failed");
 }
 
 return csd;
 }*/

void loginToPortal(int csd,char resBuffer[]){
    
    printf("%s",resBuffer);
    scanf("%s",pCredential.uName);
    
    strncpy(pCredential.pwd, getpass("Password: "), 1024);
    send(csd,(void *)&pCredential,sizeof(pCredential),0);
    
    memset(&resBuffer[0],0,BLEN);
    recv(csd,resBuffer,BLEN,0);
    
    
}

void insertStudentData(int csd){
    
    char reqBuffer[BLEN],resBuffer[BLEN];
    
    memset(&reqBuffer[0], 0, BLEN);
    strcpy(reqBuffer,"a");
    send(csd,reqBuffer,strlen(reqBuffer),0);
    memset(&reqBuffer[0], 0, BLEN);
    
    memset(&resBuffer[0], 0, BLEN);
    recv(csd,resBuffer,BLEN,0);
    
    printf("%s",resBuffer);
    scanf("%s",reqBuffer);
    
    //Send the filename to the server
    send(csd,reqBuffer,sizeof(reqBuffer),0);
    
    memset(&resBuffer[0], 0, BLEN);
    recv(csd,resBuffer,BLEN,0);
    
    printf("%s",resBuffer);
    
}

void updateOfficeHours(int csd){
    
    char reqBuffer[BLEN],resBuffer[BLEN];
    char *day,*time,*i;
    
    day = (char *)malloc(100);
    time = (char *)malloc(100);
    i = (char *)malloc(100);
    
    memset(&reqBuffer[0], 0, BLEN);
    strcpy(reqBuffer,"b");
    send(csd,reqBuffer,strlen(reqBuffer),0);
    memset(&reqBuffer[0], 0, BLEN);
    
    memset(&resBuffer[0], 0, BLEN);
    recv(csd,resBuffer,sizeof(resBuffer),0);
    
    printf("%s",resBuffer);
    printf("Appointment time (Friday 10:00 AM) : ");
    scanf("%s %s %s",day,time,i);
    
    strcpy(reqBuffer,day);
    strcat(reqBuffer," ");
    strcat(reqBuffer,time);
    strcat(reqBuffer," ");
    strcat(reqBuffer,i);
    
    send(csd,reqBuffer,strlen(reqBuffer),0);
    memset(&reqBuffer[0], 0, BLEN);
    memset(&resBuffer[0], 0, BLEN);
    
    recv(csd,resBuffer,BLEN,0);
    
    printf("%s",resBuffer);
}

void uploadFile(int csd){
    
    char reqBuffer[BLEN],resBuffer[BLEN],path[100];
    size_t fs_block_sz;
    int blen=BLEN;
    long n;
    
    memset(&reqBuffer[0], 0, BLEN);
    strcpy(reqBuffer,"d");
    send(csd,reqBuffer,strlen(reqBuffer),0);
    memset(&reqBuffer[0], 0, BLEN);
    
    memset(&resBuffer[0], 0, BLEN);
    recv(csd,resBuffer,BLEN,0);
    
    printf("%s",resBuffer);
    scanf("%s",reqBuffer);
    
    //Send the filename to the server
    send(csd,reqBuffer,sizeof(reqBuffer),0);
    
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
        if(send(csd, reqBuffer, fs_block_sz, 0) < 0)
        {
            errexit("Failed to send file :%s\n",strerror(errno));
            break;
        }
        memset(&reqBuffer[0], 0, BLEN);
        
    }
    
    char *resptr=resBuffer;
    
    //Receive data from server
    while((n=recv(csd, resptr, blen, 0))>0){
        resptr+=n;
        blen-=n;
    }
    printf("%s\n",resBuffer);
    memset(&resBuffer[0], 0, BLEN);
    
}

void uploadTest(int csd){
    
    char reqBuffer[BLEN];
    
    memset(&reqBuffer[0], 0, BLEN);
    strcpy(reqBuffer,"c");
    send(csd,reqBuffer,strlen(reqBuffer),0);
    memset(&reqBuffer[0], 0, BLEN);
    
    uploadQuestions(csd);
    
    
}

void uploadQuestions(int csd){
    
    size_t fs_block_sz;
    char resBuffer[BLEN],reqBuffer[BLEN],path[100];
    int blen=BLEN;
    
    
    memset(&resBuffer[0], 0, BLEN);
    recv(csd,resBuffer,BLEN,0);
    
    printf("%s",resBuffer);
    scanf("%s",reqBuffer);
    
    //Send the filename to the server
    send(csd,reqBuffer,sizeof(reqBuffer),0);
    
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
        if(send(csd, reqBuffer, fs_block_sz, 0) < 0)
        {
            errexit("Failed to send file :%s\n",strerror(errno));
            break;
        }
        memset(&reqBuffer[0], 0, BLEN);
        
    }
    
    /*char *resptr=resBuffer;
     
     //Receive data from server
     while((n=recv(csd, resptr, blen, 0))>0){
     resptr+=n;
     blen-=n;
     }*/
    recv(csd, resBuffer, blen, 0);
    printf("%s",resBuffer);
    memset(&resBuffer[0], 0, BLEN);
    
    uploadAnswers(csd);
}


void uploadAnswers(int csd){
    
    size_t fs_block_sz;
    char resBuffer[BLEN],reqBuffer[BLEN],path[100];
    int blen=BLEN;
    long n;
    
    //printf("%s",resBuffer);
    scanf("%s",reqBuffer);
    
    //Send the filename to the server
    send(csd,reqBuffer,sizeof(reqBuffer),0);
    
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
        if(send(csd, reqBuffer, fs_block_sz, 0) < 0)
        {
            errexit("Failed to send file :%s\n",strerror(errno));
            break;
        }
        memset(&reqBuffer[0], 0, BLEN);
        
    }
    
    char *resptr=resBuffer;
    
    //Receive data from server
    while((n=recv(csd, resptr, blen, 0))>0){
        resptr+=n;
        blen-=n;
    }
    printf("%s",resBuffer);
    memset(&resBuffer[0], 0, BLEN);
}




