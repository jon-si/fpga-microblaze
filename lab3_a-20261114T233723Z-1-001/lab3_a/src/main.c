//AXI GPIO driver
#include "xgpio.h"

//send data over UART
#include "xil_printf.h"

//information about AXI peripherals
#include "xparameters.h"
#include "xuartlite.h"

#include <stdio.h>

int parse_input(char *input, int *num1, int *num2)
{
    int i = 0;
    int sign1 = 1, sign2 = 1;
    int n1 = 0, n2 = 0;

    // Parse first number's optional sign
    if (input[i] == '-') {
        sign1 = -1;
        i++;
    }

    // Read first number
    if (input[i] < '0' || input[i] > '9')
        return 0;

    while (input[i] >= '0' && input[i] <= '9') {
        n1 = n1 * 10 + (input[i] - '0');
        i++;
    }

    // Require '/'
    if (input[i] != '/')
        return 0;
    i++;

    // Parse second number's optional sign
    if (input[i] == '-') {
        sign2 = -1;
        i++;
    }

    // Read second number
    if (input[i] < '0' || input[i] > '9')
        return 0;

    while (input[i] >= '0' && input[i] <= '9') {
        n2 = n2 * 10 + (input[i] - '0');
        i++;
    }

    // Reject trailing characters
    if (input[i] != '\0')
        return 0;

    *num1 = n1 * sign1;
    *num2 = n2 * sign2;

    return 1;
}


int main()
{
	XGpio gpio;
	XUartLite uart;

	u32 led = 0;
	int num1, num2, result;
	char input[32];

	u8 ch;
	unsigned int index;

	XGpio_Initialize(&gpio, 0);
	XGpio_SetDataDirection(&gpio, 2, 0x00000000); // set LED GPIO channel tristates to All Output

    XUartLite_Initialize(&uart, XPAR_UARTLITE_0_DEVICE_ID);
    int last_was_cr = 0;
	while (1)
	{
		xil_printf("\rPlease enter two numbers delimited by '/':");

		index = 0;

		// Receive characters until Enter is pressed
        while (1)
        {
            while (XUartLite_Recv(&uart, &ch, 1) == 0)
            {
                // Wait for UART data
            }

            xil_printf("%c", ch);

            if (ch == '\n' && last_was_cr)
			{
				last_was_cr = 0;
				continue;
			}

            if (ch == '\r' || ch == '\n')
            {
                last_was_cr = (ch == '\r');
            	xil_printf("\n\r");
            	break;
            }

            last_was_cr = 0;

            if (ch == '\b' || ch == 127)
            {
                if (index > 0)
                {
                    index--;
                }
                continue;
            }

            if (index < sizeof(input) - 1)
            {
            	input[index++] = (char)ch;
            }
        }

        input[index] = '\0';

		if (parse_input(input, &num1, &num2)){
			result = num1 * num2;
			if (result > 100) {
				led = 0xFFFFFFFF;
			} else {
				led = 0x00000000;
			}
			xil_printf("%d \r\n", result);
		} else {
			xil_printf("Invalid input. Example: 8/6\r\n");
		}

		XGpio_DiscreteWrite(&gpio, 2, led);

	}
	return 0;
}
