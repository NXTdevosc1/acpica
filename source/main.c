// For intellisense

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

ACPI_STATUS AcpiInitializationHandler(
    ACPI_HANDLE Object,
    UINT32 Function
) {
    KDebugPrint("Handler called");
    return AE_OK;
}


NSTATUS NOSAPI DriverEntry(
    void* Driver
) {
    KDebugPrint("Driver Startup.");
    ACPI_STATUS Status = AcpiInitializeSubsystem();
    if(Status != AE_OK) {
        KDebugPrint("ACPI Initialization failed, status : %.16x", Status);
        return Status;
    }

    KDebugPrint("__AcpiInitTables__");
    Status = AcpiInitializeTables(
        NULL, 0, 0
    );
    if(Status != AE_OK) return Status;

    KDebugPrint("Tables:");
    ACPI_TABLE_HEADER* Table;
    for(int i = 0;AcpiGetTableByIndex(i, &Table) == AE_OK;i++) {
        KDebugPrint("SN : %c%c%c%c Length : %d",
        Table->Signature[0], Table->Signature[1], Table->Signature[2], Table->Signature[3],
        Table->Length
        );
    }

    KDebugPrint("__AcpiInstallInitializationHandler__");
    return STATUS_SUCCESS;
    while(1);

    Status = AcpiInstallInitializationHandler(
        AcpiInitializationHandler,
        0
    );
    KDebugPrint("___ENABLING_ACPI_SUBSYSTEM___");
    Status = AcpiEnableSubsystem(ACPI_FULL_INITIALIZATION);
    return Status;
    // return STATUS_OK;
    // while(1) __halt();
    AcpiEnterSleepState(5);
    return STATUS_SUCCESS;
}