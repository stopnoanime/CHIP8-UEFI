#include "file.h"
#include <protocol/efi-lip.h>
#include <protocol/efi-sfsp.h>

// File info with constant file name field size
typedef union EFI_FILE_INFO_CONST {
    EFI_FILE_INFO data;
    CHAR16 _[MAX_FILENAME_LEN + sizeof(EFI_FILE_INFO)];
} EFI_FILE_INFO_CONST;

/** https://wiki.osdev.org/Loading_files_under_UEFI */
EFI_FILE_PROTOCOL *open_volume(EFI_HANDLE image, EFI_SYSTEM_TABLE *SystemTable) {
    EFI_GUID lipGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
    EFI_LOADED_IMAGE_PROTOCOL *limg;

    EFI_GUID fsGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *sfs;

    EFI_FILE_PROTOCOL *Volume;

    SystemTable->BootServices->HandleProtocol(image, &lipGuid, (VOID **)&limg);
    SystemTable->BootServices->HandleProtocol(limg->DeviceHandle, &fsGuid, (VOID **)&sfs);
    sfs->OpenVolume(sfs, &Volume);

    return Volume;
}

EFI_FILE_PROTOCOL *open_directory(EFI_FILE_PROTOCOL *Volume, CHAR16 *dirname, EFI_SYSTEM_TABLE *SystemTable) {
    EFI_FILE_PROTOCOL *dir;
    Volume->Open(Volume, &dir, dirname, EFI_FILE_MODE_READ, 0);

    return dir;
}

static void copy_string(CHAR16 *dest, CHAR16 *src) {
    while ((*dest++ = *src++))
        ;
}

void read_file_names(EFI_FILE_PROTOCOL *dir, CHAR16 names[][MAX_FILENAME_LEN], int max_files) {
    EFI_FILE_INFO_CONST file_info;
    UINTN file_info_size;
    int i = 0;

    for (; i < max_files;) {
        file_info_size = sizeof(file_info);
        dir->Read(dir, &file_info_size, &file_info);

        if (file_info_size == 0)
            break;

        if (file_info.data.Attribute & EFI_FILE_DIRECTORY)
            continue;

        copy_string(names[i], file_info.data.FileName);
        
        i++;
    }

    // make other file names empty
    for (; i < max_files; i++)
        names[i][0] = 0;
}

void read_file(EFI_FILE_PROTOCOL *dir, CHAR16 *filename, uint8_t *buff, uint32_t buff_size) {
    EFI_FILE_PROTOCOL *file;
    dir->Open(dir, &file, filename, EFI_FILE_MODE_READ, 0);

    UINTN size = buff_size;
    file->Read(file, &size, buff);
}