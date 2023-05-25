#include <acpios.h>

ACPI_STATUS AcpiInitializePcieConfiguration() {
    ACPI_TABLE_MCFG* Mcfg;
    ACPI_STATUS Status = AcpiGetTable("MCFG", 0, (ACPI_TABLE_HEADER**)&Mcfg);
    FAILED_RETURN(Status);

    KDebugPrint("MCFG Found");


    return STATUS_SUCCESS;
}