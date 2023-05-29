#include <acpios.h>

ACPI_MADT_IO_APIC* Routers[0x100] = {0};
UINT NumRouters = 0;

UINT NumIntOverrides = 0;
ACPI_MADT_INTERRUPT_OVERRIDE* IntOverrides[0x100] = {0};

#define INTVOID (void*)(UINT64)

// IOAPIC Redirection Table
typedef union _IOAPIC_REDTBL {
    struct {
        QWORD InterruptVector : 8;
        QWORD DeliveryMode : 3; // 0 = fixed, 1 = lowest priority, 2 = SMI, 4 = NMI, 5 = INIT, 7 = ExtINT
        QWORD DestinationMode : 1;
        QWORD DeliveryStatus : 1;
        QWORD InputPinPolarity : 1; // 0 = high active, 1 = low active
        QWORD RemoteIRR : 1;
        QWORD TriggerMode : 1; // 1 = level sensitive, 0 = edge sensitive
        QWORD InterruptMask : 1; // when set the interrupt signal is masked (disabled)
        QWORD Reserved : 39;
        QWORD Destination : 8;
    } Fields;
    struct {
        DWORD Low;
        DWORD High;
    } Value;
} IOAPIC_REDTBL;

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
    UINT MaxRedirEntry = (IoApicRead(IoApic, 1) >> 16) & 0xFF;
    KDebugPrint("Max Redir Entry %d", MaxRedirEntry);

    for(UINT i = IoApic->GlobalIrqBase;i<=IoApic->GlobalIrqBase + MaxRedirEntry;i++) {
        IoApicRemoveInterrupt(i, IM_NO_OVERRIDE);
    }

    return STATUS_SUCCESS;
}
NSTATUS IoApicSetInterrupt(
    UINT Irq,
    UINT Flags, // Call flags
    UINT InterruptVector,
    UINT64 ProcessorId,
    UINT DeliveryMode,
    BOOLEAN Polarity,
    BOOLEAN TriggerMode
) {
    if(!(Flags & IM_NO_OVERRIDE)) {
        // Check if IRQ Has an override
        for(UINT i = 0;i<NumIntOverrides;i++) {
            if(IntOverrides[i]->SourceIrq == Irq) {
                KDebugPrint("IOApic SetInt (Override SRC : %d GSI : %d)", Irq, IntOverrides[i]->GlobalIrq);
                Irq = IntOverrides[i]->GlobalIrq;
                break;
            }
        }
    }
    // Choose which IOAPIC to use
    ACPI_MADT_IO_APIC* Router = NULL;
    for(UINT i = 0;i<0x100;i++) {
        if(!Routers[i]) continue;
        if(Irq >= Routers[i]->GlobalIrqBase &&
        Irq <= ((IoApicRead(Routers[i], 1) >> 16) & 0xFF)
        ) {
            Router = Routers[i];
            break;
        }
    }
    if(!Router) return STATUS_NOT_FOUND;

    KDebugPrint("IOApic SetInt (Router : %d)", Router->Id);
    
    // Set Redirection Entry
    UINT Entry = Irq - Router->GlobalIrqBase;
    IOAPIC_REDTBL RedirectionEntry = {0};

    RedirectionEntry.Fields.InterruptVector = InterruptVector;
    RedirectionEntry.Fields.DeliveryMode = DeliveryMode;
    RedirectionEntry.Fields.InputPinPolarity = Polarity;
    RedirectionEntry.Fields.TriggerMode = TriggerMode;
    RedirectionEntry.Fields.Destination = ProcessorId;

    IoApicWrite(Router, 0x10 + Entry * 2, RedirectionEntry.Value.Low);
    IoApicWrite(Router, 0x11 + Entry * 2, RedirectionEntry.Value.High);

    return STATUS_SUCCESS;
}



NSTATUS IoApicRemoveInterrupt(
    UINT Irq,
    UINT Flags
) {
    if(!(Flags & IM_NO_OVERRIDE)) {
        // Check if IRQ Has an override
        for(UINT i = 0;i<NumIntOverrides;i++) {
            if(IntOverrides[i]->SourceIrq == Irq) {
                KDebugPrint("IOApic SetInt (Override SRC : %d GSI : %d)", Irq, IntOverrides[i]->GlobalIrq);
                Irq = IntOverrides[i]->GlobalIrq;
                break;
            }
        }
    }
    // Choose which IOAPIC to use
    ACPI_MADT_IO_APIC* Router = NULL;
    for(UINT i = 0;i<0x100;i++) {
        if(!Routers[i]) continue;
        if(Irq >= Routers[i]->GlobalIrqBase &&
        Irq <= ((IoApicRead(Routers[i], 1) >> 16) & 0xFF)
        ) {
            Router = Routers[i];
            break;
        }
    }
    if(!Router) return STATUS_NOT_FOUND;

    // Set Redirection Entry
    UINT Entry = Irq - Router->GlobalIrqBase;
    IOAPIC_REDTBL RedirectionEntry = {0};
    RedirectionEntry.Fields.InterruptMask = 1;
    IoApicWrite(Router, 0x10 + Entry * 2, RedirectionEntry.Value.Low);
    IoApicWrite(Router, 0x11 + Entry * 2, RedirectionEntry.Value.High);
    return STATUS_SUCCESS;
}
NSTATUS IoApicTerminateRouter() {
    return STATUS_SUCCESS;
}

BOOLEAN AcpiGetInterruptInformation(UINT Irq, PIM_INTERRUPT_INFORMATION InterruptInformation) {
    ObjZeroMemory(InterruptInformation);
    // If it stays set to 0xFF then no override was found
    InterruptInformation->Fields.GlobalSystemInterrupt = 0xFF;

    // Check override entries
    for(UINT i = 0;i<NumIntOverrides;i++) {
        if(IntOverrides[i]->SourceIrq == Irq) {
            UINT f = IntOverrides[i]->IntiFlags;
            if(f & 2) {
                // Active Low
                InterruptInformation->Fields.Polarity = 1;
            }
            if(f & 8) {
                // Level sensitive
                InterruptInformation->Fields.TriggerMode = 1;
            }
            // TODO : Read Delivery Mode (SMI, NMI...)
            // Default is : Fixed
            InterruptInformation->Fields.DeliveryMode = 0; // FIXED
            InterruptInformation->Fields.GlobalSystemInterrupt = IntOverrides[i]->GlobalIrq;
            InterruptInformation->Fields.Bus = IntOverrides[i]->Bus;
        }
    }
    
    // No entry was found
    if(InterruptInformation->Fields.GlobalSystemInterrupt == 0xFF) return FALSE;

    
    return TRUE;
}