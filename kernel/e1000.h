#define E1000_PCIE_CMD_IO_ENABLE            0x0001
#define E1000_PCIE_CMD_MMIO_ENABLE          0x0002
#define E1000_PCIE_CMD_BUS_MASTERING_ENABLE 0x0004

#define E1000_PCIE_BAR0_64BIT 0x00000004

#define E1000_CTRL             (0x00000/4)  /* Device Control Register - RW */
#define E1000_ICR              (0x000C0/4)  /* Interrupt Cause Read - R */
#define E1000_IMS              (0x000D0/4)  /* Interrupt Mask Set - RW */
#define E1000_IMC              (0x000D8/4)  /* Interrupt Mask Clear - W */
#define E1000_RCTL             (0x00100/4)  /* RX Control - RW */
#define E1000_TCTL             (0x00400/4)  /* TX Control - RW */
#define E1000_TIPG             (0x00410/4)  /* TX Inter-packet gap -RW */
#define E1000_RDBA             (0x02800/4)  /* RX Descriptor Base Address - RW */
#define E1000_RDTR             (0x02820/4)  /* RX Delay Timer */
#define E1000_RADV             (0x0282C/4)  /* RX Interrupt Absolute Delay Timer */
#define E1000_RDH              (0x02810/4)  /* RX Descriptor Head - RW */
#define E1000_RDT              (0x02818/4)  /* RX Descriptor Tail - RW */
#define E1000_RDLEN            (0x02808/4)  /* RX Descriptor Length - RW */
#define E1000_RSRPD            (0x02C00/4)  /* RX Small Packet Detect Interrupt */
#define E1000_TDBAL            (0x03800/4)  /* TX Descriptor Base Address Low - RW */
#define E1000_TDBAH            (0x03804/4)  /* TX Descriptor Base Address High - RW */
#define E1000_TDLEN            (0x03808/4)  /* TX Descriptor Length - RW */
#define E1000_TDH              (0x03810/4)  /* TX Descriptor Head - RW */
#define E1000_TDT              (0x03818/4)  /* TX Descripotr Tail - RW */
#define E1000_MTA              (0x05200/4)  /* Multicast Table Array - RW Array */
#define E1000_RA(n)    (0x05400/4 + 2*(n))  /* Receive Address - RW Array */

// E1000 Control Register
#define E1000_CTRL_ASDE 0x00000020 // Auto-Speed Detection Enable
#define E1000_CTRL_SLU  0x00000040 // Set Link Up
#define E1000_CTRL_RST  0x04000000 // Device Reset

// E1000 Interrupt Bits
#define E1000_INT_TXDW    0x00000001 /* Transmit Descriptor Written Back */
#define E1000_INT_TXQE    0x00000002 // Transmit Queue Empty
#define E1000_INT_LSC     0x00000004 // Link Status Change
#define E1000_INT_RXSEQ   0x00000008 // Receive Sequence Error
#define E1000_INT_RXDMT0  0x00000010 // Receive Minimum Threshold Hit
#define E1000_INT_RXO     0x00000040 // Receiver FIFO Overrun
#define E1000_INT_RXT0    0x00000080 // Receiver Timer Interrupt
#define E1000_INT_MDAC    0x00000200 // MDI/O Access Complete Register
#define E1000_INT_RXCFG   0x00000400 // Receiving /C/ Ordered Sets
#define E1000_INT_PHYINT  0x00001000 // PHY Interrupt
#define E1000_INT_GPI0    0x00002000 // General Purpose Interrupt 0
#define E1000_INT_GPI1    0x00004000 // General Purpose Interrupt 1
#define E1000_INT_TXD_LOW 0x00008000 // Transmit Descriptor Low Threshold Hit
#define E1000_INT_SRPD    0x00010000 // Small Receive Packet Detect


// E1000 Transmit Control Register
#define E1000_TCTL_EN         0x00000002 // Transmit Enable
#define E1000_TCTL_PSP        0x00000008 // Pad Short Packets
#define E1000_TCTL_CT_VAL           0x10 // Collision Threshold
#define E1000_TCTL_CT_SHIFT            4
#define E1000_TCTL_COLD_VAL         0x40 // Collision Distance
#define E1000_TCTL_COLD_SHIFT         12

// E1000 Transmit Inter Packet Gap
#define E1000_TIPG_IPGT_VAL    0x0a // IPG Transmit Time
#define E1000_TIPG_IPGT_SHIFT     0
#define E1000_TIPG_IPGR1_VAL   0x0a // IPG Receive Time 1
#define E1000_TIPG_IPGR1_SHIFT   10
#define E1000_TIPG_IPGR2_VAL   0x0a // IPG Receive Time 2
#define E1000_TIPG_IPGR2_SHIFT   20

// Transmit Descriptor
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
	uint64     addr;      // Address
	uint16   length;      // Length
	uint8       cso;      // Checksum offset
	uint8       cmd;      // Command
	uint8    status;      // Status
	uint8       css;      // Checksum Start
	uint16  special;      // Special
};

// E1000 Receive Control Register
#define E1000_RCTL_EN    0x00000002 // Receiver Enable
#define E1000_RCTL_BAM   0x00008000 // Broadcast Accept Mode
#define E1000_BSIZE_2048 0x00000000 // Receive Buffer Size
#define E1000_RCTL_SECRC 0x04000000 // Stript Ethernet CRC from incoming packets

// Receive Descriptor
#define RX_DESC_STATUS_PIF   0x80 // Passed in-exact filter
#define RX_DESC_STATUS_IPCS  0x40 // IP Checksum Calculated on Packet
#define RX_DESC_STATUS_TCPCS 0x20 // TCP Checksum Calculated on Packet
#define RX_DESC_STATUS_VP    0x08 // Packet is 802.1Q
#define RX_DESC_STATUS_IXSM  0x04 // Ignore Checksum Indication
#define RX_DESC_STATUS_EOP   0x02 // End of Packet
#define RX_DESC_STATUS_DD    0x01 // Descriptor Done
struct __attribute__((packed)) rx_desc {
	uint64     addr;
	uint16   length;
	uint16 checksum;
	uint8    status;
	uint8    errors;
	uint16  special;
};

#define E1000_RA_MAC(val, pos) ((val >> (pos * 8)) & 0xff)
#define E1000_RA_AS_MASK       0x0003000000000000UL
#define E1000_RA_AS_SHIFT                      48
#define E1000_RA_AV            0x8000000000000000UL
struct __attribute__((packed)) e1000_ra {
	uint8        mac[6];   // MAC address
	uint8         as :  2; // Address Select
	uint16 reserved0 : 13; // Reserved, write to 0
	uint8         av :  1; // Address Valid
};

extern struct spinlock e1000_lock;