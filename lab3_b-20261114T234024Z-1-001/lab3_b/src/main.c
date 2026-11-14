//AXI GPIO driver
#include "xgpio.h"

//send data over UART
#include "xil_printf.h"

//information about AXI peripherals
#include "xparameters.h"
#include "xuartlite.h"
#include "PmodKYPD.h"

#include <stdio.h>

#define DEFAULT_KEYTABLE "0FED789C456B123A"
PmodKYPD kypd;

int init_keypad(void)
{
	KYPD_begin(&kypd, XPAR_PMODKYPD_0_AXI_LITE_GPIO_BASEADDR);
	KYPD_loadKeyTable(&kypd, (u8*) DEFAULT_KEYTABLE);
}

int main()
{
	XGpio gpio;
	XUartLite uart;

	int num1, num2;
	char input_kb[2];

	char input_kp;
	u16 keystate;
	XStatus status = KYPD_NO_KEY;
	u8 key = 'x';

	u8 ch;
	unsigned int index;

	init_keypad();
	XGpio_Initialize(&gpio, 0);
	XGpio_SetDataDirection(&gpio, 2, 0x00000000); // set LED GPIO channel tristates to All Output

    XUartLite_Initialize(&uart, XPAR_UARTLITE_0_DEVICE_ID);

	xil_printf("\rWelcome to rock-paper-scissors \n");
	xil_printf("\rrock=1 paper=2 scissors=3 \n \n");

	while (1)
	{

		xil_printf("\rPlayer 1 (keyboard) enter number:");

		index = 0;
		// Player 1
		// Receive characters until Enter is pressed
        while (1)
        {
            while (XUartLite_Recv(&uart, &ch, 1) == 0)
            { }

            // handle enter
            if (ch == '\r' || ch == '\n')
            {
            	xil_printf("\n\r");
            	break;
            }

            // handle backspace
            if (ch == '\b' || ch == 127)
            {
            	if (index > 0)
            	{
            		index = 0;
            		input_kb[0] = '\0';
            		xil_printf("\b \b");
            	}
            	continue;
            }

            // accept 1, 2, 3 only
            if (index == 0 && (ch == '1' || ch == '2' || ch == '3'))
            {
            	input_kb[index++] = (char)ch;
            	input_kb[index] = '\0';
            	xil_printf("-");
            }
        }

        // Player 2
		xil_printf("\rPlayer 2 (keypad) enter number:");
        input_kp = 0;
		while (1)
        {
			keystate = KYPD_getKeyStates(&kypd);
			status = KYPD_getKeyPressed(&kypd, keystate, &key);

			if ((char) key == '1' || (char) key == '2' || (char) key == '3') {
				input_kp = (char) key;
				break;
			}
        }
    	xil_printf("-\n");
		key = 'x';

		num1 = input_kb[0] - '0';
		num2 = input_kp - '0';

		// handle game logic
		if (num1 == num2)
		{
			xil_printf("\rDraw! ");
		} else
		{
			switch (num1)
			{
			case 1:
				if (num2 == 2){
					xil_printf("\rPlayer 2 Wins! ");
				} else{
					xil_printf("\rPlayer 1 Wins! ");
				}
				break;
			case 2:
				if (num2 == 3){
					xil_printf("\rPlayer 2 Wins! ");
				} else{
					xil_printf("\rPlayer 1 Wins! ");
				}
				break;
			case 3:
				if (num2 == 1){
					xil_printf("\rPlayer 2 Wins! ");
				} else{
					xil_printf("\rPlayer 1 Wins! ");
				}
				break;
			}
		}
		xil_printf("P1=%d, P2=%d\n", num1, num2);
	}
	return 0;
}
