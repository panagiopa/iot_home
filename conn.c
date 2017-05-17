#include "git/conn.h"

//*****************************************************************************
//
// Flags to keep track of application state.
//
//*****************************************************************************
bool g_bPrintingData = false;
volatile bool g_bOnline = false;

//*****************************************************************************
//
// Input buffer for the command line interpreter.
//
//*****************************************************************************
char g_cInput[APP_INPUT_BUF_SIZE];

uint32_t g_ui32LinkRetries = 0;

//*****************************************************************************
//
// Given a list of statistics, prints each item to the UART.
//
//*****************************************************************************
void
PrintStats(tStat **psStats)
{
    uint32_t ui32Index;
    char pcStatValue[256];
    char *pcStatName;

    //
    // Loop over all statistics in the list.
    //
    for(ui32Index = 0; psStats[ui32Index] != NULL; ui32Index++)
    {
        if(psStats[ui32Index]->pcName)
        {
            //
            // For each statistic, print the name and current value to the UART.
            //
            pcStatName = psStats[ui32Index]->pcName;
            StatPrintValue(psStats[ui32Index], pcStatValue);

            UARTprintf("%25s= %s\n", pcStatName, pcStatValue);
        }
    }
}

//*****************************************************************************
//
// Prints the current MAC address to the UART.
//
//*****************************************************************************
void
PrintMac(void)
{
    uint8_t ui8Idx;
    uint8_t pui8MACAddr[6];

    //
    // Get the MAC address from the Ethernet Client layer.
    //
    EthClientMACAddrGet(pui8MACAddr);

    UARTprintf("Current MAC: ");

    //
    // Extract each pair of characters and print them to the UART.
    //
    for(ui8Idx = 0; ui8Idx < 6; ui8Idx++)
    {
        UARTprintf("%02x", pui8MACAddr[ui8Idx]);
    }

    UARTprintf("\n");
}

//*****************************************************************************
//
// This function prints a list of local statistics for this board.
//
//*****************************************************************************
void
PrintAllData(void)
{
    char cExositeCIK[CIK_LENGTH];

    if(UARTPeek('\r') != -1)
    {
        g_bPrintingData = false;

        //
        // Get a user command back
        //
        UARTgets(g_cInput, APP_INPUT_BUF_SIZE);

        //
        // Print a prompt
        //
        UARTprintf("\n> ");

        return;
    }

    UARTprintf("\033[2J\033[H");
    UARTprintf("Welcome to the Connected LaunchPad!!\n");
    UARTprintf("Internet of Things Demo\n");
    UARTprintf("Type 'help' for help.\n\n");

    //
    // Print out the MAC address for reference
    //
    PrintMac();

    //
    // Check to see if we already have a CIK, and print it to the UART
    //
    if(Exosite_GetCIK(cExositeCIK))
    {
        UARTprintf("Current CIK: %s\n", cExositeCIK);
    }
    else
    {
        UARTprintf("No CIK found. Connect to Exosite to obtain one.\n");
    }

    //
    // Check to see how many times (if any) we've failed to connect to the
    // server.
    //
    if((g_ui32LinkRetries == 0) && g_bOnline)
    {
        //
        // For zero failures, report a "Link OK"
        //
        UARTprintf("Link Status: OK\n");
    }
    else if((g_ui32LinkRetries < MAX_SYNC_RETRIES) && g_bOnline)
    {
        //
        // For the first few failures, report that we are trying to
        // re-establish a link.
        //
        UARTprintf("Link Status: Lost (Retries: %d)\n", g_ui32LinkRetries);
    }
    else
    {
        //
        // If we have exceeded the maximum number of retries, show status as
        // offline.
        //
        UARTprintf("Link Status: Offline");
    }

    //
    // Print some header text.
    //
    UARTprintf("\nCollected Statistics\n");
    UARTprintf("--------------------\n");

    PrintStats(g_psDeviceStatistics);

    UARTprintf("\nPress Enter to return to the command prompt...\n");

    UARTFlushTx(0);

    return;
}

//*****************************************************************************
//
// Prints a help message to the UART to help with troubleshooting Exosite
// connection issues.
//
//*****************************************************************************
void
PrintConnectionHelp(void)
{
    UARTprintf("Troubleshooting Exosite Connection:\n\n");

    UARTprintf("    + Make sure you are connected to the internet.\n\n");

    UARTprintf("    + Make sure you have created an Exosite profile.\n\n");

    UARTprintf("    + Make sure you have a \"Connected Launchpad\" device\n");
    UARTprintf("      created in your Exosite profile.\n\n");

    UARTprintf("    + Make sure your that your board's MAC address is\n");
    UARTprintf("      correctly registered with your exosite profile.\n\n");

    UARTprintf("    + If you have a CIK, make sure it matches the CIK for\n");
    UARTprintf("      this device in your online profile with Exosite.\n\n");

    UARTprintf("    + If you have a proxy, make sure to configure it using\n");
    UARTprintf("      this terminal. Type 'setproxy help' to get started.\n");
    UARTprintf("      Once the proxy is set, type 'activate' to obtain a\n");
    UARTprintf("      new CIK, or 'connect' to connect to exosite using an\n");
    UARTprintf("      existing CIK.\n\n");

    UARTprintf("    + Make sure your device is available for provisioning.\n");
    UARTprintf("      If you are not sure that provisioning is enabled,\n");
    UARTprintf("      check the Read Me First documentation or the online\n");
    UARTprintf("      exosite portal for more information.\n\n");
}

//*****************************************************************************
//
// Attempts to find a CIK in the EEPROM. Reports the status of this operation
// to the UART.
//
//*****************************************************************************
bool
GetEEPROMCIK(void)
{
    char pcExositeCIK[50];

    //
    // Try to read the CIK from EEPROM, and alert the user based on what we
    // find.
    //
    if(Exosite_GetCIK(pcExositeCIK))
    {
        //
        // If a CIK is found, continue on to make sure that the CIK is valid.
        //
        UARTprintf("CIK found in EEPROM storage.\n\nCIK: %s\n\n",
                   pcExositeCIK);
    }
    else
    {
        //
        // If a CIK was not found, return immediately and indicate the failure.
        //
        UARTprintf("No CIK found in EEPROM.\n");
        return 0;
    }

    UARTprintf("Connecting to Exosite...\r");

    //
    // If a CIK was found, try to sync with Exosite. This should tell us if
    // the CIK is valid or not.
    //
    if(SyncWithExosite(g_psDeviceStatistics))
    {
        //
        // If the sync worked, the CIK is valid. Alert the caller.
        //
        UARTprintf("Connected! Type 'stats' to see data for this board.");
        return 1;
    }
    else
    {
        //
        // If the sync failed, the CIK is probably invalid, so pass the error
        // back to the caller.
        //
        UARTprintf("Initial sync failed. CIK may be invalid.\n");
        return 0;
    }
}

//*****************************************************************************
//
// Attempts to provision a new CIK through a request to Exosite's servers. This
// should be used when Exosite's CIK does not match the CIK for this device, or
// when a CIK is not found in EEPROM.
//
//*****************************************************************************
bool
ProvisionCIK(void)
{
    uint32_t ui32Idx;
    char pcExositeCIK[50];

    //
    // If we get here, no CIK was found in EEPROM storage. We may need to
    // obtain a CIK from the server.
    //
    UARTprintf("Connecting to exosite to obtain a new CIK... \n");

    //
    // Try to activate with Exosite a few times. If we succeed move on with the
    // new CIK. Otherwise, fail.
    //
    for(ui32Idx = 0; ui32Idx < 1; ui32Idx++)
    {
        if(Exosite_Activate())
        {
            //
            // If exosite gives us a CIK, send feedback to the user
            //
            UARTprintf("CIK acquired!\n\n");

            if(Exosite_GetCIK(pcExositeCIK))
            {
                UARTprintf("CIK: %s\n\n", pcExositeCIK);
                UARTprintf("Connected! ");
                UARTprintf("Type 'stats' to see data for this board.");
            }
            else
            {
                //
                // This shouldn't ever happen, but print an error message in
                // case it does.
                //
                UARTprintf("ERROR reading new CIK from EEPROM.\n");
            }

            //
            // Return "true" indicating that we found a valid CIK.
            //
            return true;
        }
        else
        {
            //
            // If the activation fails, wait at least one second before
            // retrying.
            //
            //ROM_SysCtlDelay(g_ui32SysClock/3);
            if(Exosite_StatusCode() == EXO_STATUS_CONFLICT)
            {
                //
                // This can occur if the MAC address for this board has already
                // been activated, and the device has not been re-enabled for a
                // new CIK.
                //
                UARTprintf("\nExosite reported that this device is not\n");
                UARTprintf("available for provisioning. Check to make sure\n");
                UARTprintf("that you have the correct MAC address, and that\n");
                UARTprintf("this device is enabled for provisioning in your\n");
                UARTprintf("Exosite profile.\n\n");

                return false;
            }
        }
    }

    //
    // Exosite didn't respond, so let the user know.
    //
    UARTprintf("No CIK could be obtained.\n\n");

    PrintConnectionHelp();

    //
    // Return "false", indicating that no CIK was found.
    //
    return false;
}

//*****************************************************************************
//
// Attempts to provision a new CIK through a request to Exosite's servers. This
// should be used when Exosite's CIK does not match the CIK for this device, or
// when a CIK is not found in EEPROM.
//
//*****************************************************************************
bool
LocateValidCIK(void)
{
    //
    // Try to obtain a valid CIK.
    //
    UARTprintf("Locating CIK... ");

    //
    // Check the EEPROM for a valid CIK first. If none can be found
    // there, try to provision a CIK from exosite. If we can obtain a
    // CIK, make sure to set the global state variable that indicates
    // that we can connect to exosite.
    //
    if(GetEEPROMCIK())
    {
        return true;
    }
    else if(ProvisionCIK())
    {
        return true;
    }
    else
    {
        //
        // If both cases above fail, return false, indicating that we did not
        // find a CIK.
        //
        return false;
    }
}

//*****************************************************************************
//
// Prompts the user for a command, and blocks while waiting for the user's
// input. This function will return after the execution of a single command.
//
//*****************************************************************************
void
CheckForUserCommands(void)
{
    int iStatus;

    //
    // Peek to see if a full command is ready for processing
    //
    if(UARTPeek('\r') == -1)
    {
        //
        // If not, return so other functions get a chance to run.
        //
        return;
    }

    //
    // If we do have commands, process them immediately in the order they were
    // received.
    //
    while(UARTPeek('\r') != -1)
    {
        //
        // Get a user command back
        //
        UARTgets(g_cInput, APP_INPUT_BUF_SIZE);

        //
        // Process the received command
        //
        iStatus = CmdLineProcess(g_cInput);

        //
        // Handle the case of bad command.
        //
        if(iStatus == CMDLINE_BAD_CMD)
        {
            UARTprintf("Bad command!\n");
        }

        //
        // Handle the case of too many arguments.
        //
        else if(iStatus == CMDLINE_TOO_MANY_ARGS)
        {
            UARTprintf("Too many arguments for command processor!\n");
        }
    }

    //
    // Print a prompt
    //
    UARTprintf("\n> ");

}
