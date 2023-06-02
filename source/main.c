// For intellisense



#include <acpios.h>

ACPI_STATUS AcpiInitializationHandler(
    ACPI_HANDLE Object,
    UINT32 Function
) {
    KDebugPrint("Handler called");
    return AE_OK;
}

ACPI_STATUS WalkCallback(ACPI_HANDLE Object, UINT32 NestingLevel, void *Context, void **ReturnValue) {
    ACPI_BUFFER bf;
    ACPI_DEVICE_INFO* Dev = AcpiOsAllocate(sizeof(ACPI_DEVICE_INFO));
    AcpiGetObjectInfo(Object, &Dev);
    KDebugPrint("ACPI WALK CALLBACK HID : %s Name : %c%c%c%c", Dev->UniqueId.String, Dev->Name, Dev->Name >> 8, Dev->Name >> 16, Dev->Name >> 24);
    // AcpiGetIrqRoutingTable(Object, &bf);
    // KDebugPrint("ACPI WALK CALLBACK %x %x", bf.Length, bf.Pointer);
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

    // Initialize Required Kernel Tables
    AcpiInitializeApicConfiguration();
    AcpiInitializeHpetTable();
    AcpiInitializePcieConfiguration();


    KDebugPrint("__AcpiInstallInitializationHandler__");
    // return STATUS_SUCCESS;

    // Status = AcpiInstallInitializationHandler(
    //     AcpiInitializationHandler,
    //     0
    // );
    KDebugPrint("___ENABLING_ACPI_SUBSYSTEM___");
    Status = AcpiEnableSubsystem(ACPI_FULL_INITIALIZATION);
    if(Status != AE_OK) return Status;

    KDebugPrint("__ACPI_GET_DEVICES__");

    Status = AcpiGetDevices(NULL, WalkCallback, NULL, NULL);
    return Status;
    // return STATUS_OK;
    // while(1) __halt();
    AcpiEnterSleepState(5);
    return STATUS_SUCCESS;
}