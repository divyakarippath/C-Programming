//
//  ex1_client.c
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
#include <stdlib.h>
#include <unistd.h>

#define BLEN 1024

struct DVDInventory{
    int itemNum;
    char title[100];
    int quantity;
}inventory[3];

int connectTCP(const char *addr,const char *port);
void processlistdata(int csd);
void processorderdata(int csd,char resBuffer[]);
int handleerror(const char *errmsg);

int main(int argc, const char * argv[]) {
    
    char resBuffer[BLEN],reqBuffer[BLEN];
    int csd;
    strcpy(reqBuffer, argv[3]);
    
    csd = connectTCP(argv[1],argv[2]); // Establish the connection with server
    send(csd, reqBuffer,strlen(reqBuffer),0); //send the request
    
    if(strcmp(reqBuffer,"list") == 0){
        
        processlistdata(csd);
    }
    
    else{
        
        processorderdata(csd,resBuffer);
    }
    
    close(csd);
    
    return 0;
}


int connectTCP(const char *addr,const char *port){
    
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
}


void processlistdata(int csd){
    
    char *invptr = (char *)inventory;
    int invlen = sizeof(inventory);
    long n;
    int i;
    
    //Receive data from server
    while((n=recv(csd, invptr, invlen, 0))>0){
        invptr+=n;
        invlen-=n;
    }
    
    printf("Item Number\tTitle\tQuantity\n");
    
    for(i=0;i<3;i++){
        
        printf("%d\t%s\t%d\n",inventory[i].itemNum,inventory[i].title,inventory[i].quantity);
    }
    
}

void processorderdata(int csd,char resBuffer[]){
    
    char *resptr=resBuffer;
    int blen=BLEN;
    long n;
    
    //Receive data from server
    while((n=recv(csd, resptr, blen, 0))>0){
        resptr+=n;
        blen-=n;
    }
    printf("%s\n",resBuffer);
}

int handleerror(const char *errmsg){
    
    printf("%s\n",errmsg);
    exit(1);
}


