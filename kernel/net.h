#define MBUF_SIZE 2048

struct mbuf {
	unsigned char*             head;
	unsigned int                len;
	unsigned char buffer[MBUF_SIZE];
};

#define ETH_ALEN        6
#define ETH_DATA_LEN 1500
#define ETH_HLEN       14

#define ETH_P_IPV4 0x0800
#define ETH_P_ARP  0x0806
struct __attribute__((packed)) eth_hdr {
	uint8 dst[ETH_ALEN];
	uint8 src[ETH_ALEN];
	uint16    ethertype;
};

#define ARP_HLEN 8

#define ARP_HWTYPE_ETHERNET     1
#define ARP_PTYPE_IPV4 ETH_P_IPV4
#define ARP_OP_REQUEST          1
#define ARP_OP_REPLY            2
/**
 * From RFC 826
 * 16.bit: (ar$hrd) Hardware address space (e.g., Ethernet,
 *                  Packet Radio Net.)
 * 16.bit: (ar$pro) Protocol address space.  For Ethernet
 *                  hardware, this is from the set of type
 *                  fields ether_typ$<protocol>.
 *  8.bit: (ar$hln) byte length of each hardware address
 *  8.bit: (ar$pln) byte length of each protocol address
 * 16.bit: (ar$op)  opcode (ares_op$REQUEST | ares_op$REPLY)
 * nbytes: (ar$sha) Hardware address of sender of this
 *                  packet, n from the ar$hln field.
 * mbytes: (ar$spa) Protocol address of sender of this
 *                  packet, m from the ar$pln field.
 * nbytes: (ar$tha) Hardware address of target of this
 *                  packet (if known).
 * mbytes: (ar$tpa) Protocol address of target
*/
struct __attribute__((packed)) arp_hdr {
	uint16 hwtype; // Hardware Address Space
	uint16  ptype; // Protocol Address Space
	uint8  hwalen; // Hardware Address Byte Length
	uint8   palen; // Protocol Address Byte Length
	uint16 opcode; // Operation
};

#define IPV4_ALEN 4
