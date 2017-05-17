#ifndef __CONN_H_
#define __CONN_H__

#include <stdint.h>
#include <stdbool.h>
#include "stats.h"
#include "git/home_iot.h"
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





#endif
