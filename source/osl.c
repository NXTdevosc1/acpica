// For intellisense
#ifndef WIN64
#define WIN64
#define _MSVC_VER
#endif
#include <acpi.h>
#include <acpiosxf.h>

/*
 * OSL Initialization and shutdown primitives
 */
#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsInitialize
ACPI_STATUS
AcpiOsInitialize (
    void) {return 0;}
#endif

#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsTerminate
ACPI_STATUS
AcpiOsTerminate (
    void) {return 0;}
#endif


/*
 * ACPI Table interfaces
 */
#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsGetRootPointer
ACPI_PHYSICAL_ADDRESS
AcpiOsGetRootPointer (
    void) {return 0;}
#endif

#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsPredefinedOverride
ACPI_STATUS
AcpiOsPredefinedOverride (
    const ACPI_PREDEFINED_NAMES *InitVal,
    ACPI_STRING                 *NewVal) {return 0;}
#endif

#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsTableOverride
ACPI_STATUS
AcpiOsTableOverride (
    ACPI_TABLE_HEADER       *ExistingTable,
    ACPI_TABLE_HEADER       **NewTable) {return 0;}
#endif

#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsPhysicalTableOverride
ACPI_STATUS
AcpiOsPhysicalTableOverride (
    ACPI_TABLE_HEADER       *ExistingTable,
    ACPI_PHYSICAL_ADDRESS   *NewAddress,
    UINT32                  *NewTableLength) {return 0;}
#endif


/*
 * Spinlock primitives
 */
#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsCreateLock
ACPI_STATUS
AcpiOsCreateLock (
    ACPI_SPINLOCK           *OutHandle) {return 0;}
#endif

#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsDeleteLock
void
AcpiOsDeleteLock (
    ACPI_SPINLOCK           Handle) {}
#endif

#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsAcquireLock
ACPI_CPU_FLAGS
AcpiOsAcquireLock (
    ACPI_SPINLOCK           Handle) {return 0;}
#endif

#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsReleaseLock
void
AcpiOsReleaseLock (
    ACPI_SPINLOCK           Handle,
    ACPI_CPU_FLAGS          Flags) {}
#endif


/*
 * Semaphore primitives
 */
#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsCreateSemaphore
ACPI_STATUS
AcpiOsCreateSemaphore (
    UINT32                  MaxUnits,
    UINT32                  InitialUnits,
    ACPI_SEMAPHORE          *OutHandle) {return 0;}
#endif

#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsDeleteSemaphore
ACPI_STATUS
AcpiOsDeleteSemaphore (
    ACPI_SEMAPHORE          Handle) {return 0;}
#endif

#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsWaitSemaphore
ACPI_STATUS
AcpiOsWaitSemaphore (
    ACPI_SEMAPHORE          Handle,
    UINT32                  Units,
    UINT16                  Timeout) {return 0;}
#endif

ACPI_STATUS
AcpiOsSignalSemaphore (
    ACPI_SEMAPHORE          Handle,
    UINT32                  Units) {return 0;}




/*
 * Memory allocation and mapping
 */
#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsAllocate
void *
AcpiOsAllocate (
    ACPI_SIZE               Size) {return NULL;}
#endif

#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsFree
void
AcpiOsFree (
    void *                  Memory) {}
#endif

#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsMapMemory
void *
AcpiOsMapMemory (
    ACPI_PHYSICAL_ADDRESS   Where,
    ACPI_SIZE               Length) {return 0;}
#endif

#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsUnmapMemory
void
AcpiOsUnmapMemory (
    void                    *LogicalAddress,
    ACPI_SIZE               Size) {}
#endif

#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsGetPhysicalAddress
ACPI_STATUS
AcpiOsGetPhysicalAddress (
    void                    *LogicalAddress,
    ACPI_PHYSICAL_ADDRESS   *PhysicalAddress) {return 0;}
#endif


/*
 * Memory/Object Cache
 */
#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsCreateCache
ACPI_STATUS
AcpiOsCreateCache (
    char                    *CacheName,
    UINT16                  ObjectSize,
    UINT16                  MaxDepth,
    ACPI_CACHE_T            **ReturnCache) {return 0;}
#endif

#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsDeleteCache
ACPI_STATUS
AcpiOsDeleteCache (
    ACPI_CACHE_T            *Cache) {return 0;}
#endif

#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsPurgeCache
ACPI_STATUS
AcpiOsPurgeCache (
    ACPI_CACHE_T            *Cache) {return 0;}
#endif

#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsAcquireObject
void *
AcpiOsAcquireObject (
    ACPI_CACHE_T            *Cache) {return NULL;}
#endif

#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsReleaseObject
ACPI_STATUS
AcpiOsReleaseObject (
    ACPI_CACHE_T            *Cache,
    void                    *Object) {return 0;}
#endif


/*
 * Interrupt handlers
 */
#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsInstallInterruptHandler
ACPI_STATUS
AcpiOsInstallInterruptHandler (
    UINT32                  InterruptNumber,
    ACPI_OSD_HANDLER        ServiceRoutine,
    void                    *Context) {return 0;}
#endif

#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsRemoveInterruptHandler
ACPI_STATUS
AcpiOsRemoveInterruptHandler (
    UINT32                  InterruptNumber,
    ACPI_OSD_HANDLER        ServiceRoutine) {return 0;}
#endif


/*
 * Threads and Scheduling
 */
#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsGetThreadId
ACPI_THREAD_ID
AcpiOsGetThreadId (
    void) {return 0;}
#endif

#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsExecute
ACPI_STATUS
AcpiOsExecute (
    ACPI_EXECUTE_TYPE       Type,
    ACPI_OSD_EXEC_CALLBACK  Function,
    void                    *Context) {return 0;}
#endif

#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsWaitEventsComplete
void
AcpiOsWaitEventsComplete (
    void) {}
#endif

#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsSleep
void
AcpiOsSleep (
    UINT64                  Milliseconds) {return;}
#endif

#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsStall
void
AcpiOsStall (
    UINT32                  Microseconds) {return;}
#endif


/*
 * Platform and hardware-independent I/O interfaces
 */
#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsReadPort
ACPI_STATUS
AcpiOsReadPort (
    ACPI_IO_ADDRESS         Address,
    UINT32                  *Value,
    UINT32                  Width) {return 0;}
#endif

#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsWritePort
ACPI_STATUS
AcpiOsWritePort (
    ACPI_IO_ADDRESS         Address,
    UINT32                  Value,
    UINT32                  Width) {return 0;}
#endif


/*
 * Platform and hardware-independent physical memory interfaces
 */
#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsReadMemory
ACPI_STATUS
AcpiOsReadMemory (
    ACPI_PHYSICAL_ADDRESS   Address,
    UINT64                  *Value,
    UINT32                  Width) {return 0;}
#endif

#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsWriteMemory
ACPI_STATUS
AcpiOsWriteMemory (
    ACPI_PHYSICAL_ADDRESS   Address,
    UINT64                  Value,
    UINT32                  Width) {return 0;}
#endif


/*
 * Platform and hardware-independent PCI configuration space access
 * Note: Can't use "Register" as a parameter, changed to "Reg" --
 * certain compilers complain.
 */
#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsReadPciConfiguration
ACPI_STATUS
AcpiOsReadPciConfiguration (
    ACPI_PCI_ID             *PciId,
    UINT32                  Reg,
    UINT64                  *Value,
    UINT32                  Width) {return 0;}
#endif

#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsWritePciConfiguration
ACPI_STATUS
AcpiOsWritePciConfiguration (
    ACPI_PCI_ID             *PciId,
    UINT32                  Reg,
    UINT64                  Value,
    UINT32                  Width) {return 0;}
#endif


/*
 * Miscellaneous
 */
#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsReadable
BOOLEAN
AcpiOsReadable (
    void                    *Pointer,
    ACPI_SIZE               Length) {return FALSE;}
#endif

#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsWritable
BOOLEAN
AcpiOsWritable (
    void                    *Pointer,
    ACPI_SIZE               Length) {return FALSE;}
#endif

#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsGetTimer
UINT64
AcpiOsGetTimer (
    void) {return 0;}
#endif

#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsSignal
ACPI_STATUS
AcpiOsSignal (
    UINT32                  Function,
    void                    *Info) {return 0;}
#endif

#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsEnterSleep
ACPI_STATUS
AcpiOsEnterSleep (
    UINT8                   SleepState,
    UINT32                  RegaValue,
    UINT32                  RegbValue) {return 0;}
#endif


/*
 * Debug print routines
 */
#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsPrintf
ACPI_PRINTF_LIKE (1)
void ACPI_INTERNAL_VAR_XFACE
AcpiOsPrintf (
    const char              *Format,
    ...) {return;}
#endif

#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsVprintf
void
AcpiOsVprintf (
    const char              *Format,
    va_list                 Args) {return;}
#endif

#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsRedirectOutput
void
AcpiOsRedirectOutput (
    void                    *Destination) {return;}
#endif


/*
 * Debug IO
 */
#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsGetLine
ACPI_STATUS
AcpiOsGetLine (
    char                    *Buffer,
    UINT32                  BufferLength,
    UINT32                  *BytesRead) {return 0;}
#endif

#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsInitializeDebugger
ACPI_STATUS
AcpiOsInitializeDebugger (
    void) {return 0;}
#endif

#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsTerminateDebugger
void
AcpiOsTerminateDebugger (
    void) {return;}
#endif

#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsWaitCommandReady
ACPI_STATUS
AcpiOsWaitCommandReady (
    void) {return 0;}
#endif

#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsNotifyCommandComplete
ACPI_STATUS
AcpiOsNotifyCommandComplete (
    void) {return 0;}
#endif

#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsTracePoint
void
AcpiOsTracePoint (
    ACPI_TRACE_EVENT_TYPE   Type,
    BOOLEAN                 Begin,
    UINT8                   *Aml,
    char                    *Pathname) {return;}
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
    ACPI_PHYSICAL_ADDRESS   *Address) {return 0;}
#endif

#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsGetTableByIndex
ACPI_STATUS
AcpiOsGetTableByIndex (
    UINT32                  Index,
    ACPI_TABLE_HEADER       **Table,
    UINT32                  *Instance,
    ACPI_PHYSICAL_ADDRESS   *Address) {return 0;}
#endif

#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsGetTableByAddress
ACPI_STATUS
AcpiOsGetTableByAddress (
    ACPI_PHYSICAL_ADDRESS   Address,
    ACPI_TABLE_HEADER       **Table) {return 0;}
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
        return NULL;
    }
#endif

#ifndef ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsCloseDirectory
void
AcpiOsCloseDirectory (
    void                    *DirHandle) {
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
}

void
MpSaveSerialInfo (
    void       *Op,
    void            *Resource,
    char                    *DeviceName)
{
}