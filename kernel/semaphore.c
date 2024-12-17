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

void
sem_init(struct semaphore* s, int count)
{
	s->count = count;
	initlock(&s->lock, "semaphore lock");
}

void
sem_wait(struct semaphore* s)
{
	acquire(&s->lock);
	while(s->count == 0) {
		sleep(s, &s->lock);
	}
	s->count -= 1;
	release(&s->lock);
}

void
sem_post(struct semaphore* s)
{
	acquire(&s->lock);
	s->count += 1;
	wakeup(s);
	release(&s->lock);
}