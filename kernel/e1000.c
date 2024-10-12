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

static volatile uint32* regs = 0;
static volatile struct e1000_ctrl* ctrl = 0;

static volatile struct e1000_tctl* tctl = 0;
static volatile uint32* tdbal = 0;
static volatile uint32* tdbah = 0;
static volatile uint32* tdlen = 0;
static volatile uint32* tdh   = 0;
static volatile uint32* tdt   = 0;

#define TX_RING_SIZE 32
struct tx_desc tx_ring[TX_RING_SIZE] __attribute__((aligned(16)));

void
e1000_init(volatile union pcie_config_hdr* hdr) {
	printf("Configuring e1000 network card\n");

	initlock(&e1000_lock, "e1000_spinlock");
	hdr->t0.cmd |= E1000_PCIE_CMD_IO_ENABLE |
	               E1000_PCIE_CMD_MMIO_ENABLE | 
	               E1000_PCIE_CMD_BUS_MASTERING_ENABLE;
	__sync_synchronize();
	hdr->t0.base_addr_regs[0] = E1000_BASE | E1000_PCIE_BAR0_64BIT;
	hdr->t0.base_addr_regs[1] = 0;
	__sync_synchronize();

	regs = (uint32*) E1000_BASE;
	ctrl = (struct e1000_ctrl*) &regs[E1000_CTRL];

	ctrl->rst = 0;
	__sync_synchronize();
	ctrl->asde = 1;
	ctrl->slu = 1;
	ctrl->ilos = 0;
	ctrl->vme = 0;
	__sync_synchronize();

	// TX init
	assert(sizeof(struct tx_desc) == 16);
	assert(sizeof(tx_ring) % 128 == 0);

	tctl  = (struct e1000_tctl*)&regs[E1000_TCTL];
	tdbal = &regs[E1000_TDBAL];
	tdbah = &regs[E1000_TDBAH];
	tdlen = &regs[E1000_TDLEN];
	tdh   = &regs[E1000_TDH];
	tdt   = &regs[E1000_TDT];

	memset(&tx_ring, 0, TX_RING_SIZE * sizeof(struct tx_desc));
	for(uint32 i = 0 ; i < TX_RING_SIZE ; i++) {
		tx_ring[i].status |= TX_DESC_STATUS_DD;
	}

	*tdbal = (uint32)((uint64)&tx_ring & 0xffffffff);
	*tdbah = (uint32)((uint64)&tx_ring >> 32);
	*tdlen = sizeof(tx_ring);
	*tdh   = 0;
	*tdt   = 0;

	// TX control
	tctl->en = 1;
	tctl->psp = 1;

	return;
}