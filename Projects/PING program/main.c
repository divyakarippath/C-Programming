#include	"ping.h"


struct proto	proto_v4 = { proc_v4, send_v4, NULL, NULL, NULL, 0, IPPROTO_ICMP };

int	datalen = 56;		/* data that goes with ICMP echo request */

int main(int argc, char **argv)
{
	int				c;
	struct addrinfo	*ai;
	char *h;
    
    opterr = 0;		
	while ( (c = getopt(argc, argv, "v")) != -1) {
		switch (c) {
		case 'v':
			verbose++;
			break;

		case '?':
			printf("unrecognized option: %c", c);
		}
	}

	if (optind != argc-1)
		printf("usage: ping [ -v ] <hostname>");
	host = argv[optind];

	pid = getpid() & 0xffff;	/* ICMP ID field is 16 bits */
	

	ai = Host_serv(host, NULL, 0, 0);
    h = Sock_ntop_host(ai->ai_addr, ai->ai_addrlen);
    
	printf("PING %s (%s): %d data bytes\n",
			ai->ai_canonname ? ai->ai_canonname : h,
			h, datalen);

		/* 4initialize according to protocol */
	if (ai->ai_family == AF_INET) {
        
		pr = &proto_v4;

	}  else
		printf("unknown address family %d", ai->ai_family);

	pr->sasend = ai->ai_addr;
	pr->sarecv = calloc(1, ai->ai_addrlen);
	pr->salen = ai->ai_addrlen;
    
    readloop();
    
    exit(0);
}


struct addrinfo * host_serv(const char *host, const char *serv, int family, int socktype)
{
    int				n;
    struct addrinfo	hints, *res;
    
    bzero(&hints, sizeof(struct addrinfo));
    hints.ai_flags = AI_CANONNAME;	/* always return canonical name */
    hints.ai_family = family;		/* AF_UNSPEC, AF_INET, AF_INET6, etc. */
    hints.ai_socktype = socktype;	/* 0, SOCK_STREAM, SOCK_DGRAM, etc. */
    
    if ( (n = getaddrinfo(host, serv, &hints, &res)) != 0)
        return(NULL);
    
    return(res);	/* return pointer to first on linked list */
}


struct addrinfo * Host_serv(const char *host, const char *serv, int family, int socktype)
{
    int				n;
    struct addrinfo	hints, *res;
    
    bzero(&hints, sizeof(struct addrinfo));
    hints.ai_flags = AI_CANONNAME;	/* always return canonical name */
    hints.ai_family = family;		/* 0, AF_INET, AF_INET6, etc. */
    hints.ai_socktype = socktype;	/* 0, SOCK_STREAM, SOCK_DGRAM, etc. */
    
    if ( (n = getaddrinfo(host, serv, &hints, &res)) != 0)
        printf("host_serv error for %s, %s: %s",
                 (host == NULL) ? "(no hostname)" : host,
                 (serv == NULL) ? "(no service name)" : serv,
                 gai_strerror(n));
    
    return(res);	/* return pointer to first on linked list */
}



char * sock_ntop_host(const struct sockaddr *sa, socklen_t salen)
{
    static char str[128];		/* Unix domain is largest */
    
            struct sockaddr_in	*sin = (struct sockaddr_in *) sa;
            
            if (inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str)) == NULL)
                return(NULL);
            return(str);

}

char *Sock_ntop_host(const struct sockaddr *sa, socklen_t salen)
{
    char	*ptr;
    
    if ( (ptr = sock_ntop_host(sa, salen)) == NULL)
        printf("sock_ntop_host error");	/* inet_ntop() sets errno */
    return(ptr);
}


