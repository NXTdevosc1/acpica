#include <acpios.h>

void AcpiSubsystemNotifyHandler(ACPI_HANDLE Device, UINT32 Value, void* Context) {
    KDebugPrint("Notify Handler called");
}