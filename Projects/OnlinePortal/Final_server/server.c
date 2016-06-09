
//
//  Created by Divya Vannilaparambath Karippath on 9/24/15.
//  Copyright © 2015 Divya Vannilaparambath Karippath. All rights reserved.
//

/***************************************************************************************************/
/*                                     Server Program                                              */
/*                                     **************                                              */
/*                                                                                                 */
/*                                                                                                 */
/*                                   Student online Portal                                         */
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
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dirent.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <pthread.h>

#define BLEN 1024
#define QLEN 32

int fileCount,appointmentCount;

struct Appointment
{
    char item_no[10];
    char name[50];
    char status[25];
    int  sId; // ID of student who booked this appointment
};

struct {
    pthread_mutex_t	st_mutex;
    unsigned int	st_concount;
    unsigned int	st_contotal;
} stats;

struct sockaddr_in serverAddr;

struct studentLogin{
    int * sId;
    char **pwd;
}sLogin;

struct professorLogin{
    char uName[1024];
    char uPwd[1024];
}pLogin;

struct studentCredential{
    int sId;
    char pwd[1024];
}sCredential;

struct professorCredential{
    char uName[1024];
    char pwd[1024];
}pCredential;

struct Appointments
{
    int item_no;
    char name[1024];
    char status[1024];
    int  sId; // ID of student who booked this appointment
};

struct Appointments *appointment;

// Function Definitions
int  passiveTCP(const char *service, int qlen);
int  errexit(const char *format, ...);
void loadStudentData();
void processUploadRequest(int ssd);
void processTestUploadRequest(int ssd);
void processAnswersUpload(int ssd);
void processDownloadRequest(int ssd);
void processStudentLoginRequest(int ssd);
void processProfessorLoginRequest(int ssd);
void loadProfessorData();
void insertStudentData(int ssd);
void updateOfficeHours(int ssd);
void processtestRequest(int ssd);
void Appointment();
void Book_Appointment(int);
void prstats(void);
void *new_connection(void *);


int main(int argc, char * argv[])
{
    
    int msd,ssd;
    struct sockaddr_in clientAddr;
    unsigned int clientAddrlen;
    char reqBuffer[BLEN],resBuffer[BLEN];
    char *service;
    int pId;
    pthread_t	th, nwctn;
    pthread_attr_t	ta;
    stats.st_contotal = 0;
    
    
    switch (argc)   // validate user input
    {
        case	2:
            service = argv[1];
            break;
        default:
                errexit("Usage: %s <Server Port>\n", argv[0]);
    }
    
    (void) pthread_attr_init(&ta);
    (void) pthread_attr_setdetachstate(&ta, PTHREAD_CREATE_DETACHED);
    (void) pthread_mutex_init(&stats.st_mutex, 0);
    
    
    loadStudentData();
    
    loadProfessorData();
    
    Appointment();
   
    msd = passiveTCP(service,QLEN);
    
    while(1)
    {
        //creating new socket
        while((ssd = accept(msd,(struct sockaddr *)&clientAddr, &clientAddrlen)))
        {
            puts("Connection accepted");
            
            if(pthread_create( &nwctn , NULL ,  new_connection , (void*) &ssd) < 0)
                errexit("pthread_create: %s\n", strerror(errno));
            
            (void) pthread_mutex_lock(&stats.st_mutex);
            stats.st_contotal++;
            (void) pthread_mutex_unlock(&stats.st_mutex);
            
            if (pthread_create(&th, &ta, (void * (*)(void *))prstats, 0) < 0)
                errexit("pthread_create(prstats): %s\n", strerror(errno));
        }
        
        if(ssd<0)
        {
          errexit("accept failed: %s\n", strerror(errno));
        }
        
     close(ssd);
    }
    
    return 0;
}

/***************************************************************************************************/
/*                         Function for every new connection                                       */
/*   New threads gets created for each new client request                                          */
/*                                                                                                 */
/***************************************************************************************************/

void *new_connection(void *socket_desc)
{
    int ssd = *(int*)socket_desc;
    unsigned int clientAddrlen;
    char reqBuffer[BLEN],resBuffer[BLEN];
    int pId;
    
    memset(&reqBuffer[0],0,sizeof(reqBuffer));
    recv(ssd,reqBuffer,sizeof(reqBuffer),0);
        
    if(strcmp(reqBuffer,"Student")==0)
    {
        memset(&resBuffer[0],0,sizeof(resBuffer));
            
        strcpy(resBuffer,"\n\t\t\tWelcome to the Student Portal\n\t\t\t=============================\n\n\t\rPlease enter your studentId : ");
        send(ssd,resBuffer,sizeof(resBuffer),0);
        memset(&resBuffer[0],0,sizeof(resBuffer));
        recv(ssd,&sCredential,sizeof(sCredential),0);
            
        processStudentLoginRequest(ssd); // call student login function
            
        memset(&reqBuffer[0], 0, sizeof(reqBuffer));
        recv(ssd,reqBuffer,BLEN,0);
        char uInput = reqBuffer[0];
            
        switch(uInput)
        {
                    
            case 'a':
                Book_Appointment(ssd);
                break;
            case 'b':
                processtestRequest(ssd);
                break;
            case 'c':
                processDownloadRequest(ssd);
                break;
            case 'd':
                pId = fork();
                if(pId==0)
                {
                    execv("./chatserver",(char *[]){"./chatserver","9999",NULL});
                }
                break;
            default:
                printf("Invalid user Input\n");
                break;
        }
        
        memset(&reqBuffer[0], 0, sizeof(reqBuffer));
    }
        
    else if(strcmp(reqBuffer,"Professor")==0)
    {
            
        memset(&resBuffer[0],0,sizeof(resBuffer));
        strcpy(resBuffer,"\n\t\t\tWelcome to the Student Portal\n\t\t\t=============================\n\n\t\r \n\nPlease enter your userName : ");
        send(ssd,resBuffer,sizeof(resBuffer),0);
        memset(&resBuffer[0],0,sizeof(resBuffer));
        
        recv(ssd,&pCredential,sizeof(pCredential),0);
        processProfessorLoginRequest(ssd); // call Professor login function
            
        memset(&reqBuffer[0], 0, sizeof(reqBuffer));
        recv(ssd,reqBuffer,BLEN,0);
        char uInput = reqBuffer[0];
            
        switch(uInput)
        {
            case 'a':
                insertStudentData(ssd);
                break;
            case 'b':
                updateOfficeHours(ssd);
                break;
            case 'c':
                processTestUploadRequest(ssd);
                break;
            case 'd':
                processUploadRequest(ssd);
                break;
            default:
                printf("Invalid user Input\n");
                break;
        }
        memset(&reqBuffer[0], 0, sizeof(reqBuffer));
    }
}


/***************************************************************************************************/
/*                         Function to load student login file                                     */
/***************************************************************************************************/

void loadStudentData()
{
    int size=0;
    char password[1024];
    FILE *fp = fopen("./Files/Login/students.txt", "r");
    
    if(fp == NULL)
    {
        printf("File not found\n");
    }
    
    while (!feof(fp))
    {
        memset(&password[0],0,sizeof(password));
        sLogin.sId=(int *)realloc(sLogin.sId,(size + 1) * sizeof(int));
        sLogin.pwd = (char **)realloc(sLogin.pwd, (size + 1) * sizeof(char *));
        
        fscanf(fp, "%d,%s", &sLogin.sId[size],password);
        sLogin.pwd[size]=strdup(password);
        size++;
    }
    
    fileCount=size;
    
}

/***************************************************************************************************/
/*                         Function to load Professor login file                                   */
/***************************************************************************************************/

void loadProfessorData()
{
    char *full_file_string;
    FILE *fp = fopen("./Files/Login/professor.txt", "r");
    if(fp == NULL)
    {
        printf("File not found\n");
    }
    full_file_string = (char *)malloc(100);
    fgets (full_file_string, BLEN, fp);
    
    strcpy(pLogin.uName ,strtok(full_file_string, ","));
    strcpy(pLogin.uPwd ,strtok(NULL, ","));
    free(full_file_string);
}

/***************************************************************************************************/
/*          Function to insert student information in file by Professor                            */
/***************************************************************************************************/

void insertStudentData(int ssd)
{
    char resBuffer[BLEN], reqBuffer[BLEN];
    
    memset(&resBuffer[0], 0, BLEN);
    strcpy(resBuffer,"Please enter the studentID : ");
    send(ssd, resBuffer,strlen(resBuffer),0);
    memset(&resBuffer[0], 0, BLEN);
    
    memset(&reqBuffer[0], 0, BLEN);
    recv(ssd, reqBuffer,sizeof(reqBuffer),0);
    
    FILE *fp = fopen("./Files/Login/students.txt", "a");
    
    if(fp == NULL)
    {
        printf("File not found\n");
    }
    
    fprintf(fp, " %s,%s\n",reqBuffer,reqBuffer );
    fclose(fp);
    
    memset(&reqBuffer[0], 0, BLEN);
    memset(&resBuffer[0], 0, BLEN);
    strcpy(resBuffer,"Data updated successfully\n");
    send(ssd, resBuffer,strlen(resBuffer),0);
    memset(&resBuffer[0], 0, BLEN);
}

/***************************************************************************************************/
/*          Function to update office hours in file by Professor                                   */
/***************************************************************************************************/

void updateOfficeHours(int ssd)
{
    char resBuffer[BLEN],reqBuffer[BLEN];
    
    memset(&resBuffer[0], 0, BLEN);
    strcpy(resBuffer,"Please enter the appointment details\n");
    send(ssd, resBuffer,strlen(resBuffer),0);
    memset(&resBuffer[0], 0, BLEN);
    
    memset(&reqBuffer[0], 0, BLEN);
    recv(ssd,reqBuffer,sizeof(reqBuffer),0);
    
    FILE *fp = fopen("./Files/Appointment/appointment.txt", "a");
    
    if(fp == NULL)
    {
        printf("File not found\n");
    }
    
    fprintf(fp, " %s,%s,%s\n",reqBuffer,"Available","0");
    fclose(fp);
    
    memset(&reqBuffer[0], 0, BLEN);
    memset(&resBuffer[0], 0, BLEN);
    strcpy(resBuffer,"Data updated successfully\n");
    send(ssd, resBuffer,strlen(resBuffer),0);
    memset(&resBuffer[0], 0, BLEN);
}

/***************************************************************************************************/
/*          Function for student login  option                                                     */
/***************************************************************************************************/

void processStudentLoginRequest(int ssd)
{
    int flag=0;
    char resBuffer[BLEN];
    memset(&resBuffer[0],0,sizeof(resBuffer));
    for (int i=0;i<fileCount;i++)
    {
        if(sCredential.sId==sLogin.sId[i])
        {
            flag=1;
            if(strcmp(sCredential.pwd,sLogin.pwd[i])==0)
            {
                flag=2;
                memset(&resBuffer[0], 0, BLEN);
                strcpy(resBuffer,"1");
                break;
            }
            else
            {
                memset(&resBuffer[0], 0, BLEN);
                strcpy(resBuffer,"2");
                break;
            }
        }
    }
    
    if(flag==0)
    {
        memset(&resBuffer[0], 0, BLEN);
        strcpy(resBuffer,"3");
    }
    
    send(ssd, resBuffer,strlen(resBuffer),0);
    memset(&resBuffer[0],0,sizeof(resBuffer));
}

/***************************************************************************************************/
/*          Function for Professor login  option                                                   */
/***************************************************************************************************/

void processProfessorLoginRequest(int ssd)
{
    char resBuffer[BLEN];
    memset(&resBuffer[0],0,sizeof(resBuffer));
    if(strcmp(pLogin.uName,pCredential.uName)==0)
    {
        if(strcmp(pLogin.uPwd,pCredential.pwd)==0)
        {
            strcpy(resBuffer,"1");
        }
        else
        {
            strcpy(resBuffer,"2");
        }
    }
    else
    {
        strcpy(resBuffer,"3");
    }
    send(ssd, resBuffer,strlen(resBuffer),0);
    memset(&resBuffer[0],0,sizeof(resBuffer));
}

/***************************************************************************************************/
/*          Function for student to select appointment option                                      */
/***************************************************************************************************/

void Appointment()
{
    int size=0;
    int item_num = 1001;
    char *fullString = NULL;
    char *token;
    
    FILE *fp = fopen("./Files/Appointment/appointment.txt", "r");
    
    if(fp == NULL)
    {
        printf("File not found\n");
    }
    
    while (!feof(fp))
    {
        fullString = (char *)malloc(100);
        
        appointment=(struct Appointments *)realloc(appointment,(size + 1) * sizeof(struct Appointments));
        
        fgets (fullString, BLEN, fp);
        
        size_t newbuflen = strlen(fullString);
        if (fullString[newbuflen - 1] == '\n') fullString[newbuflen - 1] = '\0';
        
        if((token=strsep(&fullString, ",")) != NULL)
        {
            strcpy(appointment[size].name,token);
        }
        
        if((token=strsep(&fullString, ",")) != NULL)
        {
            strcpy(appointment[size].status,token);
        }
        
        if((token=strsep(&fullString, ",")) != NULL)
        {
            appointment[size].sId=atoi(token);
        }
        
        appointment[size].item_no=item_num++;
        free(fullString);
        
        size++;
    }
    appointmentCount=size;
}

/***************************************************************************************************/
/*          Function for student to book appointment with  Professor                               */
/***************************************************************************************************/

void Book_Appointment(int ssock)
{
    unsigned int j = 0;
    int flag =0;
    char buffer[1024],buffer2[BLEN];
    char input;
    bzero(buffer,1024);
    //Send welcome note
    strcpy(buffer,"\n\n\t\t\tWelcome to the Student Portal Reservation\n\t\t\t=========================================\n\n\t\r\n Options available\n1. Book an Appointment with the professor\n2. Cancel your appointment\n");
    strcat(buffer,"\n\nPlease select your option : ");
   
    if(send(ssock,buffer,strlen(buffer), 0) < 0)
        errexit("send_failed: %s\n", strerror(errno));
    
    bzero(buffer,1024);
    //Receive a reply from the server
    if(read(ssock,buffer,1024) < 0)
        errexit("read_failed: %s\n", strerror(errno));
    
    input=buffer[0];
    
    switch (input)
    {
        case	'1':
            bzero(buffer,1024);
            
            strcpy(buffer," List_no \t\t\t\tAppointment_time \t\tAvailability \n ------- \t\t\t\t---------------- \t\t------------ \n");
            
            for (j=0;j<appointmentCount-1;j++)
            {
                bzero(buffer2,1024);
                snprintf(buffer2,sizeof(buffer2),"%d",appointment[j].item_no);
                
                strcat(buffer," ");
                strcat(buffer,buffer2);
                strcat(buffer,"\t\t\t");
                strcat(buffer,appointment[j].name);
                strcat(buffer,"\t\t");
                strcat(buffer,appointment[j].status);
                strcat(buffer,"\n");
            }
            
            strcat(buffer,"\nPlease select one of the available slots : ");
            if(send(ssock,buffer,strlen(buffer), 0) < 0)
                errexit("send_failed: %s\n", strerror(errno));
            
            
            bzero(buffer,1024);
            //Receive a reply from the server
            if(read(ssock,buffer,1024) < 0)
                errexit("read_failed: %s\n", strerror(errno));
            int num = atoi(buffer);
            
            for(j=0;j<appointmentCount-1;j++)
            {
                if(num==appointment[j].item_no)
                {
                    if(strcmp(appointment[j].status,"Available")==0)
                    {
                        strcpy(appointment[j].status,"    NA   ");
                        appointment[j].sId = sCredential.sId;
                        bzero(buffer,1024);
                        strcpy(buffer,"\t Your appointment is booked for:");
                        strcat(buffer,appointment[j].name);
                    }
                    else if(strcmp(appointment[j].status,"    NA   ")==0)
                    {
                        bzero(buffer,1024);
                        strcpy(buffer,"\t Selected slot is not avaiable");
                    }
                    break;
                }
            }
            break;
        case 	'2':
            
            bzero(buffer,1024);
            strcpy(buffer," List_no \t\t\t\tAppointment_time \t\tAvailability \n ------- \t\t\t\t---------------- \t\t------------ \n");
            
            for (j=0;j<appointmentCount-1;j++)
            {
                printf("%d\n",appointment[j].sId);
                printf("%d\n",sCredential.sId);
                if(appointment[j].sId==sCredential.sId)
                {
                    bzero(buffer2,1024);
                    snprintf(buffer2,sizeof(buffer2),"%d",appointment[j].item_no);
                    flag++;
                    strcat(buffer," ");
                    strcat(buffer,buffer2);
                    strcat(buffer,"\t\t\t");
                    strcat(buffer,appointment[j].name);
                    strcat(buffer,"\t\t");
                    strcat(buffer,appointment[j].status);
                    strcat(buffer,"\n");
                }
                
            }
            
            if(flag==0)
            {
                bzero(buffer,1024);
                strcat(buffer,"0");
            }
            else
            {
                strcat(buffer,"\n\nPlease select the slot for cancellation : ");
                if(send(ssock,buffer,strlen(buffer), 0) < 0)
                    errexit("send_failed: %s\n", strerror(errno));
                
                bzero(buffer,1024);
                //Receive a reply from the server
                if(read(ssock,buffer,1024) < 0)
                    errexit("read_failed: %s\n", strerror(errno));
                
                //printf("Student ID: %s\n",buffer);
                int num = atoi(buffer);
                for(j=0;j<appointmentCount-1;j++)
                {
                    if(num==appointment[j].item_no)
                    {
                        if(appointment[j].sId==sCredential.sId)
                        {
                            strcpy(appointment[j].status,"Available");
                            appointment[j].sId=0;
                            bzero(buffer,1024);
                            strcpy(buffer,"\t Your booking has been cancelled\n.The appointment for:");
                            strcat(buffer,appointment[j].name);
                            strcat(buffer," is avaiable now");
                        }
                        else
                        {
                            bzero(buffer,1024);
                            strcpy(buffer,"\t Invalid choice: Selected slot was not booked by you");
                        }
                        break;
                    }
                }
            }
            break;
        default:
            printf("   Unknown message type \n");
            strcpy(buffer,"Invalid option: Choice \n");
    }
    
    
    if(send(ssock,buffer,strlen(buffer), 0) < 0)
        errexit("send_failed: %s\n", strerror(errno));
    
    FILE *fp = fopen("./Files/Appointment/appointment.txt", "w");
    
    if(fp == NULL)
    {
        printf("File not found\n");
    }
    for(j=0;j<appointmentCount-1;j++)
    {
        if(appointment[j].sId==010722270)
        {
            printf("1\n");
        }
        
        fprintf(fp, " %s,%s,%d\n",appointment[j].name,appointment[j].status,appointment[j].sId);
    }
    
    fclose(fp);
    fflush(stdout);
    
}

/***************************************************************************************************/
/*          Function for upload document by the  Professor                                         */
/***************************************************************************************************/

void processUploadRequest(int ssd)
{
    char reqBuffer[BLEN],resBuffer[BLEN];
    long fr_block_sz;
    
    memset(&resBuffer[0], 0, BLEN);
    strcpy(resBuffer,"Please enter the filename to be uploaded\n");
    send(ssd, resBuffer,strlen(resBuffer),0);
    memset(&resBuffer[0], 0, BLEN);
    
    memset(&reqBuffer[0], 0, BLEN);
    recv(ssd,reqBuffer,sizeof(reqBuffer),0);
    char path[100];
    strcpy(path,".");
    strcat(path,"/Files/Uploads/");
    strcat(path,reqBuffer);
    memset(&reqBuffer[0], 0, sizeof(reqBuffer));
    
    FILE *fr = fopen(path, "a");
    
    if(fr==NULL)
    {
        printf("File cant be opened \n");
    }
    else
    {
        while((fr_block_sz = recv(ssd, reqBuffer, BLEN, 0))>0)
        {
            size_t write_sz = fwrite(reqBuffer, sizeof(char), fr_block_sz, fr);
            if(write_sz < fr_block_sz)
            {
                printf("File write failed \n");
            }
            memset(&reqBuffer[0], 0, sizeof(reqBuffer));
            if (fr_block_sz == 0 || fr_block_sz != BLEN)
            {
                break;
            }
        }
        
        if(fr_block_sz<0)
        {
            printf("Error receiving file from client\n");
        }
        
        fclose(fr);
    }
    memset(&resBuffer[0], 0, BLEN);
    strcpy(resBuffer,"File uploaded successfully\n");
    send(ssd, resBuffer,strlen(resBuffer),0);
}

/***************************************************************************************************/
/*          Function for upload new online exam by the  Professor                                  */
/***************************************************************************************************/

void processTestUploadRequest(int ssd)
{
    char reqBuffer[BLEN],resBuffer[BLEN];
    long fr_block_sz;
    
    memset(&resBuffer[0], 0, BLEN);
    strcpy(resBuffer,"Please enter the question file name to be uploaded\n");
    send(ssd, resBuffer,strlen(resBuffer),0);
    memset(&resBuffer[0], 0, BLEN);
    
    memset(&reqBuffer[0], 0, BLEN);
    recv(ssd,reqBuffer,sizeof(reqBuffer),0);
    
    printf("filename received\n");
    char path[100];
    strcpy(path,".");
    strcat(path,"/Files/Test/questions/");
    strcat(path,reqBuffer);
    memset(&reqBuffer[0], 0, sizeof(reqBuffer));
    
    FILE *fr = fopen(path, "a");
    
    if(fr==NULL)
    {
        printf("File cant be opened \n");
    }
    else
    {
        while((fr_block_sz = recv(ssd, reqBuffer, BLEN, 0))>0)
        {
            size_t write_sz = fwrite(reqBuffer, sizeof(char), fr_block_sz, fr);
            if(write_sz < fr_block_sz)
            {
                printf("File write failed on server \n");
            }
            memset(&reqBuffer[0], 0, sizeof(reqBuffer));
            if (fr_block_sz == 0 || fr_block_sz != BLEN)
            {
                break;
            }
        }
        
        if(fr_block_sz<0)
        {
            printf("Error receiving file from client\n");
        }
        
        fclose(fr);
    }
    memset(&resBuffer[0], 0, BLEN);
    strcpy(resBuffer,"Question File uploaded successfully\nPlease enter the answer file name\n");
    send(ssd, resBuffer,strlen(resBuffer),0);
    processAnswersUpload(ssd);
}

/***************************************************************************************************/
/*          Function for upload new online exam solution by the  Professo                          */
/***************************************************************************************************/

void processAnswersUpload(int ssd)
{
    char reqBuffer[BLEN],resBuffer[BLEN];
    long fr_block_sz;
    
    memset(&reqBuffer[0], 0, BLEN);
    recv(ssd,reqBuffer,sizeof(reqBuffer),0);
    char path[100];
    strcpy(path,".");
    strcat(path,"/Files/Test/answers/");
    strcat(path,reqBuffer);
    memset(&reqBuffer[0], 0, sizeof(reqBuffer));
    
    FILE *fr = fopen(path, "a");
    
    if(fr==NULL)
    {
        printf("File cant be opened \n");
    }
    else
    {
        while((fr_block_sz = recv(ssd, reqBuffer, BLEN, 0))>0)
        {
            size_t write_sz = fwrite(reqBuffer, sizeof(char), fr_block_sz, fr);
            if(write_sz < fr_block_sz)
            {
                printf("File write failed on server\n");
            }
            memset(&reqBuffer[0], 0, sizeof(reqBuffer));
            if (fr_block_sz == 0 || fr_block_sz != BLEN)
            {
                break;
            }
        }
        
        if(fr_block_sz<0)
        {
            printf("Error receiving file from client\n");
        }
        
        fclose(fr);
    }
    memset(&resBuffer[0], 0, BLEN);
    strcpy(resBuffer,"Answer File uploaded successfully\n");
    send(ssd, resBuffer,strlen(resBuffer),0);
}

/***************************************************************************************************/
/*          Function for download the reference  material from the portal                          */
/***************************************************************************************************/

void processDownloadRequest(int ssd)
{
    int i=0,strcount=0;
    DIR  *d;
    struct dirent *dir;
    size_t fs_block_sz;
    char resBuffer[BLEN],reqBuffer[BLEN],path[100];
    char **strarray=NULL;
    d = opendir("./Files/Uploads");
    
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            if (dir->d_type == DT_REG)
            {
                strarray = (char **)realloc(strarray, (strcount + 1) * sizeof(char *));
                strarray[strcount++] = strdup(dir->d_name);
            }
        }
        closedir(d);
    }
    
    memset(&resBuffer[0], 0, BLEN);
    snprintf(resBuffer,sizeof(resBuffer),"%d",strcount);
    send(ssd,(void *)resBuffer,sizeof(resBuffer),0);
    memset(&resBuffer[0], 0, BLEN);
    
    for(i = 1; i < strcount; i++)
    {
        strcpy(resBuffer,strarray[i]);
        send(ssd,(void *)resBuffer,sizeof(resBuffer),0);
        memset(&resBuffer[0],0,sizeof(resBuffer));
        free(strarray[i]);
    }
    
    free(strarray);
    
    memset(&reqBuffer[0], 0, BLEN);
    recv(ssd,reqBuffer,sizeof(reqBuffer),0);
    
    memset(&path[0],0,sizeof(path));
    strcpy(path,".");
    strcat(path,"/Files/Uploads/");
    strcat(path,reqBuffer);
    memset(&reqBuffer[0], 0, BLEN);
    
    FILE *fs = fopen(path, "r");
    
    if(fs == NULL)
    {
        printf("File not found \n");
    }
    
    memset(&resBuffer[0], 0, BLEN);
    
    while((fs_block_sz = fread(resBuffer, sizeof(char), BLEN, fs))>0)
    {
        if(send(ssd, resBuffer, fs_block_sz, 0) < 0)
        {
            printf("Failed to send file \n");
            break;
        }
        memset(&resBuffer[0], 0, BLEN);
    }
    memset(&path[0],0,sizeof(path));
}

/***************************************************************************************************/
/*          Function for to take online exam by the  Student                                       */
/***************************************************************************************************/

void processtestRequest(int ssd)
{
    char buffer1[2048];
    char buffer2[2048];
    char buffer3[2048];
    char *hey[2048];
    char copy[2048];
    
    /** Declaration for directories**/
    DIR *d;
    struct dirent *dir;
    int i_dir=0,j_dir=0;
    
    /** Declaration for displaying file contents and calculating the d **/
    FILE *fp,*fp_1;
    int i_file=0,j_file=0,score=0;
    char ch,ch_1,filename[2048],file[2048],buffer_ans[2048],buffer_ques[2048],ans[2048];
    
    
    printf("\nstudent wants to take an online exam\n");
    strcpy(buffer2,"\n\nTest series available -");
    send(ssd,(void *)buffer2,sizeof(buffer2),0);
    
    d=opendir("./Files/Test/questions");
    if(d)
    {
        while((dir =readdir(d))!= NULL)
        {
            if (dir->d_type == DT_REG)
            {
                hey[i_dir]=dir->d_name;
                i_dir++;
            }
        }
        closedir (d);
    }
    
    memset(&buffer2[0],0,sizeof(buffer2));
    snprintf(buffer2,sizeof(buffer2),"%d",i_dir);
    send(ssd,(void *)buffer2,sizeof(buffer2),0);
   
    for(j_dir=1;j_dir<i_dir;j_dir++)
    {
        strcpy(copy,hey[j_dir]);
        send(ssd,(void *)copy,sizeof(copy),0);
        memset(&copy[0],0,sizeof(copy));
    }
    
    recv(ssd,buffer1,sizeof(buffer1),0);
    strcpy(filename,buffer1);

    memset(&file[0],0,sizeof(file));
    memset(&buffer_ques[0],0,sizeof(buffer_ques));
    strcpy(file,"./Files/Test/questions/");
    strcat(file,filename);
    fp=fopen(file, "r");
    while(ch!=EOF)
    {
        ch=fgetc(fp);
        buffer_ques[i_file]=ch;
        i_file++;
    }
    
    send(ssd,(void *)buffer_ques,sizeof(buffer_ques),0);
    fclose(fp);
   
    memset(&file[0],0,sizeof(file));
    strcpy(file,"./Files/Test/answers/");
    strcat(file,filename);
    fp_1=fopen(file, "r");
    memset(&buffer_ans[0],0,sizeof(buffer_ans));
    while(ch_1!=EOF)
    {
        ch_1=fgetc(fp_1);
        buffer_ans[j_file]=ch_1;
        j_file++;
    }
  
    fclose(fp_1);
 
    j_file=0;
    memset(&buffer1[0],0,sizeof(buffer1));
    recv(ssd,buffer1,sizeof(buffer1),0);
    strcpy(ans,buffer1);
 
    for(i_file=0;i_file<strlen(ans);i_file++)
    {
        if(ans[i_file]==buffer_ans[j_file])
        {
            score=score+1;
        }
        
        j_file++;
    }
    memset(&buffer2[0],0,sizeof(buffer2));
    snprintf(buffer2,sizeof(buffer2),"%d",score);
    send(ssd,(void *)buffer2,sizeof(buffer2),0);
   
    memset(&buffer1[0],0,sizeof(buffer1));
    memset(&buffer2[0],0,sizeof(buffer2));
    memset(&buffer3[0],0,sizeof(buffer3));
    memset(&hey[0],0,sizeof(hey));
}

/***************************************************************************************************/
/*          Function to print count of the total number of client connections to server            */
/***************************************************************************************************/

void prstats(void)
{
    (void) pthread_mutex_lock(&stats.st_mutex);
    (void) printf("%-32s: %u\n", "Total number of client connections",stats.st_contotal);
    (void) pthread_mutex_unlock(&stats.st_mutex);
}
