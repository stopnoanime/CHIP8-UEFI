CC = clang
CFLAGS = -I efi -target x86_64-pc-win32-coff -fno-stack-protector -fshort-wchar -mno-red-zone
LD = lld-link
LDFLAGS = -subsystem:efi_application -nodefaultlib -dll
EFI_PLATFORM = 1

.PHONY : all clean run

all : BOOTX64.EFI

main.efi : main.obj
	$(LD) $(LDFLAGS) -entry:efi_main $< -out:$@

main.obj : main.c
	$(CC) $(CFLAGS) -c $< -o $@

BOOTX64.EFI: main.efi
	mkdir --parents ./out/EFI/BOOT
	cp main.efi ./out/EFI/BOOT/BOOTX64.EFI

run: BOOTX64.EFI
	qemu-system-x86_64 -net none -drive if=pflash,format=raw,unit=0,readonly=on,file=OVMF_CODE.fd -drive format=raw,file=fat:rw:out
	
clean:
	rm -rf *.lib *.efi *.EFI *.obj


