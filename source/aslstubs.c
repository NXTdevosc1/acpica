// For intellisense
#ifndef WIN64
#define WIN64
#define _MSVC_VER
#endif
#include <acpi.h>
#include <actypes.h>


ACPI_STATUS
AcpiHwReadPort (
    UINT64         Address,
    UINT32                  *Value,
    UINT32                  Width)
{
    return (AE_OK);
}

ACPI_STATUS
AcpiHwWritePort (
    UINT64         Address,
    UINT32                  Value,
    UINT32                  Width)
{
    return (AE_OK);
}



#if (!ACPI_REDUCED_HARDWARE)





#endif /* !ACPI_REDUCED_HARDWARE */


ACPI_STATUS
AcpiExLoadTableOp (
    void         *WalkState,
    void     **ReturnDesc)
{
    return (AE_SUPPORT);
}

ACPI_STATUS
AcpiExUnloadTable (
    void     *DdbHandle)
{
    return (AE_SUPPORT);
}

ACPI_STATUS
AcpiExLoadOp (
    void     *ObjDesc,
    void     *Target,
    void         *WalkState)
{
    return (AE_SUPPORT);
}

void
AcpiExDoDebugObject (
    void     *SourceDesc,
    UINT32                  Level,
    UINT32                  Index)
{
    return;
}

void
AcpiExStartTraceMethod (
    void     *MethodNode,
    void     *ObjDesc,
    void         *WalkState)
{
    return;
}

void
AcpiExStopTraceMethod (
    void     *MethodNode,
    void     *ObjDesc,
    void         *WalkState)
{
    return;
}

void
AcpiExStartTraceOpcode (
    void       *Op,
    void         *WalkState)
{
    return;
}

void
AcpiExStopTraceOpcode (
    void       *Op,
    void         *WalkState)

{
    return;
}

void
AcpiExTracePoint (
    ACPI_TRACE_EVENT_TYPE   Type,
    BOOLEAN                 Begin,
    UINT8                   *Aml,
    char                    *Pathname)
{
    return;
}

ACPI_STATUS
AcpiTbFindTable (
    char                    *Signature,
    char                    *OemId,
    char                    *OemTableId,
    UINT32                  *TableIndex)
{
    return (AE_SUPPORT);
}

ACPI_STATUS
AcpiNsLoadTable (
    UINT32                  TableIndex,
    void     *Node)
{
    return (AE_NOT_IMPLEMENTED);
}
