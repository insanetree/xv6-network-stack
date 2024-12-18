#define MBUF_SIZE 2048

/**
 * Memory buffer for net stack
 */
struct mbuf {
	unsigned char buffer[MBUF_SIZE];
	unsigned char*             head;
	unsigned int                len;
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

#define ICMP_ECHOREPLY		0	/* Echo Reply			*/
#define ICMP_DEST_UNREACH	3	/* Destination Unreachable	*/
#define ICMP_SOURCE_QUENCH	4	/* Source Quench		*/
#define ICMP_REDIRECT		5	/* Redirect (change route)	*/
#define ICMP_ECHO		    8	/* Echo Request			*/
#define ICMP_TIME_EXCEEDED	11	/* Time Exceeded		*/
#define ICMP_PARAMETERPROB	12	/* Parameter Problem		*/
#define ICMP_TIMESTAMP		13	/* Timestamp Request		*/
#define ICMP_TIMESTAMPREPLY	14	/* Timestamp Reply		*/
#define ICMP_INFO_REQUEST	15	/* Information Request		*/
#define ICMP_INFO_REPLY		16	/* Information Reply		*/
#define ICMP_ADDRESS		17	/* Address Mask Request		*/
#define ICMP_ADDRESSREPLY	18	/* Address Mask Reply		*/

/* Codes for UNREACH. */
#define ICMP_NET_UNREACH	0	/* Network Unreachable		*/
#define ICMP_HOST_UNREACH	1	/* Host Unreachable		*/
#define ICMP_PROT_UNREACH	2	/* Protocol Unreachable		*/
#define ICMP_PORT_UNREACH	3	/* Port Unreachable		*/
#define ICMP_FRAG_NEEDED	4	/* Fragmentation Needed/DF set	*/
#define ICMP_SR_FAILED		5	/* Source Route failed		*/
#define ICMP_NET_UNKNOWN	6
#define ICMP_HOST_UNKNOWN	7
#define ICMP_HOST_ISOLATED	8
#define ICMP_NET_ANO		9
#define ICMP_HOST_ANO		10
#define ICMP_NET_UNR_TOS	11
#define ICMP_HOST_UNR_TOS	12
#define ICMP_PKT_FILTERED	13	/* Packet filtered */
#define ICMP_PREC_VIOLATION	14	/* Precedence violation */
#define ICMP_PREC_CUTOFF	15	/* Precedence cut off */
#define NR_ICMP_UNREACH		15	/* instead of hardcoding immediate value */

/* Codes for REDIRECT. */
#define ICMP_REDIR_NET		0	/* Redirect Net			*/
#define ICMP_REDIR_HOST		1	/* Redirect Host		*/
#define ICMP_REDIR_NETTOS	2	/* Redirect Net for TOS		*/
#define ICMP_REDIR_HOSTTOS	3	/* Redirect Host for TOS	*/

/* Codes for TIME_EXCEEDED. */
#define ICMP_EXC_TTL		0	/* TTL count exceeded		*/
#define ICMP_EXC_FRAGTIME	1	/* Fragment Reass time exceeded	*/

struct __attribute__((packed)) icmp_hdr {
	uint8      type;
	uint8      code;
	uint16 checksum;
	union {
    struct {
      uint16	id;
      uint16	sequence;
    } echo;			/* echo datagram */
    uint32	gateway;	/* gateway address */
    struct
    {
      uint16	reserved;
      uint16	mtu;
    } frag;			/* path mtu discovery */
  } un;
};