#include "types.h"
#include "riscv.h"
#include "memlayout.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"

#include "net.h"
#include "e1000.h"

static uint8 mac[6];

void
net_stack_init()
{
	if(get_mac_addr(mac)) {
		panic("Mac address not initialized");
	}
	return;
}