#include <acpios.h>

/*
Kernel functions
*/

typedef NSTATUS(__cdecl *IR_SET_INTERRUPT)(
    UINT Irq,
    UINT ProcessorInterruptNumber,
    UINT64 ProcessorId
);

typedef NSTATUS(__cdecl *IR_REMOVE_INTERRUPT)(UINT Irq);

// Never called
typedef NSTATUS(__cdecl *IR_TERMINATE_ROUTER)();

NSTATUS KRNLAPI KiSetInterruptRouter(
    UINT Router, // 00 = PIC 01 = IOAPIC
    IR_SET_INTERRUPT SetInterrupt,
    IR_REMOVE_INTERRUPT RemoveInterrupt,
    IR_TERMINATE_ROUTER TerminateRouter
);

BOOLEAN _IoApicEnabled = FALSE;



ACPI_STATUS AcpiInitializeApicConfiguration() {
    ACPI_TABLE_MADT* Madt;
    ACPI_STATUS Status = AcpiGetTable("APIC", 0, (ACPI_TABLE_HEADER**)&Madt);
    FAILED_RETURN(Status);

    KDebugPrint("APIC Found");

    

    void* LApicAddress = (void*)(UINT64)Madt->Address;

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
                        IoApicTerminateRouter
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

    return AE_OK;
}