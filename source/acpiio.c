#include <acpios.h>
#include <AcpiSystem/acpiio.h>


BOOLEAN IOENTRY AcpiIoGetVersion(IOPARAMS) {
    UINT32* Version = Parameters[0];
    if(NumParameters != 1) return FALSE;
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

DEVICE_IO_CALLBACK AcpiFunctions[] = {
    (DEVICE_IO_CALLBACK)AcpiIoGetVersion,
    (DEVICE_IO_CALLBACK)AcpiIoShutdown
};

IORESULT IOENTRY AcpiIoCallback(
    IOPARAMS
) {
    IORESULT Result = AcpiFunctions[Function](DeviceHandle, Function, NumParameters, Parameters);
    KDebugPrint("DONE: ACPI IO CALLBACK Function %d", Function);
    return Result;
}

