#include	"unp.h"
#include	<netinet/in_systm.h>
#include	<netinet/ip.h>
#include	<netinet/ip_icmp.h>
#include    <stdio.h>
#include    <stdlib.h>
#include    <pthread.h>
#ifdef	HAVE_SOCKADDR_DL_STRUCT
#include	<net/if_dl.h>
#endif

#define	BUFSIZE		1500

			/* globals */
char	 sendbuf[BUFSIZE];

int		 datalen;			/* # bytes of data following ICMP header */
char	*host;
int		 nsent;				/* add 1 for each sendto() */
pid_t	 pid;				/* our PID */
int		 sockfd;
int		 verbose;

			/* function prototypes */

void	 proc_v4(char *, ssize_t, struct msghdr *, struct timeval *);
void	 send_v4(void);
void	 readloop(void);
void	 sig_alrm(int);
void	 tv_sub(struct timeval *, struct timeval *);
void     receiveData();
void     sendData();
int errexit(const char *format, ...);

struct proto {
  void	 (*fproc)(char *, ssize_t, struct msghdr *, struct timeval *);
  void	 (*fsend)(void);
  void	 (*finit)(void);
  struct sockaddr  *sasend;	/* sockaddr{} for send, from getaddrinfo */
  struct sockaddr  *sarecv;	/* sockaddr{} for receiving */
  socklen_t	    salen;		/* length of sockaddr{}s */
  int	   	    icmpproto;	/* IPPROTO_xxx value for ICMP */
} *pr;


