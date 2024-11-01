#define MBUF_SIZE 2048

struct mbuf {
	unsigned char*             head;
	unsigned int                len;
	unsigned char buffer[MBUF_SIZE];
};