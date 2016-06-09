//
//  ex1_server.c
//  HW1
//
//  Created by Divya Vannilaparambath Karippath on 9/24/15.
//  Copyright © 2015 Divya Vannilaparambath Karippath. All rights reserved.
//


#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

#define BLEN 1024

struct DVDInventory{
    int itemNum;
    char title[100];
    int quantity;
}inventory[3];

void loadInventoryData();
int passiveTCP();
int handleerror(const char *errmsg);
void processlistrequest(int ssd);
void processorderrequest(int ssd,char reqBuffer[],char resBuffer[]);

struct sockaddr_in serverAddr;

int main(int argc, const char * argv[]) {
    
    int msd,ssd;
    struct sockaddr_in clientAddr;
    unsigned int clientAddrlen;
    char reqBuffer[BLEN],resBuffer[BLEN];
    
    loadInventoryData();
    msd = passiveTCP();
    
    clientAddrlen = sizeof(clientAddr);
    
    while(1){
        
        //creating new socket
        ssd = accept(msd,(struct sockaddr *)&clientAddr, &clientAddrlen);
        if(ssd<0){
            handleerror("accept failed");
        }
        printf("Connection accepted from Client IP : %s\tClient Port : %d\n",inet_ntoa(clientAddr.sin_addr),ntohs(clientAddr.sin_port));
        printf("Listening on Server IP : %s\tServer Port : %d\n",inet_ntoa(serverAddr.sin_addr),ntohs(serverAddr.sin_port));
        
        //Receiving request from client
        recv(ssd, reqBuffer, BLEN, 0);
            
        
        if (strcmp(reqBuffer,"list") == 0){
            
            processlistrequest(ssd);
            
        }else{
            
            processorderrequest(ssd,reqBuffer,resBuffer);
        }
        
        memset(&reqBuffer[0], 0, sizeof(reqBuffer));
        
        close(ssd);
        
    }
    
    return 0;
}

void loadInventoryData(){
    
    
    inventory[0].itemNum = 1001;
    inventory[0].quantity = 100;
    strcpy(inventory[0].title, "Star Wars");
    
    inventory[1].itemNum = 1002;
    inventory[1].quantity = 80;
    strcpy(inventory[1].title, "Harry Potter");
    
    inventory[2].itemNum = 1003;
    inventory[2].quantity = 50;
    strcpy(inventory[2].title, "Inside Out");
    
}

int passiveTCP(){
    
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8888);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    //serverAddr.sin_addr.s_addr = INADDR_ANY;
    
    
    //Create master socket
    int msd = socket(PF_INET, SOCK_STREAM, 0);
    
    if(msd<0){
        handleerror("Error in creating socket for server");
    }
    
    if(bind(msd, (struct sockaddr *)&serverAddr, sizeof(serverAddr))<0){
        handleerror("Error in binding");
    }
    
    //set the server in passive mode
    if(listen(msd, 5)==0){
        
        printf("Listening\n");
        
    }
    else{
        
        handleerror("can't listen in specified port");
    }
    
    return msd;
}

void processlistrequest(int ssd){
    
    printf("Request Type : LIST\n");
    
    //send data to client
    send(ssd,(void *)&inventory,sizeof(inventory),0);
}

void processorderrequest(int ssd,char reqBuffer[],char resBuffer[]){
    
    printf("Request Type : ORDER\n");
    
    char *token1;
    char *token2;
    char *token3;
    char *search = " ";
    bool foundItem = false;
    
    token1 = strtok(reqBuffer, search);
    token2 = strtok(NULL, search);
    token3 = strtok(NULL, search);
    int itemNum = atoi(token2);
    int quantity = atoi(token3);
    int j;
    
    printf("Requested itemnumber: %s and quantity: %s\n",token2,token3);
    for(j=0;j<3;j++){
        
        if(itemNum == inventory[j].itemNum){
            foundItem =true;
            
            if(inventory[j].quantity >= quantity){
                strcpy(resBuffer, "OK\n");
                inventory[j].quantity-=quantity;
            }
            else{
                strcpy(resBuffer, "Sorry! not enough stock\n");
            }
            send(ssd, resBuffer,strlen(resBuffer),0);
            
            
            break;
            
        }
        
    }
    
    if(foundItem==false){
        
        strcpy(resBuffer, "Requested item is not present in our inventory\n");
        send(ssd, resBuffer,strlen(resBuffer),0);
    }
}

int handleerror(const char *errmsg){
    
    printf("%s\n",errmsg);
    exit(1);
}
