// Exported from ghidra

typedef unsigned char   undefined;

//typedef unsigned char    bool;
typedef unsigned char    byte;
typedef unsigned int    dword;
typedef long long    longlong;
typedef unsigned char    uchar;
typedef unsigned int    uint;
typedef unsigned long    ulong;
typedef unsigned long long    ulonglong;
typedef unsigned char    undefined1;
typedef unsigned short    undefined2;
typedef unsigned int    undefined3;
typedef unsigned int    undefined4;
typedef unsigned long long    undefined8;
typedef unsigned short    ushort;
typedef struct SdmmcController SdmmcController, *PSdmmcController;



typedef struct _t210_sdmmc_t _t210_sdmmc_t, *P_t210_sdmmc_t;

typedef struct controller_sub controller_sub, *Pcontroller_sub;

//typedef struct weird_arg weird_arg, *Pweird_arg;

typedef struct event_info event_info, *Pevent_info;

typedef struct magic_thread_thing magic_thread_thing, *Pmagic_thread_thing;





struct magic_thread_thing {
    undefined field_0x0;
    undefined field_0x1;
    undefined field_0x2;
    undefined field_0x3;
    undefined field_0x4;
    undefined field_0x5;
    undefined field_0x6;
    undefined field_0x7;
    undefined field_0x8;
    undefined field_0x9;
    undefined field_0xa;
    undefined field_0xb;
    undefined field_0xc;
    undefined field_0xd;
    undefined field_0xe;
    undefined field_0xf;
    uint field_0x10;
    undefined field_0x14;
    undefined field_0x15;
    undefined field_0x16;
    undefined field_0x17;
    undefined field_0x18;
    undefined field_0x19;
    undefined field_0x1a;
    undefined field_0x1b;
    uint mutex_hand;
};

struct _t210_sdmmc_t {
    vu32 sysad;
    vu16 blksize;
    vu16 blkcnt;
    vu32 argument;
    vu16 trnmod;
    vu16 cmdreg;
    vu32 rspreg0;
    vu32 rspreg1;
    vu32 rspreg2;
    vu32 rspreg3;
    vu32 bdata;
    vu32 prnsts;
    vu8 hostctl;
    vu8 pwrcon;
    vu8 blkgap;
    vu8 wakcon;
    vu16 clkcon;
    vu8 timeoutcon;
    vu8 swrst;
    vu16 norintsts;
    vu16 errintsts;
    vu16 norintstsen;
    vu16 errintstsen;
    vu16 norintsigen;
    vu16 errintsigen;
    vu16 acmd12errsts;
    vu16 hostctl2;
    vu32 capareg;
    vu32 capareg_1;
    vu32 maxcurr;
    vu8 res3[4];
    vu16 setacmd12err;
    vu16 setinterr;
    vu8 admaerr;
    vu8 res4[3];
    vu32 admaaddr;
    vu32 admaaddr_hi;
    vu8 res5[156];
    vu16 slotintstatus;
    vu16 hcver;
    vu32 venclkctl;
    vu32 venspictl;
    vu32 venspiintsts;
    vu32 venceatactl;
    vu32 venbootctl;
    vu32 venbootacktout;
    vu32 venbootdattout;
    vu32 vendebouncecnt;
    vu32 venmiscctl;
    vu32 res6[34];
    vu32 field_1AC;
    vu32 field_1B0;
    vu8 res7[8];
    vu32 field_1BC;
    vu32 field_1C0;
    vu32 field_1C4;
    vu8 field_1C8[24];
    vu32 sdmemcmppadctl;
    vu32 autocalcfg;
    vu32 autocalintval;
    vu32 autocalsts;
    vu32 field_1F0;
};

struct controller_sub {
    ulonglong unk_ptr1;
    ulonglong * unk_ptr2;
    void * buffer;
};

struct weird_arg {
    undefined field_0x0;
    undefined field_0x1;
    undefined field_0x2;
    undefined field_0x3;
    undefined field_0x4;
    undefined field_0x5;
    undefined field_0x6;
    undefined field_0x7;
    undefined field_0x8;
    undefined field_0x9;
    undefined field_0xa;
    undefined field_0xb;
    undefined field_0xc;
    undefined field_0xd;
    undefined field_0xe;
    undefined field_0xf;
    longlong field_0x10;
    longlong field_0x18;
    undefined field_0x20;
    undefined field_0x21;
    undefined field_0x22;
    undefined field_0x23;
    undefined field_0x24;
    undefined field_0x25;
    undefined field_0x26;
    undefined field_0x27;
    undefined field_0x28;
    undefined field_0x29;
    undefined field_0x2a;
    undefined field_0x2b;
    undefined4 field_0x2c;
};

struct SdmmcController {
    undefined unused_field[8];
    struct _t210_sdmmc_t * t210_sdmmc;
    struct controller_sub sub[3];
    struct weird_arg unk_func_arg;
    struct event_info * event_info;
    undefined unk_buf_01[64];
    struct magic_thread_thing * magic_thread_thing;
    undefined field_0xd8[60];
    void * buf_0x80000_alligned;
    uint time_divisor;
    char unk_char;
    bool initialized;
    ushort unk_buf_02[4];
    vu32 rspreg_cache[4];
};

struct event_info {
    undefined field_0x0;
    undefined field_0x1;
    undefined field_0x2;
    undefined field_0x3;
    undefined field_0x4;
    undefined field_0x5;
    undefined field_0x6;
    undefined field_0x7;
    undefined field_0x8;
    undefined field_0x9;
    undefined field_0xa;
    undefined field_0xb;
    undefined field_0xc;
    undefined field_0xd;
    undefined field_0xe;
    undefined field_0xf;
    undefined field_0x10;
    undefined field_0x11;
    undefined field_0x12;
    undefined field_0x13;
    uint event_handle;
};

typedef struct sdmmc_cmd_t {
    ushort cmd;
    uint arg_sector;
    uint rsp_type;
    char check_busy;
} sdmmc_cmd_t;

typedef struct sdmmc_req sdmmc_req, *Psdmmc_req;

struct sdmmc_req {
    void * buf;
    ulonglong block_size;
    uint block_count;
    int is_write;
    ushort is_multi_block;
};

