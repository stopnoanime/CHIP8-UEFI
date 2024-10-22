#include <stdint.h>
#include "efi/efi-st.h"

int efi_main(void *imageHandle, EFI_SYSTEM_TABLE* systemTable) {
    systemTable->ConOut->ClearScreen(systemTable->ConOut);
    systemTable->ConOut->OutputString(systemTable->ConOut, L"Hello, World?!");
    
    for(;;);
    return 0;
}
