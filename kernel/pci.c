#include <assert.h>

#include "types.h"
#include "riscv.h"
#include "memlayout.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"

#include "pci.h"

void
pcie_init() {
	assert(sizeof(union pcie_config_hdr) == 64);	
	volatile union pcie_config_hdr* dev = (void*)PCIE_ECAM_BASE;
	assert(sizeof(dev->t0) == sizeof(dev->t1));
	while ((uint64)dev < PCIE_ECAM_BASE + PCIE_ECAM_SIZE) {
		if(dev->t0.vendor_id == 0xffff) {
			dev++;
			continue;
		}
		printf(
			"[%ld:%ld:%ld] Vendor: %x, Device: %x\n", 
			PCIE_BUS((uint64)dev),
			PCIE_DEVICE((uint64)dev),
			PCIE_FUNC((uint64)dev),
			dev->t0.vendor_id,
			dev->t0.device_id
			);
		if(dev->t0.vendor_id == 0x8086 && dev->t0.device_id == 0x100e) {
			e1000_init(dev);
		}
		dev = (void*)dev + PCIE_CONFIG_SPACE_SIZE;
	}
}