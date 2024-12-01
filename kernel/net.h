#define MBUF_SIZE 2048

/**
 * Memory buffer for net stack
 */
enum mbuf_state {MBUF_FREE = 0, MBUF_TAKEN};
struct mbuf {
	unsigned char buffer[MBUF_SIZE];
	unsigned char*             head;
	unsigned int                len;
	enum mbuf_state           state;
	struct mbuf*               next;
};

struct mbufq {
	struct mbuf* head;
	struct mbuf* tail;
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

#define IPV4_ALEN  4
#define IPV4_HLEN 20

#define IPPROTO_IPV4 4

/* RFC 790 - Assigned Numbers*/

#define IPPROTO_ICMP  1
#define IPPROTO_TCP   6
#define IPPROTO_UDP  17
struct __attribute__((packed)) ipv4_hdr {
	uint8          ver_ihl;
	uint8              tos;
	uint16          length;
	uint16  identification;
	uint16        flg_foff;
	uint8              ttl;
	uint8         protocol;
	uint16 header_checksum;
	uint32             src;
	uint32            dest;
};