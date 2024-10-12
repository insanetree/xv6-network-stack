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
#define E1000_TDBAH    (0x03804/4)  /* TX Descriptor Base Address High - RW */
#define E1000_TDLEN    (0x03808/4)  /* TX Descriptor Length - RW */
#define E1000_TDH      (0x03810/4)  /* TX Descriptor Head - RW */
#define E1000_TDT      (0x03818/4)  /* TX Descripotr Tail - RW */
#define E1000_MTA      (0x05200/4)  /* Multicast Table Array - RW Array */
#define E1000_RA       (0x05400/4)  /* Receive Address - RW Array */

// E1000 Control Register
struct __attribute__((packed)) e1000_ctrl {
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
};

// E1000 Transmit Control Register
struct __attribute__((packed)) e1000_tctl {
	uint32 reserved0 : 1;
	uint32 en : 1;
	uint32 reserved1 : 1;
	uint32 psp : 1;
	uint32 ct : 8;
	uint32 cold : 10;
	uint32 swxoff : 1;
	uint32 reserved2 : 1;
	uint32 rtcl : 1;
	uint32 nrtu : 1;
	uint32 reserved3 : 6;
};

#define TX_DESC_CMD_EOP  0x01 /* End Of Packet */
#define TX_DESC_CMD_IFCS 0x02 /* Insert FCS*/
#define TX_DESC_CMD_IC   0x04 /* Insert Checksum */
#define TX_DESC_CMD_RS   0x08 /* Report Status */
#define TX_DESC_CMD_RPS  0x10 /* Report Packet Sent */
#define TX_DESC_CMD_DEXT 0x20 /* Extension (set to 0) */
#define TX_DESC_CMD_VLE  0x40 /* VLAN Packet Enable */
#define TX_DESC_CMD_IDE  0x80 /* Interrupt Delay Enable */

#define TX_DESC_STATUS_DD 0x1 /* Descriptor Done */
#define TX_DESC_STATUS_EX 0x2 /* Excess Collision */
#define TX_DESC_STATUS_LC 0x4 /* Late Collision */
struct __attribute__((packed)) tx_desc {
	uint64    addr;      // Address
	uint16  length;      // Length
	uint8      cso;      // Checksum offset
	uint8      cmd;      // Command
	uint8   status : 4;  // Status
	uint8 reserved0 : 4; // Reserved, write to 0
	uint8      css;      // Checksum Start
	uint16 special;      // Special
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