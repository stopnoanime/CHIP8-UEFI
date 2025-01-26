#ifndef FILE_IO
#define FILE_IO

#include <efi-st.h>
#include <protocol/efi-fp.h>

#define MAX_FILENAME_LEN 32

EFI_FILE_PROTOCOL *open_volume(EFI_HANDLE image, EFI_SYSTEM_TABLE *SystemTable);
EFI_FILE_PROTOCOL *open_directory(EFI_FILE_PROTOCOL *Volume, CHAR16 *dirname, EFI_SYSTEM_TABLE *SystemTable);
void read_file_names(EFI_FILE_PROTOCOL *dir, CHAR16 names[][MAX_FILENAME_LEN], int max_files);
void read_file(EFI_FILE_PROTOCOL *dir, CHAR16 *filename, uint8_t *buff, uint32_t buff_size);

#endif
