// For intellisense
#ifndef WIN64
#define WIN64
#define _MSVC_VER 1201
#endif
#include <ddk.h>
#include <acpi.h>
#ifndef ACPI_INLINE
#define ACPI_INLINE inline
#endif
#define ACPI_INIT_FUNCTION
#ifndef ACPI_MACHINE_WIDTH
#define ACPI_MACHINE_WIDTH 64
#endif
#include <acpixf.h>
#include <acpiosxf.h>
#include <acexcep.h>
#include <nosefi.h>
// #include <efi/Uefi.h>
/*
 * OSL Initialization and shutdown primitives
 */
ACPI_STATUS
AcpiOsInitialize (
    void) {
        KDebugPrint("AcpiOsInit");
        return 0;}

ACPI_STATUS
AcpiOsTerminate (
    void) {
        KDebugPrint("AcpiOsTerminate");
        
        return 0;}


/*
 * ACPI Table interfaces
 */
ACPI_PHYSICAL_ADDRESS
AcpiOsGetRootPointer (
    void) {
        KDebugPrint("AcpiOsGetRootPtr");

        GUID g = EFI_ACPI_20_TABLE_GUID;
        ACPI_PHYSICAL_ADDRESS Addr = (ACPI_PHYSICAL_ADDRESS)KeFindSystemFirmwareTable("RSD PTR ", &g);
        if(Addr) return Addr;
        g = ACPI_TABLE_GUID;
        return (ACPI_PHYSICAL_ADDRESS)KeFindSystemFirmwareTable("RSD PTR ", &g);
    }

ACPI_STATUS
AcpiOsPredefinedOverride (
    const ACPI_PREDEFINED_NAMES *InitVal,
    ACPI_STRING                 *NewVal) {
        KDebugPrint("AcpiOsPredefinedOverride Name : %s Type : %u Val : %s", InitVal->Name, InitVal->Type, InitVal->Val);
        KDebugPrint(InitVal->Name);
        KDebugPrint(InitVal->Val);
        
        *NewVal = NULL;
        return AE_OK;
    }

ACPI_STATUS
AcpiOsTableOverride (
    ACPI_TABLE_HEADER       *ExistingTable,
    ACPI_TABLE_HEADER       **NewTable) {
        KDebugPrint("AcpiOsTableOverride %s",
        ExistingTable->Signature
        );

        *NewTable = NULL;
        return AE_OK;
    }

ACPI_STATUS
AcpiOsPhysicalTableOverride (
    ACPI_TABLE_HEADER       *ExistingTable,
    ACPI_PHYSICAL_ADDRESS   *NewAddress,
    UINT32                  *NewTableLength) {
        KDebugPrint("AcpiOsPTableOverride %s",
        ExistingTable->Signature
        );

        *NewAddress = 0;
        *NewTableLength = 0;
        return AE_OK;
    }


/*
 * Spinlock primitives
 */
UINT NumLocks=0;
SPINLOCK Locks[256] = {0};
ACPI_STATUS
AcpiOsCreateLock (
    ACPI_SPINLOCK           *OutHandle) {
        KDebugPrint("AcpiOsCreateLock");

        *OutHandle = Locks + NumLocks;
        NumLocks++;
        return AE_OK;
    }

void
AcpiOsDeleteLock (
    ACPI_SPINLOCK           Handle) {
        KDebugPrint("AcpiOsDeleteLock");

    }

ACPI_CPU_FLAGS
AcpiOsAcquireLock (
    ACPI_SPINLOCK           Handle) {
        KDebugPrint("AcpiOsAcquireLock");

        return ExAcquireSpinLock(Handle);
    }

void
AcpiOsReleaseLock (
    ACPI_SPINLOCK           Handle,
    ACPI_CPU_FLAGS          Flags) {
        KDebugPrint("AcpiOsReleaseLock");

        ExReleaseSpinLock(Handle, Flags);
    }


/*
 * Semaphore primitives
 */
UINT semindex = 0;
SEMAPHORE Sem[256] = {0};
ACPI_STATUS
AcpiOsCreateSemaphore (
    UINT32                  MaxUnits,
    UINT32                  InitialUnits,
    ACPI_SEMAPHORE          *OutHandle) {
        KDebugPrint("AcpiOsCreateSemaphore");

        ExInitSemaphore(Sem + semindex, InitialUnits);
        *OutHandle = Sem + semindex;
        semindex++;
        return AE_OK;
    }

ACPI_STATUS
AcpiOsDeleteSemaphore (
    ACPI_SEMAPHORE          Handle) {
        KDebugPrint("AcpiOsDeleteSemaphore");
        
        return AE_OK;}

ACPI_STATUS
AcpiOsWaitSemaphore (
    ACPI_SEMAPHORE          Handle,
    UINT32                  Units,
    UINT16                  Timeout) {
                KDebugPrint("AcpiOsWaitSemaphore");

        return !ExSemaphoreWait(Handle, Timeout);
    }

ACPI_STATUS
AcpiOsSignalSemaphore (
    ACPI_SEMAPHORE          Handle,
    UINT32                  Units) {
                KDebugPrint("AcpiOsSignalSem");

        while(Units) {
            if(ExSemaphoreSignal(Handle)) return AE_OK;
            Units--;
        }
        return AE_ERROR;
    }




/*
 * Memory allocation and mapping
 */
#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsAllocate
void *
AcpiOsAllocate (
    ACPI_SIZE               Size) {
                KDebugPrint("AcpiOsAlloc %x", Size);

        return MmAllocatePool(Size, 0);}
#endif

#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsFree
void
AcpiOsFree (
    void *                  Memory) {
                KDebugPrint("AcpiOsFree %x", Memory);

        MmFreePool(Memory);}
#endif

extern 

void *
AcpiOsMapMemory (
    ACPI_PHYSICAL_ADDRESS   Where,
    ACPI_SIZE               Length) {
                KDebugPrint("AcpiOsMap addr : %x len : %x", Where, Length);
        UINT64 NumPages = Length + (Where & 0xFFF);
        if(NumPages & 0xFFF) NumPages+=0x1000;
        NumPages >>= 12;

        void* Vmem = KeFindAvailableAddressSpace(
            NULL,
            NumPages,
            NULL, NULL,
            PAGE_WRITE_ACCESS
        );
        if(!Vmem) {
            KDebugPrint("AcpiOsMap Failed.");
            return NULL;
        }
        KeMapVirtualMemory(
            NULL,
            (void*)Where,
            Vmem,
            NumPages,
            PAGE_WRITE_ACCESS | PAGE_GLOBAL,
            0
        );

        KeReleaseControlFlag(NULL, PROCESS_MANAGE_ADDRESS_SPACE);
        return Vmem;
    }

void
AcpiOsUnmapMemory (
    void                    *LogicalAddress,
    ACPI_SIZE               Size) {
               KDebugPrint("AcpiOsUnmap addr : %x sz : %x", LogicalAddress, Size);
                UINT64 NumPages = Size + ((UINT64)LogicalAddress & 0xFFF);
        if(NumPages & 0xFFF) NumPages+=0x1000;
        NumPages >>= 12;
        KeUnmapVirtualMemory(NULL, LogicalAddress, &NumPages);
    }

ACPI_STATUS
AcpiOsGetPhysicalAddress (
    void                    *LogicalAddress,
    ACPI_PHYSICAL_ADDRESS   *PhysicalAddress) {
                KDebugPrint("AcpiOsGetPhysAddr");

        *PhysicalAddress = (ACPI_PHYSICAL_ADDRESS)KeConvertPointer(NULL, LogicalAddress);
        if(*PhysicalAddress == 0) return AE_NOT_FOUND;

        return AE_OK;
    }




/*
 * Interrupt handlers
 */

UINT64 NumContexes = 0;

struct {
    ACPI_OSD_HANDLER Handler;
    void* Context;
} Contexes[128];

NSTATUS AcpiInterruptHandler(INTERRUPT_HANDLER_DATA* HandlerData) {
    
    UINT64 ContextId = (UINT64)HandlerData->Context;
    KDebugPrint("ACPI Interrupt.");
    Contexes[ContextId].Handler(Contexes[ContextId].Context);
    return STATUS_SUCCESS;
}

ACPI_STATUS
AcpiOsInstallInterruptHandler (
    UINT32                  InterruptNumber,
    ACPI_OSD_HANDLER        ServiceRoutine,
    void                    *Context) {
                KDebugPrint("AcpiOsInstallInt");

        NSTATUS s = ExInstallInterruptHandler(
            InterruptNumber,
            AcpiInterruptHandler,
            (void*)(NumContexes)
        );
        Contexes[NumContexes].Handler = ServiceRoutine;
        Contexes[NumContexes].Context = Context;
        NumContexes++;
        if(s == STATUS_SUCCESS) return AE_OK;
        return AE_NOT_ACQUIRED;
    }

ACPI_STATUS
AcpiOsRemoveInterruptHandler (
    UINT32                  InterruptNumber,
    ACPI_OSD_HANDLER        ServiceRoutine) {
                KDebugPrint("AcpiOsRemoveInt");

        NSTATUS s = ExRemoveInterruptHandler(
            InterruptNumber,
            AcpiInterruptHandler
        );
        if(s == STATUS_SUCCESS) return AE_OK;
        return AE_NOT_FOUND;
    }


/*
 * Threads and Scheduling
 */
ACPI_THREAD_ID
AcpiOsGetThreadId (
    void) {
                KDebugPrint("AcpiOsGetThreadId");

        return 0;}

ACPI_STATUS
AcpiOsExecute (
    ACPI_EXECUTE_TYPE       Type,
    ACPI_OSD_EXEC_CALLBACK  Function,
    void                    *Context) {
                KDebugPrint("AcpiOsExecute");

        return 0;}

void
AcpiOsWaitEventsComplete (
    void) {
                KDebugPrint("AcpiOsWaitEvtComplete");

    }

void
AcpiOsSleep (
    UINT64                  Milliseconds) {
                KDebugPrint("AcpiOsSleep %u ms", Milliseconds);

        return;}
#include <intrin.h>
void
AcpiOsStall (
    UINT32                  Microseconds) {
                KDebugPrint("AcpiOsStall %u us", Microseconds);

    for(UINT64 i = 0;i<0x10000;i++) _mm_pause();

        return;}


// /*
//  * Platform and hardware-independent I/O interfaces
//  */
ACPI_STATUS
AcpiOsReadPort (
    ACPI_IO_ADDRESS         Address,
    UINT32                  *Value,
    UINT32                  Width) {
                KDebugPrint("AcpiOsReadPort");

        return 0;}


ACPI_STATUS
AcpiOsWritePort (
    ACPI_IO_ADDRESS         Address,
    UINT32                  Value,
    UINT32                  Width) {
                KDebugPrint("AcpiOsWritePort");

        return 0;}


// /*
//  * Platform and hardware-independent physical memory interfaces
//  */
ACPI_STATUS
AcpiOsReadMemory (
    ACPI_PHYSICAL_ADDRESS   Address,
    UINT64                  *Value,
    UINT32                  Width) {
                KDebugPrint("AcpiOsReadMem");

        return 0;}

ACPI_STATUS
AcpiOsWriteMemory (
    ACPI_PHYSICAL_ADDRESS   Address,
    UINT64                  Value,
    UINT32                  Width) {
                KDebugPrint("AcpiOsWriteMem");

        return 0;}


/*
 * Platform and hardware-independent PCI configuration space access
 * Note: Can't use "Register" as a parameter, changed to "Reg" --
 * certain compilers complain.
//  */
ACPI_STATUS
AcpiOsReadPciConfiguration (
    ACPI_PCI_ID             *PciId,
    UINT32                  Reg,
    UINT64                  *Value,
    UINT32                  Width) {
                KDebugPrint("AcpiOsReadPci");

        return 0;}

ACPI_STATUS
AcpiOsWritePciConfiguration (
    ACPI_PCI_ID             *PciId,
    UINT32                  Reg,
    UINT64                  Value,
    UINT32                  Width) {
                KDebugPrint("AcpiOsWritePci");

        return 0;}


/*
 * Miscellaneous
 */
BOOLEAN
AcpiOsReadable (
    void                    *Pointer,
    ACPI_SIZE               Length) {
                KDebugPrint("AcpiOsReadable");

        return KeCheckMemoryAccess(NULL, Pointer, Length, NULL);
    }

BOOLEAN
AcpiOsWritable (
    void                    *Pointer,
    ACPI_SIZE               Length) {
                KDebugPrint("AcpiOsWritable");

        UINT64 f;
        if(KeCheckMemoryAccess(NULL, Pointer, Length, &f)) {
            if(f & PAGE_WRITE_ACCESS) return TRUE;
        }
        return FALSE;
    }

UINT64
AcpiOsGetTimer (
    void) {        KDebugPrint("AcpiOsGetTimer");
return 0;}

ACPI_STATUS
AcpiOsSignal (
    UINT32                  Function,
    void                    *Info) {
                KDebugPrint("AcpiOsSignal");

        return 0;}

ACPI_STATUS
AcpiOsEnterSleep (
    UINT8                   SleepState,
    UINT32                  RegaValue,
    UINT32                  RegbValue) {
                KDebugPrint("AcpiOsEnterSleep");

        return 0;}


/*
 * Debug print routines
 */
#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsPrintf
ACPI_PRINTF_LIKE (1)
void ACPI_INTERNAL_VAR_XFACE
AcpiOsPrintf (
    const char              *Format,
    ...) {        KDebugPrint(Format);
return;}
#endif

#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsVprintf
void
AcpiOsVprintf (
    const char              *Format,
    va_list                 Args) {        KDebugPrint(Format);
return;}
#endif

#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsRedirectOutput
void
AcpiOsRedirectOutput (
    void                    *Destination) {
                KDebugPrint("OsRedirOutput");

        return;}
#endif


/*
 * Debug IO
 */
#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsGetLine
ACPI_STATUS
AcpiOsGetLine (
    char                    *Buffer,
    UINT32                  BufferLength,
    UINT32                  *BytesRead) {
                KDebugPrint("AcpiOsGetLine");

        return 0;}
#endif

#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsInitializeDebugger
ACPI_STATUS
AcpiOsInitializeDebugger (
    void) {        KDebugPrint("AcpiOsInitDebugger");
return 0;}
#endif

#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsTerminateDebugger
void
AcpiOsTerminateDebugger (
    void) {        KDebugPrint("AcpiOsTerminateDebugger");
return;}
#endif

#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsWaitCommandReady
ACPI_STATUS
AcpiOsWaitCommandReady (
    void) {        KDebugPrint("AcpiOsWaitCmdReady");
return 0;}
#endif

#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsNotifyCommandComplete
ACPI_STATUS
AcpiOsNotifyCommandComplete (
    void) {        KDebugPrint("AcpiOsNotifyCmdComplete");
return 0;}
#endif

#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsTracePoint
void
AcpiOsTracePoint (
    ACPI_TRACE_EVENT_TYPE   Type,
    BOOLEAN                 Begin,
    UINT8                   *Aml,
    char                    *Pathname) {        KDebugPrint("AcpiOsTracePoint");
return;}
#endif


/*
 * Obtain ACPI table(s)
 */
#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsGetTableByName
ACPI_STATUS
AcpiOsGetTableByName (
    char                    *Signature,
    UINT32                  Instance,
    ACPI_TABLE_HEADER       **Table,
    ACPI_PHYSICAL_ADDRESS   *Address) {
        KDebugPrint("AcpiOsGetTableByName");
        return 0;}
#endif

#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsGetTableByIndex
ACPI_STATUS
AcpiOsGetTableByIndex (
    UINT32                  Index,
    ACPI_TABLE_HEADER       **Table,
    UINT32                  *Instance,
    ACPI_PHYSICAL_ADDRESS   *Address) {
        KDebugPrint("AcpiOsGetTableByIndex");
        
        return 0;}
#endif

#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsGetTableByAddress
ACPI_STATUS
AcpiOsGetTableByAddress (
    ACPI_PHYSICAL_ADDRESS   Address,
    ACPI_TABLE_HEADER       **Table) {
        KDebugPrint("AcpiOsGetTableByAddress");
        
        return 0;}
#endif


/*
 * Directory manipulation
 */
#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsOpenDirectory
void *
AcpiOsOpenDirectory (
    char                    *Pathname,
    char                    *WildcardSpec,
    char                    RequestedFileType) {
        KDebugPrint("AcpiOsOpenDirectory");

        return NULL;
    }
#endif

/* RequesteFileType values */

#define REQUEST_FILE_ONLY                   0
#define REQUEST_DIR_ONLY                    1


#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsGetNextFilename
char *
AcpiOsGetNextFilename (
    void                    *DirHandle) {
        KDebugPrint("AcpiOsGetNextFilename");

        return NULL;
    }
#endif

#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsCloseDirectory
void
AcpiOsCloseDirectory (
    void                    *DirHandle) {
        KDebugPrint("AcpiOsCloseDirectory");

        return;
    }
#endif




/* Stubs for the disassembler */

void
MpSaveGpioInfo (
    void       *Op,
    void            *Resource,
    UINT32                  PinCount,
    UINT16                  *PinList,
    char                    *DeviceName)
{
        KDebugPrint("MpSaveGpioInfo");

}

void
MpSaveSerialInfo (
    void       *Op,
    void            *Resource,
    char                    *DeviceName)
{
        KDebugPrint("MpSaveSerialInfo");

}