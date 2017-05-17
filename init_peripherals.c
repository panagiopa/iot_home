#include "git/init_peripherals.h"

void init_peripherals(void)
{
    uint8_t PIO_L_SWITCH = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);
    /* enable clock for GPIOA -> TURN ON -> TAKE 3 CYCLES ENABLE FIRST*/

    /* APB = Advanced Peripherals Bus
     * AHB = Advanced High-Performance Bus
     * APB for low energy
     *  PORT_L IS IN AHB
     */
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTL_BASE, PIO_L_SWITCH);
    /* OFF RELE */
    ROM_GPIOPinWrite(GPIO_PORTL_BASE, PIO_L_SWITCH, 0XF);

    /* Motion SENSOR INPUT PIN 0 */
    ROM_GPIOPinTypeGPIOInput(GPIO_PORTL_BASE, GPIO_PIN_5);

    /* Configure the ADC photoresistor */

}

//*****************************************************************************
//
// Enables and configures ADC0 to read the internal temperature sensor into
// sample sequencer 3.
//
//*****************************************************************************
void
ConfigureADC0(void)
{
    //
    // Enable clock to ADC0.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

    //
    // Configure ADC0 Sample Sequencer 3 for processor trigger operation.
    //
    ROM_ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);

    //
    // Increase the hold time of this sample sequencer to account for the
    // temperature sensor erratum (ADC#09).
    //
    HWREG(ADC0_BASE + ADC_O_SSTSH3) = 0x4;

    //
    // Configure ADC0 sequencer 3 for a single sample of the temperature
    // sensor.
    //
    ROM_ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_TS | ADC_CTL_IE |
                                 ADC_CTL_END);

    //
    // Enable the sequencer.
    //
    ROM_ADCSequenceEnable(ADC0_BASE, 3);

    //
    // Clear the interrupt bit for sequencer 3 to make sure it is not set
    // before the first sample is taken.
    //
    ROM_ADCIntClear(ADC0_BASE, 3);
}
