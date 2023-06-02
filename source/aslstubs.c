// For intellisense
#ifndef WIN64
#define WIN64
#define _MSVC_VER
#endif
#include <ddk.h>
#include <acpi.h>
#include <actypes.h>

// ACPI_STATUS
// AcpiHwReadPort (
//     UINT64         Address,
//     UINT32                  *Value,
//     UINT32                  Width)
// {
//     KDebugPrint("AcpiHwReadPort");
//     return (AE_OK);
// }

// ACPI_STATUS
// AcpiHwWritePort (
//     UINT64         Address,
//     UINT32                  Value,
//     UINT32                  Width)
// {
//     KDebugPrint("AcpiHwWritePort");

//     return (AE_OK);
// }



#if (!ACPI_REDUCED_HARDWARE)





#endif /* !ACPI_REDUCED_HARDWARE */


// ACPI_STATUS
// AcpiExLoadTableOp (
//     void         *WalkState,
//     void     **ReturnDesc)
// {
//     KDebugPrint("***STUBS**** ------- AcpiExLoadTableOp");

//     return (AE_SUPPORT);
// }

// ACPI_STATUS
// AcpiExUnloadTable (
//     void     *DdbHandle)
// {
//     KDebugPrint("***STUBS**** ------- AcpiExUnloadTable");
    
//     return (AE_SUPPORT);
// }

// ACPI_STATUS
// AcpiExLoadOp (
//     void     *ObjDesc,
//     void     *Target,
//     void         *WalkState)
// {
//     KDebugPrint("***STUBS**** ------- AcpiExLoadOp");

//     return (AE_SUPPORT);
// }

// void
// AcpiExDoDebugObject (
//     void     *SourceDesc,
//     UINT32                  Level,
//     UINT32                  Index)
// {
//     KDebugPrint("***STUBS**** ------- AcpiExDoDebugObject");

//     return;
// }

void
AcpiExStartTraceMethod (
    void     *MethodNode,
    void     *ObjDesc,
    void         *WalkState)
{
    KDebugPrint("***STUBS**** ------- AcpiExStartTraceMethod");

    return;
}

void
AcpiExStopTraceMethod (
    void     *MethodNode,
    void     *ObjDesc,
    void         *WalkState)
{
    KDebugPrint("***STUBS**** ------- AcpiExStopTraceMethod");

    return;
}

void
AcpiExStartTraceOpcode (
    void       *Op,
    void         *WalkState)
{
    KDebugPrint("***STUBS**** ------- AcpiExStartTraceOpcode");

    return;
}

void
AcpiExStopTraceOpcode (
    void       *Op,
    void         *WalkState)

{
    KDebugPrint("***STUBS**** ------- AcpiExStopTraceOpcode");

    return;
}

void
AcpiExTracePoint (
    ACPI_TRACE_EVENT_TYPE   Type,
    BOOLEAN                 Begin,
    UINT8                   *Aml,
    char                    *Pathname)
{
    KDebugPrint("***STUBS**** ------- AcpiExTracePoint");

    return;
}

ACPI_STATUS
AcpiTbFindTable (
    char                    *Signature,
    char                    *OemId,
    char                    *OemTableId,
    UINT32                  *TableIndex)
{
    KDebugPrint("***STUBS**** ------- AcpiTbFindTable");

    return (AE_SUPPORT);
}

ACPI_STATUS
AcpiNsLoadTable (
    UINT32                  TableIndex,
    void     *Node)
{
    KDebugPrint("***STUBS**** ------- AcpiNsLoadTable");

    return (AE_NOT_IMPLEMENTED);
}
