#define E1000_PCIE_CMD_IO_ENABLE            0x0001
#define E1000_PCIE_CMD_MMIO_ENABLE          0x0002
#define E1000_PCIE_CMD_BUS_MASTERING_ENABLE 0x0004

#define E1000_PCIE_BAR0_64BIT 0x00000004

#define E1000_CTRL     (0x00000/4)  /* Device Control Register - RW */
#define E1000_ICR      (0x000C0/4)  /* Interrupt Cause Read - R */
#define E1000_IMS      (0x000D0/4)  /* Interrupt Mask Set - RW */
#define E1000_RCTL     (0x00100/4)  /* RX Control - RW */
#define E1000_TCTL     (0x00400/4)  /* TX Control - RW */
#define E1000_TIPG     (0x00410/4)  /* TX Inter-packet gap -RW */
#define E1000_RDBAL    (0x02800/4)  /* RX Descriptor Base Address Low - RW */
#define E1000_RDTR     (0x02820/4)  /* RX Delay Timer */
#define E1000_RADV     (0x0282C/4)  /* RX Interrupt Absolute Delay Timer */
#define E1000_RDH      (0x02810/4)  /* RX Descriptor Head - RW */
#define E1000_RDT      (0x02818/4)  /* RX Descriptor Tail - RW */
#define E1000_RDLEN    (0x02808/4)  /* RX Descriptor Length - RW */
#define E1000_RSRPD    (0x02C00/4)  /* RX Small Packet Detect Interrupt */
#define E1000_TDBAL    (0x03800/4)  /* TX Descriptor Base Address Low - RW */
#define E1000_TDLEN    (0x03808/4)  /* TX Descriptor Length - RW */
#define E1000_TDH      (0x03810/4)  /* TX Descriptor Head - RW */
#define E1000_TDT      (0x03818/4)  /* TX Descripotr Tail - RW */
#define E1000_MTA      (0x05200/4)  /* Multicast Table Array - RW Array */
#define E1000_RA       (0x05400/4)  /* Receive Address - RW Array */

// E1000 Control Register
union __attribute__((packed)) e1000_reg {
	struct __attribute__((packed)) {
		uint32 fd : 1;
		uint32 reserved0 : 2;
		uint32 lrst : 1;
		uint32 reserved1 : 1;
		uint32 asde : 1;
		uint32 slu : 1;
		uint32 ilos : 1;
		uint32 speed : 2;
		uint32 reserved2 : 1;
		uint32 frcspd : 1;
		uint32 frcdplx : 1;
		uint32 reserved3 : 5;
		uint32 sdp0_data : 1;
		uint32 sdp1_data : 1;
		uint32 advd3wuc : 1;
		uint32 en_phy_pwr_mgmt : 1;
		uint32 sdp0_iodir : 1;
		uint32 sdp1_iodir : 1;
		uint32 reserved4 : 2;
		uint32 rst : 1;
		uint32 rfce : 1;
		uint32 tfce : 1;
		uint32 reserved5 : 1;
		uint32 vme : 1;
		uint32 phy_rst : 1;
	} ctrl;
};
struct __attribute__((packed)) tx_desc {
	uint64    addr;
	uint16  length;
	uint8      cso;
	uint8      cmd;
	uint8   status;
	uint8 reserved;
	uint8      css;
	uint16 special;
};

struct __attribute__((packed)) rx_desc {
	uint64     addr;
	uint16   length;
	uint16 checksum;
	uint8    status;
	uint8    errors;
	uint16  special;
};

extern struct spinlock e1000_lock;