
set srcfiles=source/components/hardware/*.c source/components/disassembler/*.c source/components/dispatcher/*.c source/components/events/*.c source/components/executer/*.c source/components/utilities/* source/components/resources/*.c source/*.c

cl /O2i- %srcfiles% "../../../lib/noskx64.lib" /I../../../inc /Isource/include /Fe:acpi.sys /Fo:obj/ /link /DYNAMICBASE /FIXED:no /SUBSYSTEM:native /entry:DriverEntry
copy acpi.sys "..\..\..\diskimg\NewOS\System"