#pragma once
#include <ddk.h>
#ifndef WIN64
#define WIN64
#endif

#ifndef _MSVC_VER
#define _MSVC_VER 1201
#endif
#undef ACPI_DEBUG_OUTPUT
#include <acpi.h>
#ifndef ACPI_INLINE
#define ACPI_INLINE inline
#endif
#define ACPI_INIT_FUNCTION
#ifndef ACPI_MACHINE_WIDTH
#define ACPI_MACHINE_WIDTH 64
#endif
#include <acpixf.h>
#include <intrin.h>
#include <intmgr.h>

#define FAILED_RETURN(s) if(s != AE_OK) return s;

ACPI_STATUS AcpiHpetInit();
ACPI_STATUS AcpiInitializeApicConfiguration();
ACPI_STATUS AcpiInitializePcieConfiguration();

#define IA32_APIC_BASE_MSR 0x1B
#define IA32_APIC_BASE_MSR_ENABLE 0x800


NSTATUS IoApicAddRouter(ACPI_MADT_IO_APIC* IoApic);
NSTATUS IoApicSetInterrupt(
    UINT Irq,
    UINT Flags,
    UINT InterruptVector,
    UINT64 ProcessorId,
    UINT DeliveryMode,
    BOOLEAN Polarity,
    BOOLEAN TriggerMode
);
NSTATUS IoApicRemoveInterrupt(
    UINT Irq,
    UINT Flags
);
NSTATUS IoApicTerminateRouter();

BOOLEAN AcpiGetInterruptInformation(UINT Irq, PIM_INTERRUPT_INFORMATION InterruptInformation);

UINT32 ApicRead(UINT Offset);
void ApicWrite(UINT Offset, UINT32 Val);

// PCI
// if Allocation == NULL and returned value is TRUE
// Then use the I/O PCI Bus
BOOLEAN PciGetSegment(UINT8 PciSegment, ACPI_MCFG_ALLOCATION** Allocation);


// Either succeeds or set a screen of death
void AcpiSubsystemSetIoApicMode();

void AcpiSubsystemNotifyHandler(ACPI_HANDLE Device, UINT32 Value, void* Context);


IORESULT __cdecl AcpiIoCallback(
    IOPARAMS
);