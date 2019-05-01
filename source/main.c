#include <switch.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "twili.h"

#include "fs_types.h"

#define INJECTED_SIZE 0x100000

#define HEAP_SIZE 0x80000 //0x000540000

// we aren't an applet
u32 __nx_applet_type = AppletType_None;

// setup a fake heap (we don't need the heap anyway)
char fake_heap[HEAP_SIZE];

// we override libnx internals to do a minimal init
void __libnx_initheap(void)
{
    extern char *fake_heap_start;
    extern char *fake_heap_end;

    // setup newlib fake heap
    fake_heap_start = fake_heap;
    fake_heap_end = fake_heap + HEAP_SIZE;
}

void __appInit(void)
{
}

void __appExit(void)
{
}

// Useful for debugging in very early boot-stages
// Relies on a patched smcAmsIramCopy in exo which just calls panic instead
// If this gets called it will instantly reboot the switch (into rcm if autorcm is enabled)
void panic()
{
    SecmonArgs args = {0};
    args.X[0] = 0xF0000201;
    args.X[1] = 0xF00;

    svcCallSecureMonitor(&args);
}

bool is_inited = false;

typedef struct_mmc_obj2_t *(*__mmc_get_gc_vtab2)();
typedef struct_mmc_obj2_t *(*__mmc_get_sd_vtab2)();
typedef struct_mmc_obj2_t *(*__mmc_get_nand_vtab2)();

typedef ulonglong (*__vtab_rw_handler)(struct_mmc_obj2_t *, ulonglong, uint, void *, longlong, int);

__mmc_get_gc_vtab2 mmc_get_gc_vtab2;
__mmc_get_sd_vtab2 mmc_get_sd_vtab2;
__mmc_get_nand_vtab2 mmc_get_nand_vtab2;

ulonglong sdmmc_wrapper_read(void *buf, longlong buf_size, int mmc_id, uint sector,
                             ulonglong num_sectors)
{
    struct_mmc_obj2_t *this;
    ulonglong read_res;

    switch (mmc_id)
    {
    case 0:
        this = (*mmc_get_nand_vtab2)();
        break;
    case 1:
        this = (*mmc_get_sd_vtab2)();
        break;
    case 2:
        this = (*mmc_get_gc_vtab2)();
        break;
    default:
        fatalSimple(0xffd);
    }

    if (this != NULL)
    {
        read_res = (**(__vtab_rw_handler *)(this->vtab + 0x30))(this, (ulonglong)sector, num_sectors, buf, buf_size, 1);
        return read_res;
    }
    fatalSimple(0xffa);
}

void populate_function_pointers()
{
    MemoryInfo meminfo;
    u32 pageinfo;
    // Gets start of our .text section
    svcQueryMemory(&meminfo, &pageinfo, (u64)&populate_function_pointers);

    // hardcoded to 7.0 exfat fs
    //__fs_sleep = (__fs_sleep_func)meminfo.addr + INJECTED_SIZE + 0x1620d0;
    //__fs_controller_action = (__fs_controller_action_func)meminfo.addr + INJECTED_SIZE + 0x166590;

    // hardcoded to 6.1 exfat fs
    mmc_get_gc_vtab2 = (__mmc_get_gc_vtab2)meminfo.addr + INJECTED_SIZE + 0x15ee80;
    mmc_get_sd_vtab2 = (__mmc_get_sd_vtab2)meminfo.addr + INJECTED_SIZE + 0x15ebf0;
    mmc_get_nand_vtab2 = (__mmc_get_nand_vtab2)meminfo.addr + INJECTED_SIZE + 0x15b090;
}

void thread_main()
{
    svcSleepThread(10e+9L);

    Result rc;

    rc = smInitialize();
    if (R_FAILED(rc))
        fatalSimple(rc);

    rc = fsInitialize();
    if (R_FAILED(rc))
        fatalSimple(rc);

    rc = fsdevMountSdmc();
    if (R_FAILED(rc))
        fatalSimple(rc);

    //twiliInitialize();

    is_inited = true;
}

Thread our_thread;
void __injected_init()
{
    populate_function_pointers();
    threadCreate(&our_thread, thread_main, NULL, 0x1000, 0x3f, 3);
    threadStart(&our_thread);
}