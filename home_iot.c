//*****************************************************************************
//
// qs_iot.c - Quickstart application that connects to a cloud server.
//
// Copyright (c) 2013-2017 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 2.1.4.178 of the EK-TM4C1294XL Firmware Package.
//
//*****************************************************************************

#ifdef __cplusplus
extern "C"
{
#endif
#include "git/home_iot.h"
#include "git/conn.h"
#include "git/init_peripherals.h"
#include "git/cmd_peripherals.h"



//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>Internet of Things Quickstart (qs_iot)</h1>
//!
//!
//! This application records various information about user activity on the
//! board, and periodically reports it to a cloud server managed by Exosite.
//! In order to use all of the features of this application, you will need to
//! have an account with Exosite, and make sure that the device you are using
//! is registered to your Exosite profile with its original MAC address from
//! the factory.
//!
//! If you do not yet have an Exosite account, you can create one at
//! http://ti.exosite.com.  The web interface there will help guide you through
//! the account creation process.  There is also information in the Quickstart
//! document that is shipped along with the EK-TM4C1294XL evaluation kit.
//!
//! This application uses a command-line based interface through a virtual COM
//! port on UART 0, with the settings 115,200-8-N-1.  This application also
//! requires a wired Ethernet connection with internet access to perform
//! cloud-connected activities.
//!
//! Once the application is running you should be able to see program output
//! over the virtual COM port, and interact with the command-line.  The command
//! line will allow you to see the information being sent to and from Exosite's
//! servers, change the state of LEDs, and play a game of tic-tac-toe.  If you
//! have internet connectivity issues, need to find your MAC address, or need
//! to re-activate your EK-TM4C1294XL board with Exosite, the command line
//! interface also has options to support these operations.  Type
//! 'help' at the command prompt to see a list of available commands.
//!
//! If your local internet connection requires the use of a proxy server, you
//! will need to enter a command over the virtual COM port terminal before the
//! device will be able to connect to Exosite.  When prompted by the
//! application, type 'setproxy help' for information on how to configure the
//! proxy.  Alternatively, you may uncomment the define statements below for
//! "CUSTOM_PROXY" settings, fill in the correct information for your local
//! http proxy server, and recompile this example.  This will permanently set
//! your proxy as the default connection point.
//!
//
//*****************************************************************************

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif


// #define CUSTOM_PROXY
// #define PROXY_ADDRESS           "your.proxy.address"
// #define PROXY_PORT              80

//*****************************************************************************
//
// Global variables that will be linked to Exosite.
//
//*****************************************************************************
uint32_t g_ui32SW1Presses = 0;
uint32_t g_ui32SW2Presses = 0;
uint32_t g_ui32InternalTempF = 0;
uint32_t g_ui32InternalTempC = 0;
uint32_t g_ui32TimerIntCount = 0;
uint32_t g_ui32SecondsOnTime = 0;
uint32_t g_ui32LEDD1 = 0;
uint32_t g_ui32LEDD2 = 0;
char g_pcLocation[50] = "";
char g_pcContactEmail[100] = "";
char g_pcAlert[140] = "";
uint32_t g_rele1state = 0;
uint32_t g_ToggleRelayCMD = 0;

//*****************************************************************************
//
// Global structures used to interface with Exosite.
//
//*****************************************************************************
tStat g_sSW1Presses =
    {"SW1-presses", &g_ui32SW1Presses, "usrsw1", INT, WRITE_ONLY};

tStat g_sSW2Presses =
    {"SW2-presses", &g_ui32SW2Presses, "usrsw2", INT, WRITE_ONLY};

tStat g_sInternalTempF =
    {"Temp(F)", &g_ui32InternalTempF, 0, INT, WRITE_ONLY};

tStat g_sInternalTempC =
    {"Temp(C)", &g_ui32InternalTempC, "jtemp", INT, WRITE_ONLY};

tStat g_sSecondsOnTime =
    {"Time since reset", &g_ui32SecondsOnTime, "ontime", INT, WRITE_ONLY};

tStat g_sLEDD1 =
    { "LED D1", &g_ui32LEDD1, "ledd1", INT, READ_WRITE};

tStat g_sLEDD2 =
    {"LED D2", &g_ui32LEDD2, "ledd2", INT, READ_WRITE};

tStat g_sLocation =
    {"Location", g_pcLocation, "location", STRING, READ_ONLY};

tStat g_sContactEmail =
    {"Contact Email", g_pcContactEmail, "emailaddr", STRING, READ_WRITE};

tStat g_sAlert =
    {"Alert Message", g_pcAlert, "alert", STRING, NONE};

tStat g_sToggleRelayCMD =
    {"Toggle Relay CMD", &g_ToggleRelayCMD, "togglerelaycmd", INT, READ_WRITE};

tStat g_srele1state =
    {"Rele 1 State", &g_rele1state, "rele1state", INT, WRITE_ONLY};

//*****************************************************************************
//
// Global array of pointers to all tStat structures to be synced with Exosite.
//
//*****************************************************************************
tStat *g_psDeviceStatistics[NUM_STATS]=
{
    &g_sInternalTempC,
    &g_sSecondsOnTime,
    &g_sLocation,
    &g_sAlert,
    &g_srele1state,
    &g_sToggleRelayCMD,
    NULL
};

//*****************************************************************************
//
// Global variable to keep track of the system clock.
//
//*****************************************************************************
uint32_t g_ui32SysClock = 0;

//*****************************************************************************
//
// Global variable to keep track of the IP address.
//
//*****************************************************************************
uint32_t g_ui32IPAddr;

//*****************************************************************************
//
// Global variables to keep track of the error conditions.
//
//*****************************************************************************
enum
{
    ERR_IP_ADDR,
    ERR_CIK,
    ERR_UNKNOWN,
    ERR_NO_ERR
} g_ui32Err = ERR_NO_ERR;


uint32_t SecondsCounterLAMP = 0;
//*****************************************************************************
//
// Interrupt handler for Timer0A.
//
// This function will be called periodically on the expiration of Timer0A It
// performs periodic tasks, such as looking for input on the physical buttons,
// and reporting usage statistics to the cloud.
//
//*****************************************************************************
void
Timer0IntHandler(void)
{
    //
    // Clear the timer interrupt.
    //
    ROM_TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    //
    // Keep track of the number of times this interrupt handler has been
    // called.
    //
    g_ui32TimerIntCount++;

    //
    // Poll the GPIOs for the buttons to check for press events. Update global
    // variables as necessary.
    //
    UpdateButtons();

    if(!g_bPrintingData)
    {
        CheckForUserCommands();
    }


    CheckToggleRelayCMD();

    uint32_t r_motion = GPIOPinRead(GPIO_PORTL_BASE, GPIO_PIN_5);
    if(r_motion && GPIO_PIN_5)
    {

        uint32_t input = GPIOPinRead(GPIO_PORTL_BASE, GPIO_PIN_1) & GPIO_PIN_1;
        if(input & GPIO_PIN_1)// off rele 1
        {
            GPIOPinWrite(GPIO_PORTL_BASE, GPIO_PIN_1, 0x00);
            g_rele1state = (GPIOPinRead(GPIO_PORTL_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3))& (0x0F);
        }
        SecondsCounterLAMP = 0;
    }
    //
    // Once per second, perform the following operations.
    //
    if(!(g_ui32TimerIntCount % APP_TICKS_PER_SEC))
    {
        //
        // Keep track of the total seconds of on-time
        //
        g_ui32SecondsOnTime++;

        //
        // Take a reading from the internal temperature sensor.
        //
        UpdateInternalTemp();

        //
        // Set the LEDs to the correct state.
        //
        UpdateLEDs();

        //
        // Check to see if we have any on-going actions that require the UART
        //
        if(g_bPrintingData)
        {
            //
            // If the user has requested a data print-out, perform that here.
            //
            PrintAllData();
        }


        SecondsCounterLAMP++;
        if(SecondsCounterLAMP==15)
        {
            GPIOPinWrite(GPIO_PORTL_BASE, GPIO_PIN_1, 0x01 << PIN1);
            SecondsCounterLAMP = 0;
            g_rele1state = (GPIOPinRead(GPIO_PORTL_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3))& (0x0F);
        }


    }

    //
    // Make sure the running tally of the number of interrupts doesn't
    // overflow.
    //
    if(g_ui32TimerIntCount == (20 * APP_TICKS_PER_SEC))
    {
        //
        // Reset the interrupt count to zero.
        //
        g_ui32TimerIntCount = 0;

    }

}

//*****************************************************************************
//
// Configures Timer 0 as a general purpose, periodic timer for handling button
// presses.
//
//*****************************************************************************
void
ConfigureTimer0(void)
{
    //
    // Enable the peripherals used by this example.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

    //
    // Configure the two 32-bit periodic timers.
    //
    ROM_TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    ROM_TimerLoadSet(TIMER0_BASE, TIMER_A, g_ui32SysClock / APP_TICKS_PER_SEC);

    //
    // Lower the priority of this interrupt
    //
    ROM_IntPriorityGroupingSet(4);
    ROM_IntPrioritySet(INT_TIMER0A, 0xE0);

    //
    // Setup the interrupts for the timer timeouts.
    //
    ROM_IntEnable(INT_TIMER0A);
    ROM_TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
}
#ifdef __cplusplus
}
#endif
//*****************************************************************************
//
// Main function.
//
//*****************************************************************************
int
main(void)
{
    uint32_t ui32Timeout, ui32CIKRetries, ui32IPAddr;

    //
    // Run from the PLL at 120 MHz.
    //
    g_ui32SysClock = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                             SYSCTL_OSC_MAIN |
                                             SYSCTL_USE_PLL |
                                             SYSCTL_CFG_VCO_480), 120000000);

    //
    // Set the pinout for the board, including required pins for Ethernet
    // operation.
    //
    PinoutSet(1,0);

    //
    // Enable the UART, clear the terminal, and print a brief message.
    //
    UARTStdioConfig(0, 115200, g_ui32SysClock);

    //
    // Configure necessary peripherals.
    //
    ConfigureTimer0();
    ConfigureADC0();
    init_peripherals();
    g_sToggleRelayCMD.eReadWriteType = READ_WRITE;
    //
    // Take an initial reading of the internal temperature
    //
    UpdateInternalTemp();

    //
    // Initialize the buttons
    //
    ButtonsInit();

    //
    // If a proxy has been pre-defined, enable it here.
    //
#ifdef CUSTOM_PROXY
    usprintf(g_pcProxyAddress, PROXY_ADDRESS);
    g_ui16ProxyPort = PROXY_PORT;
    g_bUseProxy = true;
#endif

    //
    // Clear the screen, and print a welcome message.
    //
    UARTprintf("\033[2J\033[H");
    UARTprintf("Welcome to the Connected LaunchPad!!\n");
    UARTprintf("Internet of Things Demo\n");
    UARTprintf("Type \'help\' for help.\n\n");

    //
    // Initialize Exosite layer to allow Exosite-based user commands later.
    //
    Exosite_Init("texasinstruments", "ek-tm4c1294xl", IF_ENET, 0);

    //
    // Start with the assumption that we are not online yet.
    //
    g_bOnline = false;

    //
    // Assume that the IP address is not acquired.
    //
    g_ui32IPAddr = 0;
    ui32IPAddr = 0;
    g_ui32Err = ERR_IP_ADDR;

    //
    // Clear the error variables.
    //
    ui32CIKRetries = 0;

    //
    // Print the MAC address, which users will need to register with Exosite.
    //
    PrintMac();

    //
    // Notify the user that we are obtaining an IP address.
    //
    UARTprintf("Obtaining IP... \n");

    //
    // Loop a few times to make sure that DHCP has time to find an IP.
    //
    for(ui32Timeout = 10; ui32Timeout > 0; ui32Timeout--)
    {
        //
        // Check to see if we have an IP yet.
        //
        if((lwIPLocalIPAddrGet() != 0xffffffff) &&
           (lwIPLocalIPAddrGet() != 0x00000000))
        {
            //
            // Report that we found an IP address.
            //
            UARTprintf("IP Address Acquired: ");
            ui32IPAddr = lwIPLocalIPAddrGet();
            UARTprintf("%d.",ui32IPAddr & 0xFF );
            UARTprintf("%d.",ui32IPAddr >> 8 & 0xFF );
            UARTprintf("%d.",ui32IPAddr >> 16 & 0xFF );
            UARTprintf("%d\n",ui32IPAddr >> 24 & 0xFF );

            //
            // Update the global IP Address variable.  Also update the global
            // error variable.
            //
            g_ui32IPAddr = ui32IPAddr;
            g_ui32Err = ERR_NO_ERR;

            //
            // If we can find and validate a CIK with Exosite, set the flag to
            // indicate have a valid connection to the cloud.
            //
            g_bOnline = LocateValidCIK();

            break;
        }
        else if(ui32Timeout == 0)
        {
            //
            // Alert the user if it takes a long time to find an IP address. An
            // IP address can still be found later, so this is not an
            // indication of failure.
            //
            UARTprintf("No IP address found, continuing \n"
                       "to search in the background\n");
        }

        //
        // Delay a second to allow DHCP to find us an IP address.
        //
        ROM_SysCtlDelay(g_ui32SysClock / 3);
    }

    //
    // If we don't have a valid exosite connection, let the user know that the
    // device is "offline" and not performing any data synchronization with the
    // cloud.
    //
    if(!g_bOnline)
    {
        UARTprintf("Continuing in offline mode.\n\n");
    }

    //
    // Print a prompt
    //
    UARTprintf("\n> ");

    //
    // Enable interrupts and start the timer. This will enable the UART console
    // input, and also enable updates to the various cloud-enabled variables.
    //
    ROM_IntMasterEnable();
    ROM_TimerEnable(TIMER0_BASE, TIMER_A);

    //
    // Main application loop.
    //
    while(1)
    {
        //
        // Only run the following loop if we have a valid connection to
        // Exosite.
        //
        if(g_bOnline)
        {
            //
            // Attempt to sync data with Exosite
            //
            if(SyncWithExosite(g_psDeviceStatistics))
            {
                //
                // If the sync is successful, reset the "retries" count to zero
                //
                g_ui32LinkRetries = 0;
            }
            else if(Exosite_StatusCode() == EXO_STATUS_NOAUTH)
            {
                //
                // Failed for having an old CIK.  Flush the UART output, and
                // stop any data-printing operation.
                //
                g_bPrintingData = 0;
                UARTFlushTx(0);

                //
                // Alert the user of the expired CIK.
                //
                UARTprintf("\nCIK no longer valid.\n");
                UARTprintf("The application will automatically try to ");
                UARTprintf("acquire a new CIK.\n");
                UARTprintf("\n> ");

                //
                // We did connect to Exosite, so the link is still valid, but
                // data syncing will not work.  Do not increment the number of
                // link retries, but do consider the board "offline" for data
                // syncing.
                //
                g_bOnline = false;
                g_ui32Err = ERR_CIK;
                ui32CIKRetries = 0;

                //
                // Put the LEDs in error indicating mode.  Turn LED D1 off.
                // LED D2 is used as error indicator in this case.
                //
                g_ui32LEDD1 = 0;
            }
            else
            {
                //
                // Sync failed for some other reason.  Since we don't know the
                // reason for this failure, try to sync with Exosite server
                // again.  Increment the failure counter to aid in debug.
                //
                g_ui32LinkRetries++;
            }

            //
            // Check if IP address has changed.
            //
            ui32IPAddr = lwIPLocalIPAddrGet();
            if(g_ui32IPAddr != ui32IPAddr)
            {
                //
                // Yes, then let the app know that link is lost and we are
                // offline.
                //
                g_bOnline = false;
                g_ui32Err = ERR_IP_ADDR;

                //
                // Alert the user that IP Address is lost.
                //
                g_bPrintingData = 0;
                UARTFlushTx(0);
                UARTprintf("\nIP Address lost/changed.\n");

                //
                // Put the LEDs in error indicating mode.  Turn LED D2 off.
                // LED D1 is used as the error indicator in this case.
                //
                g_ui32LEDD2 = 0;
            }
        }
        else
        {
            //
            // If we are here it means that connection to Exosite is lost.
            // Handle the different cases that could have caused this.
            //
            switch(g_ui32Err)
            {
                case ERR_IP_ADDR:
                {
                    //
                    // Check if IP address has been acquired.
                    //
                    ui32IPAddr = lwIPLocalIPAddrGet();
                    if((ui32IPAddr == 0xffffffff) || (ui32IPAddr == 0x00000000))
                    {
                        //
                        // No - Delay a second to allow DHCP to find an IP
                        // address.
                        //
                        ROM_SysCtlDelay(g_ui32SysClock / 3);

                        //
                        // Change the state of LED D1 so that it blinks,
                        // indicating that IP Address is lost.
                        //
                        g_ui32LEDD1 ^= 1;
                        break;
                    }

                    //
                    // Report the IP address to the user.
                    //
                    UARTprintf("IP Address Acquired: ");
                    UARTprintf("%d.",ui32IPAddr & 0xFF );
                    UARTprintf("%d.",ui32IPAddr >> 8 & 0xFF );
                    UARTprintf("%d.",ui32IPAddr >> 16 & 0xFF );
                    UARTprintf("%d\n",ui32IPAddr >> 24 & 0xFF );

                    //
                    // Update the global IP Address variable.
                    //
                    g_ui32IPAddr = ui32IPAddr;

                    //
                    // Check if the CIK is valid.  This also helps in verifying
                    // connection with Exosite.
                    //
                    g_bOnline = LocateValidCIK();
                    if(!g_bOnline)
                    {
                        //
                        // Failed to connect to Exosite.  Assume that CIK is
                        // not valid.
                        //
                        g_ui32Err = ERR_CIK;
                        ui32CIKRetries = 0;
                        break;
                    }

                    //
                    // Print a prompt
                    //
                    UARTprintf("\n> ");

                    //
                    // Reset the necessary variables to inform the application
                    // that the error is cleared.
                    //
                    g_ui32Err = ERR_NO_ERR;
                    g_ui32LinkRetries = 0;

                    break;
                }

                case ERR_CIK:
                {
                    //
                    // Attempt to acquire a new CIK from Exosite.
                    //
                    if(ProvisionCIK())
                    {
                        //
                        // Success - notify the application that we are
                        // connected to Exosite.  Also reset the necessary
                        // error variables.
                        //
                        g_bOnline = true;
                        g_ui32Err = ERR_NO_ERR;
                        ui32CIKRetries = 0;

                        break;
                    }

                    //
                    // Since we failed to acquire a new CIK, try a few times
                    // before giving up.
                    //
                    ui32CIKRetries++;
                    if(ui32CIKRetries > 5)
                    {
                        //
                        // Report to the user.
                        //
                        UARTprintf("\nCIK no longer valid.  Tried %d times ",
                                   ui32CIKRetries);
                        UARTprintf("to acquire new CIK but failed.\n");
                        UARTprintf("Log in to exosite to check on the ");
                        UARTprintf("status of your device.\n");

                        //
                        // Failed to acquire new CIK after trying a few times.
                        // So stop trying to acquire new CIK.
                        //
                        g_ui32Err = ERR_UNKNOWN;
                        ui32CIKRetries = 0;
                    }

                    //
                    // Change the state of LED D2 so that it blinks, indicating
                    // that CIK is no longer valid.
                    //
                    g_ui32LEDD2 ^= 1;
                    break;
                }

                case ERR_UNKNOWN:
                {
                    //
                    // Report to the user.
                    //
                    UARTprintf("\nFailed to connect to Exosite due to ");
                    UARTprintf("unknown reason.\n");

                    //
                    // Display IP address.
                    //
                    UARTprintf("IP Address: ");
                    ui32IPAddr = lwIPLocalIPAddrGet();
                    UARTprintf("%d.",ui32IPAddr & 0xFF );
                    UARTprintf("%d.",ui32IPAddr >> 8 & 0xFF );
                    UARTprintf("%d.",ui32IPAddr >> 16 & 0xFF );
                    UARTprintf("%d\n",ui32IPAddr >> 24 & 0xFF );

                    UARTprintf("Trying to sync with Exosite Server again.\n");
                    UARTprintf("\n> ");

                    //
                    // Set the appropriate flags to retry syncing with Exosite
                    // server.
                    //
                    g_bOnline = true;
                    g_ui32Err = ERR_NO_ERR;

                    break;
                }

                case ERR_NO_ERR:
                default:
                {
                    break;
                }
            }
        }
    }

}

