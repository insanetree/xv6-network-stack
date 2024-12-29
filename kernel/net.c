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

// This code is lifted from FreeBSD's ping.c, and is copyright by the Regents
// of the University of California.
static unsigned short
in_cksum(const unsigned char *addr, int len)
{
  int nleft = len;
  const unsigned short *w = (const unsigned short *)addr;
  unsigned int sum = 0;
  unsigned short answer = 0;

  /*
   * Our algorithm is simple, using a 32 bit accumulator (sum), we add
   * sequential 16 bit words to it, and at the end, fold back all the
   * carry bits from the top 16 bits into the lower 16 bits.
   */
  while (nleft > 1)  {
    sum += *w++;
    nleft -= 2;
  }

  /* mop up an odd byte, if necessary */
  if (nleft == 1) {
    *(unsigned char *)(&answer) = *(const unsigned char *)w;
    sum += answer;
  }

  /* add back carry outs from top 16 bits to low 16 bits */
  sum = (sum & 0xffff) + (sum >> 16);
  sum += (sum >> 16);
  /* guaranteed now that the lower 16 bits of sum are correct */

  answer = ~sum; /* truncate to 16 bits */
  return answer;
}

void
ipv4_tx(struct mbuf* tx_buf, uint16 length, uint8 protocol, uint32 dest)
{
	struct ipv4_hdr* hdr = mbufgrow(tx_buf, IPV4_HLEN);
	hdr->ver_ihl = (IPPROTO_IPV4 << 4) | (IPV4_HLEN >> 2);
	hdr->tos = 0;
	hdr->length = swap16(IPV4_HLEN + length);
	hdr->identification = swap16(0);
	hdr->flg_foff = swap16((2 << 13) | 0); // Don't Fragment
	hdr->ttl = 255u;
	hdr->protocol = protocol;
	hdr->header_checksum = 0; // Calculate it later
	memmove(&(hdr->src), host_ip, IPV4_ALEN);
	hdr->dest = swap32(dest);
	hdr->header_checksum = in_cksum((uint8*)hdr, IPV4_HLEN);
	eth_tx(tx_buf, ETH_P_IPV4);
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
icmp_tx(uint16 id, uint16 sequence, uint32 dest)
{
	struct mbuf* tx_buf;
	e1000_get_tx_buf(&tx_buf);
	struct icmp_hdr* hdr = mbufgrow(tx_buf, sizeof(*hdr));
	hdr->checksum = 0;
	hdr->code = 0;
	hdr->type = ICMP_ECHO;
	hdr->un.echo.id = swap16(id);
	hdr->un.echo.sequence = swap16(sequence);
	hdr->checksum = in_cksum((uint8*)hdr, sizeof(*hdr));
	ipv4_tx(tx_buf, sizeof(*hdr), IPPROTO_ICMP, dest);
}

void
icmp_rx(struct mbuf* rx_buf)
{
	uint16 port = 0;
	struct icmp_hdr* hdr = (struct icmp_hdr*)rx_buf->head;
	if(rx_buf->len < sizeof(struct icmp_hdr)) {
		goto fail;
	}
	if(in_cksum((uint8*)hdr, rx_buf->len)) {
		goto fail;
	}
	switch(hdr->type) {
	case ICMP_ECHO:
	case ICMP_ECHOREPLY:
		port = hdr->un.echo.id;
		hdr->un.echo.id = swap16(hdr->un.echo.id);
		hdr->un.echo.sequence = swap16(hdr->un.echo.sequence);
		break;
	default:
		printf("Unimplemented ICMP: %d ", hdr->type);
		goto fail;
	}
	if(raw_sock_recv(rx_buf, swap16(port))) {
		printf("No open socket ");
		goto fail;
	}
	return;
fail:
	printf("ICMP message malformed\n");
	return;
}

void
ipv4_rx(struct mbuf* rx_buf)
{
	struct ipv4_hdr* hdr = (struct ipv4_hdr*)(rx_buf->head);

	if(rx_buf->len < sizeof(struct ipv4_hdr)) {
		printf("Received buffer has no space\n");
		goto fail;
	}
	if(rx_buf->len < swap16(hdr->length)) {
		goto fail;
	}
	if(in_cksum((uint8*)hdr, (hdr->ver_ihl & 0x0f) << 2)) {
		printf("Checksum fail\n");
		goto fail;
	}
	if(memcmp(&hdr->dest, host_ip, IPV4_ALEN)) {
		printf("Bad destination\n");
		goto fail;
	}

	mbuftrim(rx_buf, IPV4_HLEN);

	switch (hdr->protocol)
	{
	case IPPROTO_ICMP:
		icmp_rx(rx_buf);
		break;
	case IPPROTO_TCP:
	case IPPROTO_UDP:
	default:
		break;
	}

	return;
fail:
	printf("Packet dropped\n");
	return;
}

void
arp_rx(struct mbuf* rx_buf)
{
	struct arp_hdr* hdr = (struct arp_hdr*)(rx_buf->head);
	if(rx_buf->len < sizeof(struct arp_hdr)) {
		goto fail;
	}
	hdr->hwtype = swap16(hdr->hwtype);
	hdr->ptype = swap16(hdr->ptype);
	hdr->opcode = swap16(hdr->opcode);
	if(hdr->opcode == ARP_OP_REPLY) {
		if(strncmp((void*)nh_ip, (void*)hdr + ARP_HLEN + hdr->hwalen, hdr->palen) == 0) {
			memmove((void*)nh_mac, (void*)hdr + ARP_HLEN, hdr->hwalen);
		}	
	}
	return;
fail:
	printf("ARP message malformed\n");
	return;
}

void
eth_rx(struct mbuf* rx_buf)
{
	struct eth_hdr* hdr = (struct eth_hdr*)(rx_buf->head);
	if(rx_buf->len < sizeof(struct eth_hdr)) {
		goto fail;
	}
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
		goto fail;
	}
	return;
fail:
	printf("Frame dropped\n");
	return;
}