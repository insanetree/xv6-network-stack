struct semaphore
{
	struct spinlock lock;
	int            count;
};
