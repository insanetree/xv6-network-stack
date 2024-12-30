#include "kernel/types.h"
#include "user/user.h"
#include "kernel/net.h"

int main(int argc, char** argv)
{
    if(argc != 2) {
        printf("usage: ping <ip_addr>\n");
        return -1;
    }

    uint32 ip = 0;
    int ret;
    int octets = 4;
    char* ptr = argv[1];
    char* dot = argv[1];
    while(octets) {
        switch(*dot) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            dot++;
            break;
        case '.':
        case '\0':
            *dot = '\0';
            ret = atoi(ptr);
            if(ret & ~255) {
                printf("wrong ip addr format 1\n");
                return -1;
            }
            ip = (ip << 8) | ret;
            ptr = ++dot;
            octets--;
            break;
        default:
        	printf("%d\n", *dot);
            printf("wrong ip addr format 2\n");
            return -1;
        }
    }

	int sock = 0;
    sock = sock_open(0);
    uint64 buff[2048>>3];
    
    for (int i = 0; i < 50; i++)
    {
        icmp_echo(sock, ip, i);
        sock_recv(sock, buff);
        struct icmp_hdr* hdr = (struct icmp_hdr*)&buff;
        printf("Ping echo reply seq=%d\n", hdr->un.echo.sequence);
    }


    close(sock);
    return 0;
}