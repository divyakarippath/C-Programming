//
//  ex2_client.c
//  HW1_UDP
//
//  Created by Divya Vannilaparambath Karippath on 9/23/15.
//  Copyright © 2015 Divya Vannilaparambath Karippath. All rights reserved.
//

#include <stdio.h>
#include <sys/socket.h>
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

int connectUDP(const char *addr,const char *port);
void processlistdata(int csd);
void processorderdata(int csd,char resBuffer[]);
int handleerror(const char *errmsg);

struct sockaddr_in serveraddr;
unsigned int addrlen = sizeof(serveraddr);

int main(int argc, const char * argv[]) {
    
    int csd;
    char resBuffer[BLEN],reqBuffer [BLEN];

    strcpy(reqBuffer, argv[3]);
    
    csd = connectUDP(argv[1],argv[2]);
    
    printf("Sending data to server ,host:%s port:%s\n",argv[1],argv[2]);
    
    sendto(csd, reqBuffer, strlen(reqBuffer), 0, (struct sockaddr *)&serveraddr, addrlen);
    
    if(strcmp(reqBuffer,"list") == 0){
        
        processlistdata(csd);
    }
    
    else{
        
        processorderdata(csd,resBuffer);
    }
    
    close(csd);
    
    return 0;
}


int connectUDP(const char *addr,const char *port){
    
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(atoi(port));
    serveraddr.sin_addr.s_addr = inet_addr(addr);
    //sin.sin_addr.s_addr = INADDR_ANY;
    
    //Creating socket
    int csd = socket(PF_INET, SOCK_DGRAM, 0);
    
    if(csd<0){
        handleerror("Error in creating socket for client");
    }

    return csd;
}

void processlistdata(int csd){
    
    int i;
    
    //Receive data from server
    
    recvfrom(csd, (char *)inventory, sizeof(inventory), 0, (struct sockaddr *)&serveraddr, &addrlen);
    
    printf("Item Number\tTitle\tQuantity\n");
    
    for(i=0;i<3;i++){
        
        printf("%d\t%s\t%d\n",inventory[i].itemNum,inventory[i].title,inventory[i].quantity);
    }

}

void processorderdata(int csd,char resBuffer[]){
    
    //Receive data from server
    recvfrom(csd, resBuffer, BLEN, 0,(struct sockaddr *)&serveraddr, &addrlen);
    printf("%s\n",resBuffer);
}

int handleerror(const char *errmsg){
    
    printf("%s\n",errmsg);
    exit(1);
}

