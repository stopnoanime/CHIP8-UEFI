// Compare interval and stall based loop
#include <efi-st.h>
#include <protocol/efi-gop.h>
#include <stdbool.h>

#define LOOP_PERIOD_US 16666
#define LOOP_COUNT 8
#define EXTRA_DELAY 10000

#define PRINT(s) SystemTable->ConOut->OutputString(SystemTable->ConOut, s)

void print_uint64_t(uint64_t num, EFI_SYSTEM_TABLE *SystemTable) {
    CHAR16 buf[25];
    int i = 0;

    do {
        buf[i++] = (num % 10) + u'0';
        num /= 10;
    } while (num > 0);
    i--;

    buf[i + 1] = u' ';
    buf[i + 2] = 0;

    for (int j = 0; j < i; j++, i--) {
        CHAR16 temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;
    }

    PRINT(buf);
}

void measure_time(uint64_t *last, EFI_SYSTEM_TABLE *SystemTable) {
    uint64_t curr = __builtin_ia32_rdtsc();

    if (*last != 0)
        print_uint64_t(curr - *last, SystemTable);

    *last = curr;
}

void delay_loop(int times, int extra_delay, EFI_SYSTEM_TABLE *SystemTable) {
    uint64_t timer = 0;

    for (int i = 0; i < times; i++) {
        measure_time(&timer, SystemTable);

        if (extra_delay)
            SystemTable->BootServices->Stall(extra_delay);

        SystemTable->BootServices->Stall(LOOP_PERIOD_US);
    }
}

typedef struct {
    int i;
    int count;
    uint64_t time;
    int extra_delay;
    bool should_start_next;
    EFI_SYSTEM_TABLE *SystemTable;
} CallbackContext;

CallbackContext context;

void interval_loop(int times, int extra_delay, EFI_SYSTEM_TABLE *SystemTable);

void interval_callback(EFI_EVENT Event, void *_) {
    EFI_SYSTEM_TABLE *SystemTable = context.SystemTable;

    measure_time(&context.time, SystemTable);

    if (context.extra_delay)
        SystemTable->BootServices->Stall(context.extra_delay);

    if (++context.i == context.count) {
        SystemTable->BootServices->SetTimer(Event, TimerCancel, 0);
        SystemTable->BootServices->CloseEvent(Event);

        if (context.should_start_next) {
            PRINT(u"\r\nInterval loop with 10ms extra delay:\r\n");
            interval_loop(LOOP_COUNT, EXTRA_DELAY, SystemTable);
            context.should_start_next = false;
        }
    }
}

void interval_loop(int times, int extra_delay, EFI_SYSTEM_TABLE *SystemTable) {
    EFI_EVENT TimerEvent;
    context = (CallbackContext){
        .i = 0,
        .time = 0,
        .count = times,
        .extra_delay = extra_delay,
        .should_start_next = true,
        .SystemTable = SystemTable,
    };

    SystemTable->BootServices->CreateEvent(
        EVT_TIMER | EVT_NOTIFY_SIGNAL, TPL_CALLBACK, interval_callback, 0, &TimerEvent
    );

    SystemTable->BootServices->SetTimer(TimerEvent, TimerPeriodic, LOOP_PERIOD_US * 10);
}

int efi_main(EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    SystemTable->ConOut->ClearScreen(SystemTable->ConOut);

    PRINT(u"Delay loop with no extra delay:\r\n");
    delay_loop(LOOP_COUNT, 0, SystemTable);

    PRINT(u"\r\nDelay loop with 10ms extra delay:\r\n");
    delay_loop(LOOP_COUNT, EXTRA_DELAY, SystemTable);

    PRINT(u"\r\nInterval loop with no extra delay:\r\n");
    interval_loop(LOOP_COUNT, 0, SystemTable);

    for (;;) {
    }
}
