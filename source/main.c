// For intellisense
#ifndef WIN64
#define WIN64
#define _MSVC_VER
#endif
#include <ddk.h>
#include <acpi.h>
#include <acpixf.h>
NSTATUS NOSAPI DriverEntry(
    void* Driver
) {
    KDebugPrint("Driver Startup.");
    
    AcpiEnterSleepState(5);
    return STATUS_SUCCESS;
}