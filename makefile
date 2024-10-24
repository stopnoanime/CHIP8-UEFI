CC = clang
CFLAGS = -I efi -target x86_64-pc-win32-coff -fno-stack-protector -fshort-wchar -mno-red-zone
LD = lld-link
LDFLAGS = -subsystem:efi_application -nodefaultlib -dll
EFI_PLATFORM = 1

.PHONY : all clean run

all : BOOTX64.EFI

main.efi: main.o display.o chip8.o input.o
	$(LD) $(LDFLAGS) -entry:efi_main $^ -out:$@

main.o: main.c
chip8.o: chip8.c
display.o: display.c
input.o: input.c

BOOTX64.EFI: main.efi
	mkdir --parents ./out/EFI/BOOT
	mkdir --parents ./out/roms
	cp main.efi ./out/EFI/BOOT/BOOTX64.EFI
	cp roms/* ./out/roms
	
run: BOOTX64.EFI
	qemu-system-x86_64 -net none -drive if=pflash,format=raw,unit=0,readonly=on,file=OVMF_CODE.fd -drive format=raw,file=fat:rw:out
	
clean:
	rm -rf *.lib *.efi *.EFI *.o out


