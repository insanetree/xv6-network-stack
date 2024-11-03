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

static volatile uint32 *regs = 0;
static volatile uint32 *ctrl = 0;

static volatile uint32 *rctl = 0;
static volatile uint64 *rdba = 0;
static volatile uint32 *rdlen = 0;
static volatile uint32 *rdh = 0;
static volatile uint32 *rdt = 0;

static volatile uint32 *tctl = 0;
static volatile uint32 *tipg = 0;
static volatile uint32 *tdbal = 0;
static volatile uint32 *tdbah = 0;
static volatile uint32 *tdlen = 0;
static volatile uint32 *tdh = 0;
static volatile uint32 *tdt = 0;

#define RX_RING_SIZE 32
struct rx_desc rx_ring[RX_RING_SIZE] __attribute__((aligned(16)));
struct mbuf rx_mbuf[RX_RING_SIZE];

#define TX_RING_SIZE 32
struct tx_desc tx_ring[TX_RING_SIZE] __attribute__((aligned(16)));
struct mbuf tx_mbuf[TX_RING_SIZE];

static volatile uint64 *ra = 0;

void e1000_init(volatile union pcie_config_hdr *hdr)
{
	printf("Configuring e1000 network card\n");

	initlock(&e1000_lock, "e1000_spinlock");
	hdr->t0.cmd |= E1000_PCIE_CMD_IO_ENABLE |
				   E1000_PCIE_CMD_MMIO_ENABLE |
				   E1000_PCIE_CMD_BUS_MASTERING_ENABLE;
	__sync_synchronize();
	hdr->t0.base_addr_regs[0] = E1000_BASE | E1000_PCIE_BAR0_64BIT;
	hdr->t0.base_addr_regs[1] = 0;
	__sync_synchronize();

	regs = (uint32 *)E1000_BASE;
	ctrl = &regs[E1000_CTRL];

	*ctrl |= E1000_CTRL_RST;
	__sync_synchronize();
	*ctrl = E1000_CTRL_ASDE | E1000_CTRL_SLU;
	__sync_synchronize();

	// TX init
	static_assert(sizeof(tx_ring) % 128 == 0);

	tctl = &regs[E1000_TCTL];
	tipg = &regs[E1000_TIPG];
	tdbal = &regs[E1000_TDBAL];
	tdbah = &regs[E1000_TDBAH];
	tdlen = &regs[E1000_TDLEN];
	tdh = &regs[E1000_TDH];
	tdt = &regs[E1000_TDT];

	memset(&tx_ring, 0, TX_RING_SIZE * sizeof(struct tx_desc));
	for (uint32 i = 0; i < TX_RING_SIZE; i++)
	{
		tx_ring[i].status |= TX_DESC_STATUS_DD;
	}

	*tdbal = (uint32)((uint64)&tx_ring & 0xffffffff);
	*tdbah = (uint32)((uint64)&tx_ring >> 32);
	*tdlen = sizeof(tx_ring);
	*tdh = 0;
	*tdt = 0;

	// RX init
	rctl = &regs[E1000_RCTL];
	rdba = (uint64 *)(&regs[E1000_RDBA]);
	rdlen = &regs[E1000_RDLEN];
	rdh = &regs[E1000_RDH];
	rdt = &regs[E1000_RDT];

	memset(&rx_ring, 0, RX_RING_SIZE * sizeof(struct rx_desc));
	for (uint i = 0; i < RX_RING_SIZE; i++)
	{
		rx_mbuf[i].head = (void *)(rx_mbuf[i].buffer);
		rx_ring[i].addr = (uint64)rx_mbuf[i].head;
	}

	*rdba = (uint64)(&rx_ring);
	*rdlen = sizeof(rx_ring);
	*rdh = 0;
	*rdt = RX_RING_SIZE - 1;

	// TX control
	*tctl = E1000_TCTL_EN | E1000_TCTL_PSP | (E1000_TCTL_CT_VAL << E1000_TCTL_CT_SHIFT) | (E1000_TCTL_COLD_VAL << E1000_TCTL_COLD_SHIFT);
	*tipg = (E1000_TIPG_IPGT_VAL << E1000_TIPG_IPGT_SHIFT) | (E1000_TIPG_IPGR1_VAL << E1000_TIPG_IPGR1_SHIFT) | (E1000_TIPG_IPGR2_VAL << E1000_TIPG_IPGR2_SHIFT);

	// RX control
	*rctl = E1000_RCTL_EN | E1000_RCTL_BAM | E1000_RCTL_SECRC;

	return;
}

int get_mac_addr(uint8 dest[])
{
	static_assert(sizeof(struct e1000_ra) == sizeof(uint64));
	for (uint8 i = 0; i < 16; i++)
	{
		ra = (uint64*)&regs[E1000_RA(i)];
		uint64 valid = *ra & E1000_RA_AV;
		uint64 select = (*ra & E1000_RA_AS_MASK) >> E1000_RA_AS_SHIFT;
		if (valid && select == 0)
		{
			dest[0] = E1000_RA_MAC(*ra, 0);
			dest[1] = E1000_RA_MAC(*ra, 1);
			dest[2] = E1000_RA_MAC(*ra, 2);
			dest[3] = E1000_RA_MAC(*ra, 3);
			dest[4] = E1000_RA_MAC(*ra, 4);
			dest[5] = E1000_RA_MAC(*ra, 5);
			return 0;
		}
	}
	return -1;
}