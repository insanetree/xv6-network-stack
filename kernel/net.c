#include "types.h"
#include "riscv.h"
#include "memlayout.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"

#include "net.h"
#include "e1000.h"

static uint8 host_mac[ETH_ALEN];
static uint8 host_ip[IPV4_ALEN] = {10, 0, 2, 15};

static uint8 nh_mac[ETH_ALEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static uint8 nh_ip[IPV4_ALEN] = {10, 0, 2, 2};

#define ip_arr_to_u32(ip_arr) (*((uint32*)&ip_arr))

static inline uint16
swap16(uint16 val)
{
	return ((val & 0xff) << 8) | (val >> 8);
}


void
net_stack_init()
{
	e1000_intr_en(0); // Disable Interrupts
	if(get_mac_addr(host_mac)) {
		panic("Mac address not initialized");
	}

	arp_tx(ip_arr_to_u32(nh_ip));
	return;
}

void
eth_tx(void* in, uint16 len, uint16 ethertype)
{
	struct eth_hdr hdr;
	memmove(hdr.dst, nh_mac, ETH_ALEN);
	memmove(hdr.src, host_mac, ETH_ALEN);
	hdr.ethertype = swap16(ethertype);
	struct mbuf* tx_buf;
	e1000_get_tx_buf(&tx_buf);
	tx_buf->len = len + ETH_HLEN;
	tx_buf->head = tx_buf->buffer;
	memmove(tx_buf->buffer, &hdr, ETH_HLEN);
	memmove(tx_buf->buffer + ETH_HLEN, in, len);
	e1000_tx(tx_buf);
}

void
ipv4_tx()
{

}

void
arp_tx(uint32 ip)
{
	static const uint16 size = ARP_HLEN + 2 * (ETH_ALEN + IPV4_ALEN);
	uint8 buff[size];

	uint8* arp_hdr_ptr = &buff[0];
	uint8* sha = &arp_hdr_ptr[ARP_HLEN];
	uint8* spa = &sha[ETH_ALEN];
	uint8* tha = &spa[IPV4_ALEN];
	uint8* tpa = &tha[ETH_ALEN];

	memset(buff, 0, size);
	struct arp_hdr arp;
	arp.hwtype = swap16(ARP_HWTYPE_ETHERNET);
	arp.hwalen = ETH_ALEN;
	arp.ptype = swap16(ARP_PTYPE_IPV4);
	arp.palen = IPV4_ALEN;
	arp.opcode = swap16(ARP_OP_REQUEST);

	memmove(arp_hdr_ptr, &arp, ARP_HLEN);
	memmove(sha, host_mac, ETH_ALEN);
	memmove(spa, host_ip, IPV4_ALEN);
	// tha is 0
	memmove(tpa, nh_ip, IPV4_ALEN);

	eth_tx(buff, size, ETH_P_ARP);
}
