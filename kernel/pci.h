#ifndef _PCI_H_
#define _PCI_H

union __attribute__((packed)) pcie_config_hdr {
	struct __attribute__((packed)) type0 {
		uint32 vendor_id : 16;
		uint32 device_id : 16;
		uint32 cmd : 16;
		uint32 status : 16;
		uint32 rev_id : 8;
		uint32 class_code : 24;
		uint32 cache_lane_size : 8;
		uint32 master_latency_timer : 8;
		uint32 hdr_type : 8;
		uint32 bist : 8;
		uint32 base_addr_regs[6];
		uint32 cis_ptr : 32;
		uint32 subsys_vendor_id : 16;
		uint32 subsys_id : 16;
		uint32 expansion_rom_base_addr : 32;
		uint32 capabilities_ptr : 8;
		uint32 reserved0 : 24;
		uint32 reserved1 : 32;
		uint32 intr_line : 8;
		uint32 intr_pin : 8;
		uint32 min_gnt : 8;
		uint32 max_lat : 8;
	}t0;

	struct __attribute__((packed)) type1 {
		uint32 vendor_id : 16;
		uint32 device_id : 16;
		uint32 cmd : 16;
		uint32 status : 16;
		uint32 rev_id : 8;
		uint32 class_code : 24;
		uint32 cache_lane_size : 8;
		uint32 primary_latency_timer : 8;
		uint32 hdr_type : 8;
		uint32 bist : 8;
		uint32 base_addr_reg0 : 32;
		uint32 base_addr_reg1 : 32;
		uint32 primary_bus_num : 8;
		uint32 secondary_bus_num : 8;
		uint32 subordinate_bus_num : 8;
		uint32 secondary_latency_timer : 8;
		uint32 io_base : 8;
		uint32 io_limit : 8;
		uint32 secondary_status : 16;
		uint32 mem_base : 16;
		uint32 mem_limit : 16;
		uint32 prefetch_mem_base : 16;
		uint32 prefetch_mem_limit : 16;
		uint32 prefetch_base_upper : 32;
		uint32 prefetch_limit_upper : 32;
		uint32 io_base_upper : 16;
		uint32 io_limit_upper : 16;
		uint32 capabilities_ptr : 8;
		uint32 reserved0 : 24;
		uint32 expansion_rom_base : 32;
		uint32 intr_line : 8;
		uint32 intr_pin : 8;
		uint32 bridge_control : 16;
	}t1;
};

#endif // _PCI_H_