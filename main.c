#include "efi/efi-st.h"
#include "display.h"

void itoaw(unsigned val, CHAR16 *s) {
    CHAR16 *tmp = s;
    tmp[0] = 0;

    while (val || tmp == s) {
        *(++tmp) = (val % 10) + '0';
        val /= 10;
    }

    while (s < tmp) {
        CHAR16 swap;
        swap = *s;
        *(s++) = *tmp;
        *(tmp--) = swap;
    }
}

int efi_main(EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE* SystemTable) {
    init_display(SystemTable);

    for(int x = 0; x < CHIP8_DISPLAY_X; x++)
        for(int y= 0; y< CHIP8_DISPLAY_Y; y++)
            CHIP8_FRAMEBUFFER[x][y] = (x+y)%2;

    draw_framebuffer();
    
    for(;;);
    return 0;
}
