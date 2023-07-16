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
#include <intmgr.h>
#include <acpios.h>
// #include <efi/Uefi.h>
/*
 * OSL Initialization and shutdown primitives
 */
ACPI_STATUS
AcpiOsInitialize (
    void) {
        // KDebugPrint("AcpiOsInit");
        return 0;}

ACPI_STATUS
AcpiOsTerminate (
    void) {
        // KDebugPrint("AcpiOsTerminate");
        
        return 0;}


/*
 * ACPI Table interfaces
 */
ACPI_PHYSICAL_ADDRESS
AcpiOsGetRootPointer (
    void) {
        // KDebugPrint("AcpiOsGetRootPtr");

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
        // KDebugPrint("AcpiOsPredefinedOverride Name : %s Type : %u Val : %s", InitVal->Name, InitVal->Type, InitVal->Val);
        // KDebugPrint(InitVal->Name);
        // KDebugPrint(InitVal->Val);
        
        *NewVal = NULL;
        return AE_OK;
    }

ACPI_STATUS
AcpiOsTableOverride (
    ACPI_TABLE_HEADER       *ExistingTable,
    ACPI_TABLE_HEADER       **NewTable) {
        // KDebugPrint("AcpiOsTableOverride %s",
        // ExistingTable->Signature
        // );

        *NewTable = NULL;
        return AE_OK;
    }

ACPI_STATUS
AcpiOsPhysicalTableOverride (
    ACPI_TABLE_HEADER       *ExistingTable,
    ACPI_PHYSICAL_ADDRESS   *NewAddress,
    UINT32                  *NewTableLength) {
        // KDebugPrint("AcpiOsPTableOverride %s",
        // ExistingTable->Signature
        // );

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
        // KDebugPrint("AcpiOsCreateLock");

        *OutHandle = Locks + NumLocks;
        NumLocks++;
        return AE_OK;
    }

void
AcpiOsDeleteLock (
    ACPI_SPINLOCK           Handle) {
        // KDebugPrint("AcpiOsDeleteLock");

    }

ACPI_CPU_FLAGS
AcpiOsAcquireLock (
    ACPI_SPINLOCK           Handle) {
        // KDebugPrint("AcpiOsAcquireLock");

        return ExAcquireSpinLock(Handle);
    }

void
AcpiOsReleaseLock (
    ACPI_SPINLOCK           Handle,
    ACPI_CPU_FLAGS          Flags) {
        // KDebugPrint("AcpiOsReleaseLock");

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
        // KDebugPrint("AcpiOsCreateSemaphore");

        ExInitSemaphore(Sem + semindex, InitialUnits);
        *OutHandle = Sem + semindex;
        semindex++;
        return AE_OK;
    }

ACPI_STATUS
AcpiOsDeleteSemaphore (
    ACPI_SEMAPHORE          Handle) {
        // KDebugPrint("AcpiOsDeleteSemaphore");
        
        return AE_OK;}

ACPI_STATUS
AcpiOsWaitSemaphore (
    ACPI_SEMAPHORE          Handle,
    UINT32                  Units,
    UINT16                  Timeout) {
                // KDebugPrint("AcpiOsWaitSemaphore");

        return !ExSemaphoreWait(Handle, Timeout);
    }

ACPI_STATUS
AcpiOsSignalSemaphore (
    ACPI_SEMAPHORE          Handle,
    UINT32                  Units) {
                // KDebugPrint("AcpiOsSignalSem");

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
        void* p = MmAllocatePool(Size, 0);
                // KDebugPrint("AcpiOsAlloc s: 0x%x adr : 0x%x", Size, p);

        return p;
    }
#endif

#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsFree
void
AcpiOsFree (
    void *                  Memory) {
                // KDebugPrint("AcpiOsFree %x", Memory);

        MmFreePool(Memory);}
#endif

extern 

void *
AcpiOsMapMemory (
    ACPI_PHYSICAL_ADDRESS   Where,
    ACPI_SIZE               Length) {
                // KDebugPrint("AcpiOsMap addr : %x len : %x", Where, Length);
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
            PAGE_WRITE_ACCESS | PAGE_EXECUTE,
            0
        );

        KeReleaseControlFlag(NULL, PROCESS_MANAGE_ADDRESS_SPACE);
        return (void*)((UINT64)Vmem + (Where & 0xFFF));
    }

void
AcpiOsUnmapMemory (
    void                    *LogicalAddress,
    ACPI_SIZE               Size) {
            //    KDebugPrint("AcpiOsUnmap addr : %x sz : %x", LogicalAddress, Size);
                UINT64 NumPages = Size + ((UINT64)LogicalAddress & 0xFFF);
        if(NumPages & 0xFFF) NumPages+=0x1000;
        NumPages >>= 12;
        KeUnmapVirtualMemory(NULL, LogicalAddress, &NumPages);
    }

ACPI_STATUS
AcpiOsGetPhysicalAddress (
    void                    *LogicalAddress,
    ACPI_PHYSICAL_ADDRESS   *PhysicalAddress) {
                // KDebugPrint("AcpiOsGetPhysAddr");

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
} AcpiIrqs[200];

static UINT ic = 0;
static UINT ics[8] = {
    0xFFFFFF,
    0xFF,
    0xFFFF,
    0xFF00,
    0xFF0000,
    0xFF00FF,
    0xFFFF00,
    0
};

void KRNLAPI __KiClearScreen(UINT Color);

NSTATUS AcpiInterruptHandler(INTERRUPT_HANDLER_DATA* HandlerData) {
    if(ic == 8) ic = 0;
    __KiClearScreen(ics[ic]);
    ic++;
    UINT64 ContextId = (UINT64)HandlerData->Context;
    KDebugPrint("ACPI Interrupt.");
    AcpiIrqs[HandlerData->InterruptNumber].Handler(AcpiIrqs[HandlerData->InterruptNumber].Context);
    return STATUS_SUCCESS;
}

ACPI_STATUS
AcpiOsInstallInterruptHandler (
    UINT32                  InterruptNumber,
    ACPI_OSD_HANDLER        ServiceRoutine,
    void                    *Context) {
                KDebugPrint("AcpiOsInstallInt#%d ISR %x CTX %x", InterruptNumber, ServiceRoutine, Context);

        NSTATUS s = ExInstallInterruptHandler(
            InterruptNumber,
            IM_LEVEL_TRIGGERED, // GSI's are by default level triggered
            AcpiInterruptHandler,
            NULL
        );
        AcpiIrqs[InterruptNumber].Handler = ServiceRoutine;
        AcpiIrqs[InterruptNumber].Context = Context;
        if(s == STATUS_SUCCESS) return AE_OK;
        KDebugPrint("AcpiOsInstallInt Failed code %d", s);
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
        UINT64 ThreadId = KeGetCurrentThreadId();
        return ThreadId;
    }

ACPI_STATUS
AcpiOsExecute (
    ACPI_EXECUTE_TYPE       Type,
    ACPI_OSD_EXEC_CALLBACK  Function,
    void                    *Context) {
        KDebugPrint("ACPI OS EXECUTE Type : %d Fun : %x Context : %x", Type, Function, Context);
        return KeCreateThread(NULL, NULL, 0, Function, Context);            
    }

void
AcpiOsWaitEventsComplete (
    void) {
                // KDebugPrint("AcpiOsWaitEvtComplete");

    }

void
AcpiOsSleep (
    UINT64                  Milliseconds) {
                // KDebugPrint("AcpiOsSleep %u ms", Milliseconds);
                Stall(Milliseconds * 1000);
        return;}
#include <intrin.h>
void
AcpiOsStall (
    UINT32                  Microseconds) {
                // KDebugPrint("AcpiOsStall %u us", Microseconds);

            Stall(Microseconds);

        return;}


// /*
//  * Platform and hardware-independent I/O interfaces
//  */
ACPI_STATUS
AcpiOsReadPort (
    ACPI_IO_ADDRESS         Address,
    UINT32                  *Value,
    UINT32                  Width) {
            // KDebugPrint("AcpiOsReadPort");
        
    if(Width == 32) {
        *Value = __indword(Address);
    } else if(Width == 16) {
        *Value = __inword(Address);

    } else if(Width == 8) {
        *Value = __inbyte(Address);
    } else return AE_ERROR;

    return AE_OK;
}


ACPI_STATUS
AcpiOsWritePort (
    ACPI_IO_ADDRESS         Address,
    UINT32                  Value,
    UINT32                  Width) {
                // KDebugPrint("AcpiOsWritePort");
        if(Width == 32) {
            __outdword(Address, Value);
        } else if(Width == 16) {
            __outword(Address, Value);
        } else if(Width == 8) {
            __outbyte(Address, Value);
        } else return AE_ERROR;

        return AE_OK;
}


// /*
//  * Platform and hardware-independent physical memory interfaces
//  */
ACPI_STATUS
AcpiOsReadMemory (
    ACPI_PHYSICAL_ADDRESS   Address,
    UINT64                  *Value,
    UINT32                  Width) {
                // KDebugPrint("AcpiOsReadMem");
        *Value = *(volatile UINT64*)Address;
        return AE_OK;
}

ACPI_STATUS
AcpiOsWriteMemory (
    ACPI_PHYSICAL_ADDRESS   Address,
    UINT64                  Value,
    UINT32                  Width) {
                // KDebugPrint("AcpiOsWriteMem");
        if(Width == 8) {
            *(volatile UINT8*)Address = Value;

        } else if(Width == 16) {
            *(volatile UINT16*)Address = Value;

        } else if(Width == 32) {
            *(volatile UINT32*)Address = Value;

        } else if(Width == 64) {
            *(volatile UINT64*)Address = Value;
        } else return AE_ERROR;
        return AE_OK;}


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
    ACPI_MCFG_ALLOCATION* Allocation;
    KDebugPrint("AcpiOsReadPci");
    if(PciGetSegment(PciId->Segment, &Allocation)) {
        if(!Allocation) {
            KDebugPrint("TODO: READ I/O");
            while(1);
        } else {
            if(PciId->Bus < Allocation->StartBusNumber) return AE_ERROR;
            *Value = *(UINT64*)(Allocation->Address + ((PciId->Bus - Allocation->StartBusNumber) << 20) | (PciId->Device << 15) | (PciId->Function << 12));
        }
        return AE_OK;
    }
    return AE_ERROR;    
}

ACPI_STATUS
AcpiOsWritePciConfiguration (
    ACPI_PCI_ID             *PciId,
    UINT32                  Reg,
    UINT64                  Value,
    UINT32                  Width) {
                KDebugPrint("AcpiOsWritePci");
    ACPI_MCFG_ALLOCATION* Allocation;

        if(PciGetSegment(PciId->Segment, &Allocation)) {
            if(!Allocation) {
                KDebugPrint("TODO: READ I/O");
                while(1);
            } else {
                if(PciId->Bus < Allocation->StartBusNumber) return AE_ERROR;
                if(Width == 64) {
                    *(UINT64*)(Allocation->Address + ((PciId->Bus - Allocation->StartBusNumber) << 20) | (PciId->Device << 15) | (PciId->Function << 12)) = Value;
                } else if(Width == 32) {
                    *(UINT32*)(Allocation->Address + ((PciId->Bus - Allocation->StartBusNumber) << 20) | (PciId->Device << 15) | (PciId->Function << 12)) = Value;
                } else if(Width == 16) {
                    *(UINT16*)(Allocation->Address + ((PciId->Bus - Allocation->StartBusNumber) << 20) | (PciId->Device << 15) | (PciId->Function << 12)) = Value;
                } else if(Width == 8) {
                    *(UINT8*)(Allocation->Address + ((PciId->Bus - Allocation->StartBusNumber) << 20) | (PciId->Device << 15) | (PciId->Function << 12)) = Value;
                } else return AE_ERROR;
            }
            return AE_OK;
        }
        return AE_ERROR;
}


/*
 * Miscellaneous
 */
BOOLEAN
AcpiOsReadable (
    void                    *Pointer,
    ACPI_SIZE               Length) {
                // KDebugPrint("AcpiOsReadable");

        return KeCheckMemoryAccess(NULL, Pointer, Length, NULL);
    }

BOOLEAN
AcpiOsWritable (
    void                    *Pointer,
    ACPI_SIZE               Length) {
                // KDebugPrint("AcpiOsWritable");

        UINT64 f;
        if(KeCheckMemoryAccess(NULL, Pointer, Length, &f)) {
            if(f & PAGE_WRITE_ACCESS) return TRUE;
        }
        return FALSE;
    }
extern PKTIMER _HpetTimerObject;
UINT64
AcpiOsGetTimer (
    void) {
        // Read time & Convert it to 100 nanosecond unit
        UINT64 Time = KeGetTimeSinceBoot() * 10;
        KDebugPrint("AcpiOsGetTimer %x", Time);
    
        return Time;
}

ACPI_STATUS
AcpiOsSignal (
    UINT32                  Function,
    void                    *Info) {
                KDebugPrint("UNIMPLEMENTED !! __ AcpiOsSignal");

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
return AE_NOT_IMPLEMENTED;}
#endif

#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsNotifyCommandComplete
ACPI_STATUS
AcpiOsNotifyCommandComplete (
    void) {        KDebugPrint("AcpiOsNotifyCmdComplete");
return AE_NOT_IMPLEMENTED;}
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