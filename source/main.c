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
    ACPI_DEVICE_INFO* Dev = AcpiOsAllocate(0x1000);
    AcpiGetObjectInfo(Object, &Dev);
    KDebugPrint("ACPI WALK CALLBACK NL : %d HID : %s Name : %c%c%c%c",NestingLevel, Dev->UniqueId.String, Dev->Name, Dev->Name >> 8, Dev->Name >> 16, Dev->Name >> 24);
    // AcpiGetIrqRoutingTable(Object, &bf);
    // KDebugPrint("ACPI WALK CALLBACK %x %x", bf.Length, bf.Pointer);
    return AE_OK;
}

void _disable();
#pragma intrinsic(_disable)


/*

 * This function should only be called once,
    when the ACPI Driver initializes the interrupting system in the kernel
    and initializes the APIC and HPET along with other essential tables

    This is the last step of kernel initialization, this function finally enables interrupts
    and sets up the task scheduling system
*/
void KRNLAPI KeSchedulingSystemInit();

/*-------------------------------------------------------*/

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


    // Initialize Required Kernel Tables
    if((Status = AcpiInitializeApicConfiguration()) != AE_OK) return Status;
    if((Status = AcpiInitializePcieConfiguration()) != AE_OK) return Status;

    // *** From here:
    if((Status = AcpiHpetInit()) != AE_OK) return Status;


    KDebugPrint("__AcpiLoadTables__");
    Status = AcpiLoadTables();
    if(Status != AE_OK) return Status;

    // *** To here should ideally take less than 1 second

    KDebugPrint("Completing secondary ACPI Tables initialization");
    // *** Now we will set IOAPIC Mode and enable interrupts in the kernel
    AcpiSubsystemSetIoApicMode();

    // Now we will execute the last step of kernel initialization
    KeSchedulingSystemInit();


    KDebugPrint("Tables:");
    ACPI_TABLE_HEADER* Table;
    for(int i = 0;AcpiGetTableByIndex(i, &Table) == AE_OK;i++) {
        KDebugPrint("SN : %c%c%c%c Length : %d",
        Table->Signature[0], Table->Signature[1], Table->Signature[2], Table->Signature[3],
        Table->Length
        );
    }

    KDebugPrint("__AcpiInstallNotifyHandler__");
    
    Status = AcpiInstallNotifyHandler(ACPI_ROOT_OBJECT, ACPI_SYSTEM_NOTIFY, AcpiSubsystemNotifyHandler, NULL);
    if(Status != AE_OK) return Status;

    KDebugPrint("___ENABLING_ACPI_SUBSYSTEM___");
    Status = AcpiEnableSubsystem(ACPI_FULL_INITIALIZATION);
    if(Status != AE_OK) return Status;

    KDebugPrint("__ACPI_INIT_OBJECTS__");
    Status = AcpiInitializeObjects(ACPI_FULL_INITIALIZATION);
    if(Status != AE_OK) return Status;


    KDebugPrint("__ACPI_GET_DEVICES__");

    


    Status = AcpiGetDevices("PCI0", WalkCallback, NULL, NULL);
    // AcpiEnterSleepStatePrep(5);
    // _disable();
    // KDebugPrint("Shutting Down...");
    // AcpiEnterSleepState(5);
    // KDebugPrint("Shutdown Done!");
    return Status;
    // return STATUS_OK;
    // while(1) __halt();
    return STATUS_SUCCESS;
}