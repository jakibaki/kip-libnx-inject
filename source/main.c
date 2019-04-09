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

// sleep(int milliseconds, uint divisor)
typedef void (*__fs_sleep_func)(int, unsigned int);

// uint controller_action(SdmmcController *controller,sdmmc_cmd_t *sdmmc_cmd,sdmmc_req *req, uint *blocks_transfered_write_out)
typedef uint (*__fs_controller_action_func)(SdmmcController *, sdmmc_cmd_t *, sdmmc_req *, uint *);

__fs_sleep_func __fs_sleep;
__fs_controller_action_func __fs_controller_action;

uint hooked_sdmmc_execute_cmd(SdmmcController *controller, sdmmc_cmd_t *sdmmc_cmd, sdmmc_req *req, uint *blocks_transfered)
{
    // Fun fact: accessing the sd while non-sd-requests are processed works somewhat fine (fopen doesn't want to work iirc but if you open the file in the other thread you can fwrite/fread without trouble)

    /*
    t210_sdmmc & 0xfff == 0x000 -> sd-card
    t210_sdmmc & 0xfff == 0x200 -> game-card
    t210_sdmmc & 0xfff == 0x600 -> nand
    */

    uint controller_action_res;
    _t210_sdmmc_t *t210_sdmmc;
    bool sd_clock_disabled;

    if (controller->initialized == false)
    {
        fatalSimple(0xaaffaa);
        //panic_idk(&DAT_7100144ec6, &DAT_7100144ec6, &DAT_7100144ec6, 0, in_x4, in_x5, in_x6, in_x7);
    }
    t210_sdmmc = controller->t210_sdmmc;
    sd_clock_disabled = (t210_sdmmc->clkcon >> 2 & 1) == 0;
    if (sd_clock_disabled)
    {
        /* enabled sd_clock */
        t210_sdmmc->clkcon = t210_sdmmc->clkcon | 4;
        (*__fs_sleep)(8, controller->time_divisor);
    }
    controller_action_res = (*__fs_controller_action)(controller, sdmmc_cmd, req, blocks_transfered);
    (*__fs_sleep)(8, controller->time_divisor);
    if (sd_clock_disabled)
    {
        controller->t210_sdmmc->clkcon = controller->t210_sdmmc->clkcon & 0xfffb;
    }

    u32 ctrl = (u64)controller->t210_sdmmc & 0xfff;

    if (is_inited && ctrl == 0x000) // log sd-access
    {
        if (req != NULL)
        {
            printf("Cmd: 0x%x Block size: 0x%llx Num Blocks: 0x%x Arg: 0x%x Writing?: %d\n", sdmmc_cmd->cmd, req->block_size, req->block_count, sdmmc_cmd->arg_sector, req->is_write);
        }
        else
        {
            printf("Cmd: 0x%x Arg: 0x%x\n", sdmmc_cmd->cmd, sdmmc_cmd->arg_sector);
        }
    }

    return controller_action_res;
}

void populate_function_pointers()
{
    MemoryInfo meminfo;
    u32 pageinfo;
    // Gets start of our .text section
    svcQueryMemory(&meminfo, &pageinfo, (u64)&populate_function_pointers);

    // hardcoded to 7.0 fat32 fs
    __fs_sleep = (__fs_sleep_func)meminfo.addr + INJECTED_SIZE + 0x1620d0;
    __fs_controller_action = (__fs_controller_action_func)meminfo.addr + INJECTED_SIZE + 0x166590;
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

    twiliInitialize();

    is_inited = true;
}

Thread our_thread;
void __injected_init()
{
    populate_function_pointers();
    threadCreate(&our_thread, thread_main, NULL, 0x1000, 0x3f, 3);
    threadStart(&our_thread);
}