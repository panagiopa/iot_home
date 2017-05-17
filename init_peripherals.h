#ifndef __INIT_PERIPHERALS_H_
#define __INIT_PERIPHERALS_H__


#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_adc.h"
#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/uart.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include "utils/cmdline.h"
#include "utils/uartstdio.h"
#include "utils/ustdlib.h"
#include "utils/lwiplib.h"
#include "drivers/pinout.h"
#include "drivers/buttons.h"
#include "drivers/exosite_hal_lwip.h"
#include "drivers/eth_client_lwip.h"
#include "exosite.h"
#include "stats.h"
#include "requests.h"
#include "commands.h"

#define NORMAL 0
#define NORMAL_MOTION_OFF 1
#define ALARM 2

#define THEFT_ALARM_MOTION 1    // MOTION

#define RELE_ON 0x0U
#define RELE_OFF 0x1U

#define PIN0 0x00U
#define PIN1 0x01U
#define PIN2 0x02U
#define PIN3 0x03U
#define PIN4 0x04U
#define PIN5 0x05U
#define PIN6 0x06U
#define PIN7 0x07U




void
ConfigureADC0(void);

void init_peripherals(void);








#endif
