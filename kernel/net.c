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

static inline uint32
swap32(uint32 val)
{
	return (
		((val & 0xff000000ul) >> 24) |
		((val & 0x00ff0000ul) >>  8) |
		((val & 0x0000ff00ul) <<  8) |
		((val & 0x000000fful) << 24)
	);
}

static inline void*
mbufgrow(struct mbuf* mbuf, uint16 len)
{
	if(len > MBUF_SIZE || mbuf->len + len > MBUF_SIZE) {
		panic("mbuf grow");
	}
	mbuf->len += len;
	mbuf->head -= len;
	return mbuf->head;
}

static inline void*
mbuftrim(struct mbuf* mbuf, uint16 len)
{
	if(len > MBUF_SIZE || mbuf->len < len) {
		panic("mbuf trim");
	}
	mbuf->len -= len;
	mbuf->head += len;
	return mbuf->head;
}

void
net_stack_init()
{
	e1000_intr_en(
		E1000_INT_RXT0 |
		E1000_INT_RXDMT0 |
		E1000_INT_RXO
	);
	if(get_mac_addr(host_mac)) {
		panic("Mac address not initialized");
	}

	arp_tx(ip_arr_to_u32(nh_ip));
	return;
}

void
eth_tx(struct mbuf* tx_buf, uint16 ethertype)
{
	struct eth_hdr* hdr = mbufgrow(tx_buf, ETH_HLEN);
	memmove(hdr->dst, nh_mac, ETH_ALEN);
	memmove(hdr->src, host_mac, ETH_ALEN);
	hdr->ethertype = swap16(ethertype);
	e1000_tx(tx_buf);
}

void
ipv4_tx(void* in, uint32 dest, uint16 len)
{

}

void
arp_tx(uint32 ip)
{
	struct mbuf* tx_buf;
	e1000_get_tx_buf(&tx_buf);
	memmove(mbufgrow(tx_buf, IPV4_ALEN), nh_ip, IPV4_ALEN);
	memmove(mbufgrow(tx_buf, ETH_ALEN), nh_mac, ETH_ALEN);
	memmove(mbufgrow(tx_buf, IPV4_ALEN), host_ip, IPV4_ALEN);
	memmove(mbufgrow(tx_buf, ETH_ALEN), host_mac, ETH_ALEN);
	struct arp_hdr* hdr = mbufgrow(tx_buf, ARP_HLEN);
	hdr->hwtype = swap16(ARP_HWTYPE_ETHERNET);
	hdr->ptype = swap16(ARP_PTYPE_IPV4);
	hdr->hwalen = ETH_ALEN;
	hdr->palen = IPV4_ALEN;
	hdr->opcode = swap16(ARP_OP_REQUEST);
	eth_tx(tx_buf, ETH_P_ARP);
}

void
ipv4_rx(struct mbuf* buff)
{

}

void
arp_rx(struct mbuf* buff)
{
	struct arp_hdr* hdr = (struct arp_hdr*)(buff->head);
	hdr->hwtype = swap16(hdr->hwtype);
	hdr->ptype = swap16(hdr->ptype);
	hdr->opcode = swap16(hdr->opcode);
	if(hdr->opcode == ARP_OP_REPLY) {
		if(strncmp((void*)nh_ip, (void*)hdr + ARP_HLEN + hdr->hwalen, hdr->palen) == 0) {
			memmove((void*)nh_mac, (void*)hdr + ARP_HLEN, hdr->hwalen);
		}	
	}
	buff->state = MBUF_FREE;
}

void
eth_rx(struct mbuf* rx_buf)
{
	struct eth_hdr* hdr = (struct eth_hdr*)(rx_buf->head);
	mbuftrim(rx_buf, ETH_HLEN);
	hdr->ethertype = swap16(hdr->ethertype);
	switch(hdr->ethertype) {
	case ETH_P_ARP:
		arp_rx(rx_buf);
		break;
	case ETH_P_IPV4:
		ipv4_rx(rx_buf);
		break;
	default:
		printf("Unrecognized ethertype %x\n", hdr->ethertype);
	}
}