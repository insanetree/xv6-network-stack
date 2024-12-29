#include "kernel/types.h"
#include "user/user.h"
#include "kernel/net.h"

int main()
{
	int sock = 0;
    sock = sock_open(0);
    uint64 buff[2048>>3];
    
    for (int i = 0; i < 50; i++)
    {
        icmp_echo(sock, 0x0a000202, i);
        sock_recv(sock, buff);
        struct icmp_hdr* hdr = (struct icmp_hdr*)&buff;
        printf("Ping echo reply seq=%d\n", hdr->un.echo.sequence);
    }


    close(sock);
    return 0;
}