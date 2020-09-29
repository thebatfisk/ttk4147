#include <asf.h>
#include <board.h>
#include <gpio.h>
#include <sysclk.h>
#include "busy_delay.h"

#define CONFIG_USART_IF (AVR32_USART2)

// defines for BRTT interface
#define TEST_A AVR32_PIN_PA31
#define RESPONSE_A AVR32_PIN_PA30
#define TEST_B AVR32_PIN_PA29
#define RESPONSE_B AVR32_PIN_PA28
#define TEST_C AVR32_PIN_PA27
#define RESPONSE_C AVR32_PIN_PB00

enum test_types
{
    TEST_TYPE_A = 0,
    TEST_TYPE_B = 1,
    TEST_TYPE_C = 2,
};

int8_t test_type = -1;

__attribute__((__interrupt__)) static void interrupt_J3(void);

// void brtt_interrupts_init()
// {
//     /*
// 	gpio_enable_gpio_pin(RESPONSE_A);
// 	gpio_enable_gpio_pin(RESPONSE_B);
// 	gpio_enable_gpio_pin(RESPONSE_C);
// 	gpio_enable_gpio_pin(TEST_A);
// 	gpio_enable_gpio_pin(TEST_B);
// 	gpio_enable_gpio_pin(TEST_C);*/
//     gpio_configure_pin(RESPONSE_A, GPIO_DIR_OUTPUT);
//     gpio_configure_pin(RESPONSE_B, GPIO_DIR_OUTPUT);
//     gpio_configure_pin(RESPONSE_C, GPIO_DIR_OUTPUT);
//     gpio_configure_pin(TEST_A, GPIO_DIR_INPUT | GPIO_INIT_HIGH);
//     gpio_configure_pin(TEST_B, GPIO_DIR_INPUT | GPIO_INIT_HIGH);
//     gpio_configure_pin(TEST_C, GPIO_DIR_INPUT | GPIO_INIT_HIGH);
//     gpio_enable_pin_interrupt(TEST_A, GPIO_FALLING_EDGE);
//     gpio_enable_pin_interrupt(TEST_B, GPIO_FALLING_EDGE);
//     gpio_enable_pin_interrupt(TEST_C, GPIO_FALLING_EDGE);
// }

void init()
{
    sysclk_init();
    board_init();
    busy_delay_init(BOARD_OSC0_HZ);

    cpu_irq_disable();
    INTC_init_interrupts();
    // Added by us
    // brtt_interrupts_init();
    // ****************
    INTC_register_interrupt(&interrupt_J3, AVR32_GPIO_IRQ_3, AVR32_INTC_INT1);
    cpu_irq_enable();

    stdio_usb_init(&CONFIG_USART_IF);

#if defined(__GNUC__) && defined(__AVR32__)
    setbuf(stdout, NULL);
    setbuf(stdin, NULL);
#endif
}

__attribute__((__interrupt__)) static void interrupt_J3(void)
{
    if (gpio_get_pin_interrupt_flag(TEST_A))
    {
        test_type = TEST_TYPE_A;
        gpio_clear_pin_interrupt_flag(TEST_A);
    } else if (gpio_get_pin_interrupt_flag(TEST_B))
    {
        test_type = TEST_TYPE_B;
        gpio_clear_pin_interrupt_flag(TEST_B);
    } else if (gpio_get_pin_interrupt_flag(TEST_C))
    {
        test_type = TEST_TYPE_C;
        gpio_clear_pin_interrupt_flag(TEST_C);
    }
}

int main(void)
{
    init();

    /* Initial code */

    while (1)
    {
        gpio_toggle_pin(LED0_GPIO);

        printf("tick\n");

        busy_delay_ms(500);
    }

    /* Task A */

    // Initialization
    // gpio_enable_gpio_pin(RESPONSE_A);
    // gpio_enable_gpio_pin(TEST_A);
    // gpio_configure_pin(RESPONSE_A, GPIO_DIR_OUTPUT | GPIO_INIT_LOW);
    // gpio_configure_pin(TEST_A, GPIO_DIR_INPUT | GPIO_INIT_HIGH);

    // while (!gpio_get_pin_value(TEST_A) == 0)
    // {
    // }

    // gpio_set_pin_low(RESPONSE_A);
    // busy_delay_us(5);
    // gpio_set_pin_high(RESPONSE_A);

    /* Task B */

    // Initialization
    // gpio_enable_gpio_pin(RESPONSE_A);
    // gpio_enable_gpio_pin(RESPONSE_B);
    // gpio_enable_gpio_pin(RESPONSE_C);
    // gpio_enable_gpio_pin(TEST_A);
    // gpio_enable_gpio_pin(TEST_B);
    // gpio_enable_gpio_pin(TEST_C);
    // gpio_configure_pin(RESPONSE_A, GPIO_DIR_OUTPUT);
    // gpio_configure_pin(RESPONSE_B, GPIO_DIR_OUTPUT);
    // gpio_configure_pin(RESPONSE_C, GPIO_DIR_OUTPUT);
    // gpio_configure_pin(TEST_A, GPIO_DIR_INPUT | GPIO_INIT_HIGH);
    // gpio_configure_pin(TEST_B, GPIO_DIR_INPUT | GPIO_INIT_HIGH);
    // gpio_configure_pin(TEST_C, GPIO_DIR_INPUT | GPIO_INIT_HIGH);

    // while (1)
    // {
    //     if (gpio_get_pin_value(TEST_A) == 0)
    //     {
    //         gpio_set_pin_low(RESPONSE_A);
    //         busy_delay_us(5);
    //         gpio_set_pin_high(RESPONSE_A);
    //     } else if (gpio_get_pin_value(TEST_B) == 0)
    //     {
    //         gpio_set_pin_low(RESPONSE_B);
    //         busy_delay_us(5);
    //         gpio_set_pin_high(RESPONSE_B);
    //     } else if (gpio_get_pin_value(TEST_C) == 0)
    //     {
    //         gpio_set_pin_low(RESPONSE_C);
    //         busy_delay_us(5);
    //         gpio_set_pin_high(RESPONSE_C);
    //     }
    // }

    /* Task C */

    // while (1)
    // {
    //     if (test_type == TEST_TYPE_A)
    //     {
    //         gpio_set_pin_low(RESPONSE_A);
    //         busy_delay_us(5);
    //         gpio_set_pin_high(RESPONSE_A);
    //         test_type = -1;
    //     } else if (test_type == TEST_TYPE_B)
    //     {
    //         gpio_set_pin_low(RESPONSE_B);
    //         // Task D
    //         // busy_delay_us(5);
    //         // busy_delay_us(100);
    //         // *******************
    //         gpio_set_pin_high(RESPONSE_B);
    //         test_type = -1;
    //     } else if (test_type == TEST_TYPE_C)
    //     {
    //         gpio_set_pin_low(RESPONSE_C);
    //         busy_delay_us(5);
    //         gpio_set_pin_high(RESPONSE_C);
    //         test_type = -1;
    //     }
    // }
}
