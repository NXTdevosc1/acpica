
set srcfiles=source/*.c
set objfiles=
cl /O2i- %srcfiles% %objfiles% /GS- "../../../lib/noskx64.lib" "generate/msvc2017/AcpiSubsystem/AcpiSubsystem.lib" "../../../lib/syscruntime.lib" /I../../../inc /Isource/include /DWIN64 /Fe:acpi.sys /Fo:obj/ /link /DYNAMICBASE /FIXED:no /MACHINE:X64 /RELEASE /SUBSYSTEM:native /entry:DriverEntry
copy acpi.sys "..\..\..\diskimg\NewOS\System"

@REM cd ../../..
@REM wsl sudo make createfs
@REM cd src/drivers/acpi