/*
 * JRPC2.xex (2014 retail release) checks fixed XBDM addresses and calls
 * internal parameter parsers by address. The stub branches are calculated
 * from the linker map and verified after linking.
 *
 * This block is private compatibility glue, not a replacement for XBDM's
 * command dispatcher. Keep the ordinary implementation in .text$B.
 */

#include <stddef.h>
#include "jrpc_branch_offsets.h"

typedef unsigned char JRPC_BYTE;
typedef unsigned long JRPC_DWORD;

#pragma pack(push, 1)
typedef struct _JRPC_COMPAT_LAYOUT {
    JRPC_BYTE reserved0[0x5380];
    JRPC_DWORD pchGetParam[4];
    JRPC_BYTE reserved1[0x5590 - 0x5380 - 0x10];
    JRPC_DWORD fGetSzParam[4];
    JRPC_BYTE reserved2[0x55E8 - 0x5590 - 0x10];
    JRPC_DWORD fGetDwParam[4];
    JRPC_BYTE reserved3[0x6E30 - 0x55E8 - 0x10];
    JRPC_DWORD jrpcHookFalse[8];
    JRPC_BYTE reserved4[0x9008 - 0x6E30 - 0x20];
    JRPC_DWORD jrpcHookTrue[8];
    JRPC_BYTE reserved5[0x16D30 - 0x9008 - 0x20];
    JRPC_DWORD layoutMarker;
} JRPC_COMPAT_LAYOUT;
#pragma pack(pop)

typedef char CheckPchOffset[(offsetof(JRPC_COMPAT_LAYOUT, pchGetParam) == 0x5380) ? 1 : -1];
typedef char CheckSzOffset[(offsetof(JRPC_COMPAT_LAYOUT, fGetSzParam) == 0x5590) ? 1 : -1];
typedef char CheckDwOffset[(offsetof(JRPC_COMPAT_LAYOUT, fGetDwParam) == 0x55E8) ? 1 : -1];
typedef char CheckHookAOffset[(offsetof(JRPC_COMPAT_LAYOUT, jrpcHookFalse) == 0x6E30) ? 1 : -1];
typedef char CheckHookBOffset[(offsetof(JRPC_COMPAT_LAYOUT, jrpcHookTrue) == 0x9008) ? 1 : -1];
/* lwz 0xBD30(r11) sign-extends its displacement: 0x91020000 - 0x42D0. */
typedef char CheckMarkerOffset[(offsetof(JRPC_COMPAT_LAYOUT, layoutMarker) == 0x16D30) ? 1 : -1];

/* Place this block at the start of XDK's executable .text section. */
#pragma section(".text", read, execute)
__declspec(allocate(".text")) const JRPC_COMPAT_LAYOUT g_jrpcCompat = {
    {0},
    {JRPC_PCH_BRANCH, 0x4E800020, 0x60000000, 0x60000000}, /* PchGetParam */
    {0},
    {JRPC_SZ_BRANCH, 0x4E800020, 0x60000000, 0x60000000}, /* FGetSzParam */
    {0},
    {JRPC_DW_BRANCH, 0x4E800020, 0x60000000, 0x60000000}, /* FGetDwParam */
    {0},
    /* JRPC2 copies and overwrites the first 16 bytes. Put the branch AFTER
     * those bytes so its original-function trampoline can safely resume at
     * +0x10; JRPC2 does not relocate an ordinary relative branch. */
    {0x60000000, 0x60000000, 0x60000000, 0x60000000,
     JRPC_FEATURES_BRANCH, 0x4E800020, 0x60000000, 0x60000000},
    {0},
    {0x60000000, 0x60000000, 0x60000000, 0x60000000,
     JRPC_GETFILE_BRANCH, 0x4E800020, 0x60000000, 0x60000000},
    {0},
    0x91001920
};
