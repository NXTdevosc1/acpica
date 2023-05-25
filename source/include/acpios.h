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

#define FAILED_RETURN(s) if(s != AE_OK) return s;

ACPI_STATUS AcpiInitializeHpetTable();
ACPI_STATUS AcpiInitializeApicConfiguration();
ACPI_STATUS AcpiInitializePcieConfiguration();



NSTATUS IoApicAddRouter(ACPI_MADT_IO_APIC* IoApic);
NSTATUS IoApicSetInterrupt(
    UINT Irq,
    UINT ProcessorInterruptNumber,
    UINT64 ProcessorId
);
NSTATUS IoApicRemoveInterrupt(
    UINT Irq
);
NSTATUS IoApicTerminateRouter();