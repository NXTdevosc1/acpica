#include <ddk.h>

NSTATUS NOSAPI DriverEntry(
    void* Driver
) {
    KDebugPrint("Driver Startup.");
    return STATUS_SUCCESS;
}