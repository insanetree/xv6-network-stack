#include "kernel/types.h"
#include "user/user.h"

int main()
{
	int sock = 0;
    sock = sock_open(0);
    close(sock);
    return 0;
}