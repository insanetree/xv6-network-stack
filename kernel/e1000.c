#include <assert.h>

#include "types.h"
#include "riscv.h"
#include "memlayout.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"

#include "pci.h"
#include "e1000.h"
#include "net.h"

struct spinlock e1000_lock;

static volatile uint32* regs = 0;
static volatile struct e1000_ctrl* ctrl = 0;

static volatile struct e1000_rctl* rctl = 0;
static volatile uint64*  rdba = 0;
static volatile uint32* rdlen = 0;
static volatile uint32*   rdh = 0;
static volatile uint32*   rdt = 0;


static volatile struct e1000_tctl* tctl = 0;
static volatile uint32* tdbal = 0;
static volatile uint32* tdbah = 0;
static volatile uint32* tdlen = 0;
static volatile uint32* tdh   = 0;
static volatile uint32* tdt   = 0;

#define RX_RING_SIZE 32
struct rx_desc rx_ring[RX_RING_SIZE] __attribute__((aligned(16)));
struct mbuf rx_mbuf[RX_RING_SIZE];

#define TX_RING_SIZE 32
struct tx_desc tx_ring[TX_RING_SIZE] __attribute__((aligned(16)));
struct mbuf tx_mbuf[TX_RING_SIZE];

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
	static_assert(sizeof(struct tx_desc) == 16);
	static_assert(sizeof(tx_ring) % 128 == 0);

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

	// RX init
	rctl = (struct e1000_rctl*)&regs[E1000_RCTL];
	rdba = (uint64*)(&regs[E1000_RDBA]);
	rdlen = &regs[E1000_RDLEN];
	rdh = &regs[E1000_RDH];
	rdt = &regs[E1000_RDT];

	memset(&rx_ring, 0, RX_RING_SIZE * sizeof(struct rx_desc));
	for(uint i = 0 ; i < RX_RING_SIZE ; i++) {
		rx_mbuf[i].head = (void*)(rx_mbuf[i].buffer);
		rx_ring[i].addr = (uint64)rx_mbuf[i].head;
	}
	
	*rdba = (uint64)(&rx_ring);
	*rdlen = sizeof(rx_ring);
	*rdh = 0;
	*rdt = RX_RING_SIZE - 1;


	// TX control
	static_assert(sizeof(*tctl) == sizeof(uint32));
	memset(tctl, 0, sizeof(*tctl));
	tctl->en = 1;
	tctl->psp = 1;

	// RX control
	static_assert(sizeof(*rctl) == sizeof(uint32));
	memset(rctl, 0, sizeof(*rctl));	
	rctl->en = 1;
	rctl->bam = 1;
	rctl->secrc = 1;


	return;
}