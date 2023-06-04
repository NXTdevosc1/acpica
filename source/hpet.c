#include <acpios.h>

typedef volatile struct {
    UINT64 GeneralCapabilitiesAndId;
    UINT64 Reserved0;
    UINT64 GeneralConfiguration;
    UINT64 Reserved1;
    // GENERAL_INT_STATUS : Timers 0-31 INTSTS bitmask
    UINT64 GeneralInterruptStatus;
    UINT8 Reserved2[0xF0-0x28];
    UINT64 MainCounterValue;
    UINT64 Reserved3;
    struct {
        UINT64 TimerConfigurationAndCapability;
        UINT64 TimerComparatorValue;
        UINT64 TimerFsbInterruptRoute;
        UINT64 Reserved;
    } Timers[32];// Reserved for timers 3-31
} HPET_REGISTERS;

// General Capability and ID
    #define HPET_REVISION_ID 0 // Bit Offset
    #define HPET_NUM_TIMERS 8
    #define HPET_COUNT_SIZE 13
    #define HPET_LEGACY_REPLACEMENT_ROUTE 15
    #define HPET_VENDORID 16
    #define HPET_COUNTER_CLICK_PERIOD 32
// General Configuration
#define HPET_ENABLE 1
#define HPET_LEGACY_REPLACEMENT_ROUTE_ENABLE 2

// Timer(n) Config & Cap (Bit Offsets)
#define TIMER_INT_TYPE 1
#define TIMER_INT_ENABLE 2
#define TIMER_TYPE_CONFIG 3
#define TIMER_PERIODIC_INT_CAPABILITY 4
#define TIMER_SIZE 5 // 0 = 32 BITS / 1 = 64 BITS
#define TIMER_VALUE_SET 6
#define TIMER_32BIT_MODE_ENABLE 8
#define TIMER_INTERRUPT_ROUTE 9 // 5 Bits
#define TIMER_FSB_INTERRUPT_ENABLE 14
#define TIMER_FSB_DELIVERY_CAPABILITY 15
#define TIMER_INTERRUPT_ROUTING_CAPABILILTY 32

HPET_REGISTERS* HpetRegisters;
PDEVICE HpetDevice;
volatile UINT64 HpetFrequency = 0;
PKTIMER _HpetTimerObject = NULL;
UINT64 HpetReadCounter() {
    return HpetRegisters->MainCounterValue;
}


NSTATUS HpetInterruptHandler(
    INTERRUPT_HANDLER_DATA* HandlerData
) {
    KDebugPrint("Hpet Interrupt");
    KeTimerTick(HandlerData->Context);
    HpetRegisters->GeneralConfiguration &= ~HPET_ENABLE;
    HpetRegisters->MainCounterValue = 0;
    // Make the timer tick every second
    HpetRegisters->Timers[0].TimerComparatorValue = HpetFrequency;
    HpetRegisters->GeneralConfiguration |= HPET_ENABLE;
    return STATUS_SUCCESS;
}

ACPI_STATUS AcpiInitializeHpetTable() {
    ACPI_TABLE_HPET* Hpet;
    ACPI_STATUS Status = AcpiGetTable("HPET", 0, (ACPI_TABLE_HEADER**)&Hpet);
    FAILED_RETURN(Status);


    HpetRegisters = AcpiOsMapMemory(Hpet->Address.Address, 0x1000);
    KDebugPrint("HPET Found Address : %x, HPET Mapped Address : %x", Hpet->Address.Address, HpetRegisters);

    HpetDevice = PnpCreateDevice(DEVICE_TIMER, 0, L"ACPI/HPET", L"High Performance Event Timer");
    if(!HpetDevice) return AE_ERROR;
    // Configure Hpet
    HpetRegisters->GeneralConfiguration &= ~HPET_ENABLE;
    HpetRegisters->GeneralConfiguration |= HPET_LEGACY_REPLACEMENT_ROUTE_ENABLE;

    UINT NumTimers = ((HpetRegisters->GeneralCapabilitiesAndId >> HPET_NUM_TIMERS) & 0x1F) + 1;
    for(UINT i = 0;i<NumTimers;i++) {
        // Disable Interrupt, Set One Shot Mode, Set edge sensitive interrupt mode
        HpetRegisters->Timers[i].TimerConfigurationAndCapability &= ~((1 << TIMER_INT_ENABLE) | (1 << TIMER_TYPE_CONFIG) | (1 << TIMER_INT_TYPE));
    }

    IM_INTERRUPT_INFORMATION IntInfo;
    if(!KeQueryInterruptInformation(0, &IntInfo)) {
        return AE_ERROR;
    }

    // Configure first timer
    UINT32 IntMask = HpetRegisters->Timers[0].TimerConfigurationAndCapability >> TIMER_INTERRUPT_ROUTING_CAPABILILTY;
    UINT32 Cnf = HpetRegisters->Timers[0].TimerConfigurationAndCapability;
    Cnf &= ~(0x1F << TIMER_INTERRUPT_ROUTE);
    Cnf |= IntInfo.Fields.GlobalSystemInterrupt << TIMER_INTERRUPT_ROUTE;
    HpetRegisters->Timers[0].TimerConfigurationAndCapability = Cnf;



    HpetFrequency = 0x38D7EA4C68000 / (HpetRegisters->GeneralCapabilitiesAndId >> HPET_COUNTER_CLICK_PERIOD);
    KDebugPrint("Hpet Frequency : %d HZ", HpetFrequency);

    HpetRegisters->MainCounterValue = 0;
    HpetRegisters->Timers[0].TimerComparatorValue = HpetFrequency;
    HpetRegisters->Timers[0].TimerConfigurationAndCapability |= (1 << TIMER_INT_ENABLE);
    HpetRegisters->GeneralConfiguration |= HPET_ENABLE;

    IO_INTERFACE_DESCRIPTOR Io = {0};
    Io.NumFunctions = 1; // READ_COUNTER
    Io.IoCallback = (DEVICE_IO_CALLBACK)HpetReadCounter;
    
    if(!IoSetInterface(
        HpetDevice,
        &Io
    )) return AE_ERROR;


    PKTIMER Tmr;
    if(NERROR(KeCreateTimer(
        HpetDevice,
        TIMER_USAGE_COUNTER,
        HpetFrequency,
        &Tmr
    ))) return AE_ERROR;
    if(NERROR((Status = ExInstallInterruptHandler(0, 0, HpetInterruptHandler, (void*)Tmr)))) {
        KDebugPrint("HPET Failed to install int handler STATUS : %d", Status);
        return AE_ERROR;
    }

    _HpetTimerObject = Tmr;

    return AE_OK;
}