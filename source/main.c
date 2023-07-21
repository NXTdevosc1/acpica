// For intellisense



#include <acpios.h>
#include <AcpiSystem/acpiio.h>


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

PDEVICE AcpiDevice = NULL;

void KRNLAPI KiDrawDebugRect(UINT8 DbgStage);

NSTATUS NOSAPI DriverEntry(
    void* Driver
) {
    KDebugPrint("Driver Startup.");
    KiDrawDebugRect(0);
    ACPI_STATUS Status = AcpiInitializeSubsystem();
    if(Status != AE_OK) {
        KDebugPrint("ACPI Initialization failed, status : %.16x", Status);
        return Status;
    }
    KiDrawDebugRect(1);

    KDebugPrint("__AcpiInitTables__");
    Status = AcpiInitializeTables(
        NULL, 0, 0
    );
    if(Status != AE_OK) return Status;

    KiDrawDebugRect(2);

    // Initialize Required Kernel Tables
    if((Status = AcpiInitializeApicConfiguration()) != AE_OK) return Status;
    KiDrawDebugRect(3);
    if((Status = AcpiInitializePcieConfiguration()) != AE_OK) return Status;
    KiDrawDebugRect(4);

    // *** From here:
    if((Status = AcpiHpetInit()) != AE_OK) return Status;

    KiDrawDebugRect(5);

    KDebugPrint("__AcpiLoadTables__");
    Status = AcpiLoadTables();
    if(Status != AE_OK) return Status;
    KiDrawDebugRect(6);


    // *** To here should ideally take less than 1 second

    KDebugPrint("Completing secondary ACPI Tables initialization");
    // *** Now we will set IOAPIC Mode and enable interrupts in the kernel
    AcpiSubsystemSetIoApicMode();
    KiDrawDebugRect(7);

    // Now we will execute the last step of kernel initialization
    KeSchedulingSystemInit();

    KiDrawDebugRect(8);

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


    KDebugPrint("Creating ACPI Device Object");
    AcpiDevice = KeCreateDevice(
        DEVICE_COMPUTER_MANAGEMENT,
        0,
        L"Advanced Configuration And Power Interface 2.0+",
        NULL
    );

    IO_INTERFACE_DESCRIPTOR AcpiIo = {0};
    AcpiIo.NumFunctions = ACPIO_NUM_FUNCTIONS;
    AcpiIo.IoCallback = AcpiIoCallback;

    IoSetInterface(AcpiDevice, &AcpiIo);
    KiDrawDebugRect(9);
    return STATUS_SUCCESS;
}