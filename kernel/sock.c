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

void
ksock_close(struct sock* sock)
{
	sock->state = SOCK_CLOSED;
	// TODO empty queue
}

int
ksock_recv(struct mbuf* in_mbuf, enum sock_type type, uint16 port)
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
	return ksock_recv(mbuf, SOCKET_RAW, port);
}

uint64
sys_icmp_echo()
{
	int fd;
	int dest;
	int seq;
	struct file* file;
	struct sock* sock;
	argint(1, &dest);
	argint(2, &seq);
	if(argfd(0, &fd, &file)) {
		goto fail;
	}
	sock = file->sock;
	if(!sock) {
		goto fail;
	}

	icmp_tx(sock->port, seq, dest);

	return 0;

fail:
	return -1;
}

uint64
sys_sock_recv()
{
	int fd;
	struct file* file;
	struct sock* sock;
	uint64 dest;
	argaddr(1, &dest);
	if(argfd(0, &fd, &file)) {
		goto fail_fd;
	}
	sock = file->sock;
	if(!sock) {
		goto fail_fd;
	}

	sem_wait(&sock->count);
	acquire(&sock->mutex);
	if(copyout(myproc()->pagetable, dest, (char*)sock->queue.head->head, sock->queue.head->len)) {
		goto fail_cpy;
	}

	struct mbuf* mbuf = sock->queue.head;
	sock->queue.head = mbuf->next;
	if(!sock->queue.head) {
		sock->queue.head = sock->queue.tail = 0;
	}
	kfree(mbuf);

	release(&sock->mutex);
	return 0;

fail_cpy:
	release(&sock->mutex);
fail_fd:
	return -1;
}