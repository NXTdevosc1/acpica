#include <acpios.h>
#include <AcpiSystem/acpiio.h>


BOOLEAN IOENTRY AcpiIoGetVersion(IOPARAMS) {
    UINT32* Version = Parameters[0];
    KDebugPrint("ACPI_IO_GET_VER %x", Version);
    *Version = 0x20000;
    return TRUE;
}

void _disable();
#pragma intrinsic(_disable)

void IOENTRY AcpiIoShutdown(IOPARAMS) {
    AcpiEnterSleepStatePrep(5);
    _disable();
    AcpiEnterSleepState(5);
    KeRaiseException(STATUS_SUBSYSTEM_NOT_PRESENT);
}

BOOLEAN AcpiIoGetTable(IOPARAMS) {
    char* Signature = Parameters[0];
    UINT32 Instance = UINT32VOID Parameters[1];
    ACPI_TABLE_HEADER** Header = Parameters[2];

    KDebugPrint("ACPI GET TABLE SIG : %s INST : %d HDR : %x", Signature, Instance, Header);

    if((AcpiGetTable(Signature, Instance, Header) != AE_OK)) return FALSE;

    return TRUE;
}

BOOLEAN AcpiIoGetTableByIndex(IOPARAMS) {
    UINT32 TableIndex = UINT32VOID Parameters[0];
    ACPI_TABLE_HEADER** Hdr = Parameters[1];
    if((AcpiGetTableByIndex(TableIndex, Hdr)) != AE_OK) return FALSE;
    return TRUE;
}

struct {
    void* IoProc;
    UINT32 NumParameters;
} AcpiFunctions[] = {
    {AcpiIoGetVersion, 1}, {AcpiIoShutdown, 0},
    {AcpiIoGetTable, 3}, {AcpiIoGetTableByIndex, 2}
};

IORESULT IOENTRY AcpiIoCallback(
    IOPARAMS
) {
    if(AcpiFunctions[Function].NumParameters != NumParameters) KeRaiseException(STATUS_INVALID_PARAMETER);
    KDebugPrint("ACPI IO Protocol %d", Function);
    IORESULT Result = ((DEVICE_IO_CALLBACK)AcpiFunctions[Function].IoProc)(DeviceHandle, Function, NumParameters, Parameters);
    return Result;
}

