#include	"ping.h"
#include   <pthread.h>

char			recvbuf[BUFSIZE];
struct msghdr	msg;
ssize_t			n;
struct timeval	tval;

void readloop(void)
{
    //char			recvbuf[BUFSIZE];
    char			controlbuf[BUFSIZE];
    //struct msghdr	msg;
    struct iovec	iov;
    //ssize_t			n;
    //struct timeval	tval;
    int	size;
    int ttl = 32;
    
    pthread_t pt1,pt2;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    
    
    sockfd = socket(pr->sasend->sa_family, SOCK_RAW, pr->icmpproto);
    setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl));
    
    setuid(getuid());
    if (pr->finit)
        (*pr->finit)();
    
    size = 60 * 1024;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));
    
   if(pthread_create(&pt1, &attr, (void *(*)(void *))sendData,0)<0){
        
        printf("Error creating thread\n");
        exit(1);
    }
    
    iov.iov_base = recvbuf;
    iov.iov_len = sizeof(recvbuf);
    msg.msg_name = pr->sarecv;
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_control = controlbuf;
    for ( ; ; ) {
        msg.msg_namelen = pr->salen;
        msg.msg_controllen = sizeof(controlbuf);
        n = recvmsg(sockfd, &msg, 0);
        //printf("N : %zd\n",n);
        if (n < 0) {
            if (errno == EINTR)
                continue;
            else{
                printf("recvmsg error");
                exit(1);
            }
            
        }
        
        gettimeofday(&tval, NULL);
        if(pthread_create(&pt2, &attr, (void *(*)(void *))receiveData,0)<0){
            
            printf("Error creating thread\n");
            exit(1);
        }

    }
}



void sendData(){
    
    while(1){
        (*pr->fsend)();
        sleep(1);
    }
}

void receiveData(){
    
    (*pr->fproc)(recvbuf, n, &msg, &tval);
    sleep(1);
   
}
