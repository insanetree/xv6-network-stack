#include <assert.h>

#include "types.h"
#include "riscv.h"
#include "memlayout.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"

#include "pci.h"
#include "e1000.h"

struct spinlock e1000_lock;

static volatile union e1000_reg* regs= 0;

void
e1000_init(volatile union pcie_config_hdr* hdr) {
	printf("Configuring e1000 network card\n");
	assert(sizeof(union e1000_reg) == sizeof(uint32));	

	initlock(&e1000_lock, "e1000_spinlock");
	hdr->t0.cmd |= E1000_PCIE_CMD_IO_ENABLE |
	               E1000_PCIE_CMD_MMIO_ENABLE | 
	               E1000_PCIE_CMD_BUS_MASTERING_ENABLE;
	__sync_synchronize();
	hdr->t0.base_addr_regs[0] = E1000_BASE | E1000_PCIE_BAR0_64BIT;
	hdr->t0.base_addr_regs[1] = 0;
	__sync_synchronize();

	regs = (union e1000_reg*) E1000_BASE;


}