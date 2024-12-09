#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "stat.h"
#include "spinlock.h"
#include "proc.h"
#include "sleeplock.h"
#include "fs.h"
#include "buf.h"
#include "file.h"
#include "net.h"

enum sock_type {SOCKET_RAW, SOCKET_STREAM, SOCKET_DATAGRAM};
enum sock_state {SOCK_CLOSED, SOCK_OPEN};

struct sock {
	enum sock_state state;
	enum sock_type   type;
	uint16           port;
	struct spinlock  lock;
	struct mbufq    queue;
};

#define SOCK_ARR_LEN 10

struct sock sock_array[SOCK_ARR_LEN];

void
ksock_init() {
	for(int i = 0 ; i < SOCK_ARR_LEN ; i++) {
		sock_array[i].state = SOCK_CLOSED;
		initlock(&(sock_array[i].lock), "slock");
		sock_array[i].queue.head = 0;
		sock_array[i].queue.tail = 0;
	}
}