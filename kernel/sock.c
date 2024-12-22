#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "stat.h"
#include "spinlock.h"
#include "proc.h"
#include "sleeplock.h"
#include "semaphore.h"
#include "fs.h"
#include "buf.h"
#include "file.h"
#include "net.h"

enum sock_type {SOCKET_RAW, SOCKET_STREAM, SOCKET_DATAGRAM};
enum sock_state {SOCK_CLOSED, SOCK_OPEN};

struct sock {
	enum sock_state  state;
	enum sock_type    type;
	uint16            port;
	struct spinlock  mutex;
	struct semaphore count;
	struct mbufq     queue;
};

#define SOCK_ARR_LEN 10

struct sock sock_array[SOCK_ARR_LEN];

void
ksock_init() {
	for(int i = 0 ; i < SOCK_ARR_LEN ; i++) {
		sock_array[i].state = SOCK_CLOSED;
		initlock(&(sock_array[i].mutex), "sock mutex");
		sem_init(&(sock_array[i].count), 0);
		sock_array[i].queue.head = 0;
		sock_array[i].queue.tail = 0;
	}
}

int
ksock_open(uint16 port, struct sock** sock)
{
	for(int i = 0 ; i < SOCK_ARR_LEN ; i++) {
		if(sock_array[i].state == SOCK_OPEN) {
			continue;
		}
		*sock = &sock_array[i];
		(*sock)->state = SOCK_OPEN;
		(*sock)->port = port ? port : 0xc000 | i;
		(*sock)->type = SOCKET_RAW;
		return 0;
	}
	return -1;
}

int
sock_recv(struct mbuf* in_mbuf, enum sock_type type, uint16 port)
{
	for(int i = 0 ; i < SOCK_ARR_LEN ; i++) {
		if(sock_array[i].state == SOCK_CLOSED) {
			continue;
		}
		if(sock_array[i].type != type) {
			continue;
		}
		if(sock_array[i].port != port) {
			continue;
		}

		acquire(&sock_array[i].mutex);
		struct mbuf* mbuf = kalloc();
		memmove(mbuf, in_mbuf, sizeof(*in_mbuf));
		if(!sock_array[i].queue.head) { // queue is empty
			sock_array[i].queue.head = mbuf;
			sock_array[i].queue.tail = mbuf;
			mbuf->next = 0;
		} else {
			sock_array[i].queue.tail->next = mbuf;
			sock_array[i].queue.tail = mbuf;
		}
		sem_post(&sock_array[i].count);
		release(&sock_array[i].mutex);
		return 0;
	}
	return -1;
}

int
raw_sock_recv(struct mbuf* mbuf, uint16 port) {
	return sock_recv(mbuf, SOCKET_RAW, port);
}