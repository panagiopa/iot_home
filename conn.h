#ifndef __CONN_H_
#define __CONN_H__

#include "git/home_iot.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "utils/uartstdio.h"
#include "stats.h"
#include "drivers/eth_client_lwip.h"
#include "exosite.h"
//*****************************************************************************
//
// Global macro definitions.
//
//*****************************************************************************
#define MAX_SYNC_RETRIES        20
#define APP_INPUT_BUF_SIZE                  1024


extern char g_cInput[APP_INPUT_BUF_SIZE];
extern uint32_t g_ui32LinkRetries;
extern volatile bool g_bOnline;
extern bool g_bPrintingData;

void PrintAllData(void);
bool ProvisionCIK(void);

extern void PrintMac(void);
extern void PrintStats(tStat **psStats);
extern void PrintConnectionHelp(void);
extern bool LocateValidCIK(void);

void
CheckForUserCommands(void);

#endif
