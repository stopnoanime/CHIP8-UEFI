OUT_DIR := build
SRC := $(wildcard src/*.c)
OBJ := $(patsubst src/%.c, $(OUT_DIR)/%.o, $(SRC))

CC = clang
CFLAGS = -Iefi -target x86_64-pc-win32-coff -fno-stack-protector -fshort-wchar -mno-red-zone
LD = lld-link
LDFLAGS = -subsystem:efi_application -nodefaultlib -dll
EFI_PLATFORM = 1

.PHONY : all clean run partition

all : $(OUT_DIR)/main.efi

$(OUT_DIR)/main.efi: $(OBJ)
	$(LD) $(LDFLAGS) -entry:efi_main $^ -out:$@

$(OUT_DIR)/%.o: src/%.c | $(OUT_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(OUT_DIR):
	mkdir -p $@

partition: $(OUT_DIR)/main.efi
	mkdir -p partition/EFI/BOOT
	mkdir -p partition/roms
	cp $(OUT_DIR)/main.efi partition/EFI/BOOT/BOOTX64.EFI
	cp roms/* partition/roms

OVMF.fd:
	cp /usr/share/OVMF/OVMF_CODE.fd OVMF.fd

run: partition OVMF.fd
	qemu-system-x86_64 -net none -drive if=pflash,format=raw,unit=0,readonly=on,file=OVMF.fd -drive format=raw,file=fat:rw:partition
	
clean:
	rm -rf *.lib *.efi *.EFI *.o partition *.fd


