#include <acpios.h>

/*
Kernel functions
*/

void* LApicAddress = (void*)(UINT64)0xFEE00000;

NSTATUS KRNLAPI KiSetInterruptRouter(
    UINT Router, // 00 = PIC 01 = IOAPIC
    IR_SET_INTERRUPT SetInterrupt,
    IR_REMOVE_INTERRUPT RemoveInterrupt,
    IR_TERMINATE_ROUTER TerminateRouter,
    IR_GET_INTERRUPT_INFORMATION GetInterruptInformation,
    IR_END_OF_INTERRUPT Eoi
);

BOOLEAN _IoApicEnabled = FALSE;

extern ACPI_MADT_INTERRUPT_OVERRIDE* IntOverrides[];
extern UINT NumIntOverrides;



NSTATUS TestIrq(INTERRUPT_HANDLER_DATA* Interrupt) {
    KDebugPrint("IRQ Fired.");

    while(1) __halt();
}

void ApicEndOfInterrupt() {
    ApicWrite(0xB0, 0);
}


ACPI_STATUS AcpiInitializeApicConfiguration() {
    ACPI_TABLE_MADT* Madt;
    ACPI_STATUS Status = AcpiGetTable("APIC", 0, (ACPI_TABLE_HEADER**)&Madt);
    FAILED_RETURN(Status);

    KDebugPrint("APIC Found");

    

    LApicAddress = (void*)(UINT64)Madt->Address;

    char* endptr = (char*)Madt + Madt->Header.Length;

    char* ptr = (char*)(Madt + 1);
    ACPI_SUBTABLE_HEADER* Header;
    while(ptr < endptr) {
        Header = (ACPI_SUBTABLE_HEADER*)ptr;
        switch(Header->Type) {
            case ACPI_MADT_TYPE_LOCAL_APIC: {
                ACPI_MADT_LOCAL_APIC* lapic = (void*)ptr;
                
                KDebugPrint("lapic id %x flags %x pid %x", lapic->Id, lapic->LapicFlags, lapic->ProcessorId);
                break;
            }
            case ACPI_MADT_TYPE_IO_APIC: {
                ACPI_MADT_IO_APIC* ioapic = (void*)ptr;
                KDebugPrint("IO apic id %x addr %x irq base %x", ioapic->Id, ioapic->Address, ioapic->GlobalIrqBase);
                if(!_IoApicEnabled) {
                    if(KiSetInterruptRouter(
                        1,
                        IoApicSetInterrupt,
                        IoApicRemoveInterrupt,
                        IoApicTerminateRouter,
                        AcpiGetInterruptInformation,
                        ApicEndOfInterrupt
                    ) == STATUS_SUCCESS) {
                        _IoApicEnabled = TRUE;
                    }
                }
                IoApicAddRouter(ioapic);
                
                break;
            }
            case ACPI_MADT_TYPE_INTERRUPT_OVERRIDE: {
                ACPI_MADT_INTERRUPT_OVERRIDE* tbl = (void*)ptr;
                KDebugPrint("ACPI_MADT_TYPE_INTERRUPT_OVERRIDE Bus : %x GSI : %x SOURCE : %x",
                tbl->Bus, tbl->GlobalIrq, tbl->SourceIrq
                );
                IntOverrides[NumIntOverrides] = tbl;
                NumIntOverrides++;
                break;
            }
            case ACPI_MADT_TYPE_NMI_SOURCE: {
                KDebugPrint("ACPI_MADT_TYPE_NMI_SOURCE");
                break;
            }
            case ACPI_MADT_TYPE_LOCAL_APIC_NMI: {
                ACPI_MADT_LOCAL_APIC_NMI* nmi = (void*)ptr;
                KDebugPrint("ACPI_MADT_TYPE_LOCAL_APIC_NMI pid %x lint %x intiflags %x",
                nmi->ProcessorId, nmi->Lint, nmi->IntiFlags
                );
                break;
            }
            case ACPI_MADT_TYPE_LOCAL_APIC_OVERRIDE: {
                KDebugPrint("ACPI_MADT_TYPE_LOCAL_APIC_OVERRIDE");
                ACPI_MADT_LOCAL_APIC_OVERRIDE* tbl = (void*)ptr;
                LApicAddress = (void*)tbl->Address;
                break;
            }
            case ACPI_MADT_TYPE_IO_SAPIC: {
                KDebugPrint("ACPI_MADT_TYPE_IO_SAPIC");
                break;
            }
            case ACPI_MADT_TYPE_LOCAL_SAPIC: {
                KDebugPrint("ACPI_MADT_TYPE_LOCAL_SAPIC");
                break;
            }
            case ACPI_MADT_TYPE_INTERRUPT_SOURCE: {
                KDebugPrint("ACPI_MADT_TYPE_INTERRUPT_SOURCE");
                break;
            }
            case ACPI_MADT_TYPE_LOCAL_X2APIC: {
                KDebugPrint("ACPI_MADT_TYPE_LOCAL_X2APIC");
                break;
            }
            case ACPI_MADT_TYPE_LOCAL_X2APIC_NMI: {
                KDebugPrint("ACPI_MADT_TYPE_LOCAL_X2APIC_NMI");
                break;
            }
            case ACPI_MADT_TYPE_GENERIC_INTERRUPT: {
                KDebugPrint("ACPI_MADT_TYPE_GENERIC_INTERRUPT");
                break;
            }
        }
        ptr += Header->Length;
    }

    KeMapVirtualMemory(NULL, LApicAddress, LApicAddress, 1, PAGE_WRITE_ACCESS | PAGE_GLOBAL, 0);

    // Hardware Enable the APIC
    __writemsr(IA32_APIC_BASE_MSR, (UINT64)LApicAddress | IA32_APIC_BASE_MSR_ENABLE);
    // Set Spurious Interrupt Register
    ApicWrite(0xF0, ApicRead(0xF0) | 0x100);
    // Enable APIC Timer

    // NSTATUS s = ExInstallInterruptHandler(
    //     0, 0, TestIrq, NULL
    // );

    // KDebugPrint("ExInstallInt Status = %x", s);

    

    return AE_OK;
}

UINT32 ApicRead(UINT Offset) {
    return *(volatile UINT32*)((char*)LApicAddress + Offset);
}
void ApicWrite(UINT Offset, UINT32 Val) {
    *(volatile UINT32*)((char*)LApicAddress + Offset) = Val;
}