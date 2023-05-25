#include <acpios.h>

ACPI_MADT_IO_APIC* Routers[0x100];
UINT NumRouters = 0;

#define INTVOID (void*)(UINT64)

UINT32 IoApicRead(
    ACPI_MADT_IO_APIC* IoApic,
    UINT32 Offset
) {
    *(volatile UINT32*)((UINT64)IoApic->Address) = Offset;
    return *(volatile UINT32*)((UINT64)IoApic->Address + 0x10);
}

void IoApicWrite(
    ACPI_MADT_IO_APIC* IoApic,
    UINT32 Offset,
    UINT32 Value
) {
    *(volatile UINT32*)((UINT64)IoApic->Address) = Offset;
    *(volatile UINT32*)((UINT64)IoApic->Address + 0x10) = Value;
}

NSTATUS IoApicAddRouter(ACPI_MADT_IO_APIC* IoApic) {
    Routers[IoApic->Id] = IoApic;
    NumRouters++;
    KeMapVirtualMemory(
        NULL,
        INTVOID IoApic->Address,
        INTVOID IoApic->Address,
        1,
        PAGE_WRITE_ACCESS,
        0
    );
    KDebugPrint("Max Redir Entry %d", (IoApicRead(IoApic, 1) >> 16) & 0xFF);
    return STATUS_SUCCESS;
}
NSTATUS IoApicSetInterrupt(
    UINT Irq,
    UINT ProcessorInterruptNumber,
    UINT64 ProcessorId
) {
    return STATUS_SUCCESS;
}
NSTATUS IoApicRemoveInterrupt(
    UINT Irq
) {
    return STATUS_SUCCESS;
}
NSTATUS IoApicTerminateRouter() {
    return STATUS_SUCCESS;
}