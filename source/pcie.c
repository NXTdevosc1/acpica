#include <acpios.h>
ACPI_TABLE_MCFG* Mcfg = NULL;
UINT NumPciSegments = 0;
ACPI_MCFG_ALLOCATION* McfgAllocationTable;

ACPI_MCFG_ALLOCATION* PciSegments[0x100] = {0};

BOOLEAN PciGetSegment(UINT8 PciSegment, ACPI_MCFG_ALLOCATION** Allocation) {
    if(!Mcfg) {
        if(PciSegment == 0) {
            *Allocation = NULL; // Use I/O Space
        } else return FALSE;
    } else {
        *Allocation = PciSegments[PciSegment];
        if(!(*Allocation)) return FALSE;
    }
    return TRUE;
}

ACPI_STATUS AcpiInitializePcieConfiguration() {
    ACPI_STATUS Status = AcpiGetTable("MCFG", 0, (ACPI_TABLE_HEADER**)&Mcfg);
    FAILED_RETURN(Status);
    KDebugPrint("MCFG Found");

    NumPciSegments = (Mcfg->Header.Length - sizeof(ACPI_TABLE_MCFG)) / sizeof(ACPI_MCFG_ALLOCATION);
    McfgAllocationTable = (ACPI_MCFG_ALLOCATION*)(Mcfg + 1);
    KDebugPrint("Num Segments : %d", NumPciSegments);
    for(UINT i = 0;i<NumPciSegments;i++) {
        PciSegments[McfgAllocationTable[i].PciSegment] = McfgAllocationTable + i;
        UINT64 Num2MBPages = ((McfgAllocationTable[i].EndBusNumber - McfgAllocationTable[i].StartBusNumber) + 1) << 20;
        Num2MBPages = AlignForward(Num2MBPages, 0x200000) >> 21;
        KeMapVirtualMemory(
            NULL,
            (void*)McfgAllocationTable[i].Address,
            (void*)McfgAllocationTable[i].Address,
            Num2MBPages,
            PAGE_2MB | PAGE_WRITE_ACCESS,
            PAGE_CACHE_DISABLE
        );
        KDebugPrint("PCI seg#%d SB : %d EB : %d ADDR : %x", McfgAllocationTable[i].PciSegment, McfgAllocationTable[i].StartBusNumber,McfgAllocationTable[i].EndBusNumber,McfgAllocationTable[i].Address);
    }
    ACPI_MCFG_ALLOCATION* t;
    KDebugPrint("Test get seg: S0 : %x S1 : %x", PciGetSegment(0, &t), PciGetSegment(1, &t));

    return STATUS_SUCCESS;
}