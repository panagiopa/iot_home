#include <git/cmd_peripherals.h>

void CheckToggleRelayCMD(void)
{
    if(g_ToggleRelayCMD == CMD1_ON)
    {
        uint32_t input = GPIOPinRead(GPIO_PORTL_BASE, GPIO_PIN_0) & GPIO_PIN_0;
        if(input & GPIO_PIN_0)// off rele 1
        {
            //ON rele 1
            GPIOPinWrite(GPIO_PORTL_BASE, GPIO_PIN_0, 0x00);
        }else
        {
            //on buzzer
            GPIOPinWrite(GPIO_PORTL_BASE, GPIO_PIN_0, 0x01);
        }

        g_ToggleRelayCMD = 0x0;
        /* force to write not read */
        g_sToggleRelayCMD.eReadWriteType = READ_WRITE;
    }
    else if(g_ToggleRelayCMD == CMD2_ON)
    {
        uint32_t input = GPIOPinRead(GPIO_PORTL_BASE, GPIO_PIN_1) & GPIO_PIN_1;
        if(input & GPIO_PIN_1)// off rele 1
        {
            //ON rele 1
            GPIOPinWrite(GPIO_PORTL_BASE, GPIO_PIN_1, 0x00);
        }else
        {
            //on buzzer
            GPIOPinWrite(GPIO_PORTL_BASE, GPIO_PIN_1, 0x01 << PIN1);
        }

        g_ToggleRelayCMD = 0x0;
        /* force to write not read */
        g_sToggleRelayCMD.eReadWriteType = READ_WRITE;
    }

    g_rele1state = (GPIOPinRead(GPIO_PORTL_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3))& (0x0F);

}

//*****************************************************************************
//
// Takes a reading from the internal temperature sensor, and updates the
// corresponding global statistics.
//
//*****************************************************************************
void
UpdateInternalTemp(void)
{
    uint32_t pui32ADC0Value[1], ui32TempValueC, ui32TempValueF;

    //
    // Take a temperature reading with the ADC.
    //
    ROM_ADCProcessorTrigger(ADC0_BASE, 3);

    //
    // Wait for the ADC to finish taking the sample
    //
    while(!ROM_ADCIntStatus(ADC0_BASE, 3, false))
    {
    }

    //
    // Clear the interrupt
    //
    ROM_ADCIntClear(ADC0_BASE, 3);

    //
    // Read the analog voltage measurement.
    //
    ROM_ADCSequenceDataGet(ADC0_BASE, 3, pui32ADC0Value);

    //
    // Convert the measurement to degrees Celcius and Fahrenheit, and save to
    // the global state variables.
    //
    ui32TempValueC = ((1475 * 4096) - (2250 * pui32ADC0Value[0])) / 40960;
    g_ui32InternalTempC = ui32TempValueC;
    ui32TempValueF = ((ui32TempValueC * 9) + 160) / 5;
    g_ui32InternalTempF = ui32TempValueF;
}

//*****************************************************************************
//
// Polls the buttons, and updates global state accordingly.
//
//*****************************************************************************
void
UpdateButtons(void)
{
    uint8_t ui8Buttons, ui8ButtonsChanged;

    //
    // Check the current debounced state of the buttons.
    //
    ui8Buttons = ButtonsPoll(&ui8ButtonsChanged,0);

    //
    // If either button has been pressed, record that status to the
    // corresponding global variable.
    //
    if(BUTTON_PRESSED(USR_SW1, ui8Buttons, ui8ButtonsChanged))
    {
        g_ui32SW1Presses++;
    }
    else if(BUTTON_PRESSED(USR_SW2, ui8Buttons, ui8ButtonsChanged))
    {
        g_ui32SW2Presses++;
    }
}

//*****************************************************************************
//
// Turns LEDs on or off based on global state variables.
//
//*****************************************************************************
void
UpdateLEDs(void)
{
    //
    // If either LED's global flag is set, turn that LED on. Otherwise, turn
    // them off.
    //
    if(g_ui32LEDD1)
    {
        ROM_GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, GPIO_PIN_1);
    }
    else
    {
        ROM_GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, 0);
    }

    if(g_ui32LEDD2)
    {
        ROM_GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, GPIO_PIN_0);
    }
    else
    {
        ROM_GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, 0);
    }
}
