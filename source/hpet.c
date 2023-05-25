#include <acpios.h>

ACPI_STATUS AcpiInitializeHpetTable() {
    ACPI_TABLE_HPET* Hpet;
    ACPI_STATUS Status = AcpiGetTable("HPET", 0, (ACPI_TABLE_HEADER**)&Hpet);
    FAILED_RETURN(Status);

    KDebugPrint("HPET Found");


    return AE_OK;
}