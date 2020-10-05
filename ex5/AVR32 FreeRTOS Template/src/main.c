#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gpio.h>
#include <power_clocks_lib.h>
#include <sysclk.h>
#include <compiler.h>
#include <board.h>
#include <stdio_usb.h>


#define CONFIG_USART_IF (AVR32_USART2)

#include "FreeRTOS.h"
#include "task.h"

#define TEST_A      AVR32_PIN_PA31
#define RESPONSE_A  AVR32_PIN_PA30
#define TEST_B      AVR32_PIN_PA29
#define RESPONSE_B  AVR32_PIN_PA28
#define TEST_C      AVR32_PIN_PA27
#define RESPONSE_C  AVR32_PIN_PB00

// PART 3 code
struct responseTaskArgs {
		struct{
			uint32_t test;
			uint32_t response;
			} pin;
			// other args
		uint32_t useBusyWait; // task C: {{TEST_C, RESPONSE_C}, 1}. 0 for all other tasks
	};


void busy_delay_ms(int delay){
    for(; delay != 0; delay--){
        for(int i = 0; i < 2108; i++){
            asm volatile ("" ::: "memory");
        }
    }
}

void busy_delay_short(void){
    for(int i = 0; i < 10; i++){
        asm volatile ("" ::: "memory");
    }
}

void init(){
	board_init();

    gpio_configure_pin(TEST_A, GPIO_DIR_INPUT);
    gpio_configure_pin(TEST_B, GPIO_DIR_INPUT);
    gpio_configure_pin(TEST_C, GPIO_DIR_INPUT);
    gpio_configure_pin(RESPONSE_A, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
    gpio_configure_pin(RESPONSE_B, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
    gpio_configure_pin(RESPONSE_C, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);

	pcl_switch_to_osc(PCL_OSC0, FOSC0, OSC0_STARTUP);

	stdio_usb_init(&CONFIG_USART_IF);

    #if defined(__GNUC__) && defined(__AVR32__)
	    setbuf(stdout, NULL);
	    setbuf(stdin,  NULL);
    #endif
}

// TASK A - Create two tasks that periodically toggle two LEDs.

static void taskFn1(void* args){ // toggle LED0 every 200ms
	const portTickType delay = 200 / portTICK_RATE_MS;

    int i = 0;

	while(1){
		gpio_toggle_pin(LED0_GPIO);
		printf("tick %d\n", i++);

		vTaskDelay(delay);
    // void vTaskDelay( portTickType xTicksToDelay ) PRIVILEGED_FUNCTION;
	}
}

static void taskFn2(void* args){ // toggle LED1 every 500ms
	const portTickType delay = 500 / portTICK_RATE_MS;

	//int i = 0;

	while(1){
		gpio_toggle_pin(LED1_GPIO);
		//printf("tick %d\n", i++);

		vTaskDelay(delay);
	}
}

// PART 3 code
static void responseTask(void* args){
	struct responseTaskArgs a = *(struct responseTaskArgs*)args;
	while(1){
		if(gpio_pin_is_low(a.pin.test)){
      // TASK C - Make task C wait 3ms before responding
			if (a.useBusyWait) {
				busy_delay_ms(3);
			}
			gpio_clr_gpio_pin(a.pin.response);
			//vTaskDelay(1);
			busy_delay_short();
			gpio_set_gpio_pin(a.pin.response);
		}
	vTaskDelay(1);
	}
}


int main(){
	init();

	//xTaskCreate(taskFn, "", 1024, NULL, tskIDLE_PRIORITY + 1, NULL);

// TASK B - Create three tasks, one each for test A, B and C
xTaskCreate(responseTask, "", 1024, (&(struct responseTaskArgs){{TEST_A, RESPONSE_A}, 0}), tskIDLE_PRIORITY + 2, NULL);
xTaskCreate(responseTask, "", 1024, (&(struct responseTaskArgs){{TEST_B, RESPONSE_B}, 0}), tskIDLE_PRIORITY + 2, NULL);
xTaskCreate(responseTask, "", 1024, (&(struct responseTaskArgs){{TEST_C, RESPONSE_C}, 1}), tskIDLE_PRIORITY + 1, NULL);

	// Start the real-time scheduler, anything after this will not run.
	vTaskStartScheduler();

}
