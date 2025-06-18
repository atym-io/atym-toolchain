/*
 * Copyright (C) 2025 Atym Incorporated. All rights reserved.
 */
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include "ocre_api.h"

#define LED0_PORT 7
#define LED0 7
#define BUTTON_PORT 2
#define BUTTON_0 13

// Timer callback function - can be any function you want
static void my_timer_function(void)
{
    printf("Timer triggered - blinking LED!\n");
    static bool led_state = false;
    static int blink_count = 0;

    // Active-low: RESET (low) = ON, SET (high) = OFF
    int ret = led_state ? ocre_gpio_pin_set(LED0_PORT, LED0, OCRE_GPIO_PIN_RESET) // ON
                        : ocre_gpio_pin_set(LED0_PORT, LED0, OCRE_GPIO_PIN_SET);  // OFF
    if (ret != 0)
    {
        printf("Failed to set LED: %d\n", ret);
    }
    else
    {
        printf("LED state set to %s (logical %d, count %d)\n", led_state ? "ON" : "OFF", led_state, ++blink_count);
    }
    led_state = !led_state;
}

// GPIO callback function - can be any function you want
static void my_button_function(void)
{
    printf("Button pressed - blinking LED!\n");
    static bool led_state = false;
    static int blink_count = 0;

    // Active-low: RESET (low) = ON, SET (high) = OFF
    int ret = led_state ? ocre_gpio_pin_set(LED0_PORT, LED0, OCRE_GPIO_PIN_RESET) // ON
                        : ocre_gpio_pin_set(LED0_PORT, LED0, OCRE_GPIO_PIN_SET);  // OFF
    if (ret != 0)
    {
        printf("Failed to set LED: %d\n", ret);
    }
    else
    {
        printf("LED state set to %s (logical %d, count %d)\n", led_state ? "ON" : "OFF", led_state, ++blink_count);
    }
    led_state = !led_state;
}

// Example of another function you could use
static void my_other_function(void)
{
    printf("Alternative function called!\n");
    // Do something else here
}

int main(void)
{
    const int timer_id = 1;
    int interval_ms = 3000;
    bool is_periodic = true;

    printf("\
    ██████  ██      ██ ███    ██ ██   ██ ██    ██ \n\
    ██   ██ ██      ██ ████   ██ ██  ██   ██  ██  \n\
    ██████  ██      ██ ██ ██  ██ █████     ████   \n\
    ██   ██ ██      ██ ██  ██ ██ ██  ██     ██    \n\
    ██████  ███████ ██ ██   ████ ██   ██    ██    \n");
    printf("Blinky Test with Generic Callbacks\n");

    // Initialize GPIO
    if (ocre_gpio_init() != 0)
    {
        printf("GPIO init failed\n");
        return -1;
    }

    // Configure LED
    if (ocre_gpio_configure(LED0_PORT, LED0, OCRE_GPIO_DIR_OUTPUT) != 0)
    {
        printf("LED config failed\n");
        return -1;
    }

    // Configure Button
    if (ocre_gpio_configure(BUTTON_PORT, BUTTON_0, OCRE_GPIO_DIR_INPUT) != 0)
    {
        printf("Button config failed\n");
        return -1;
    }
    if (ocre_gpio_register_callback(BUTTON_PORT, BUTTON_0) != 0)
    {
        printf("Button callback registration failed\n");
        return -1;
    }

    // Manual GPIO test
    printf("Testing LED: Setting ON\n");
    if (ocre_gpio_pin_set(LED0_PORT, LED0, OCRE_GPIO_PIN_RESET) != 0) // Active-low ON
        printf("Failed to set LED ON\n");
    ocre_sleep(1000);
    printf("Testing LED: Setting OFF\n");
    if (ocre_gpio_pin_set(LED0_PORT, LED0, OCRE_GPIO_PIN_SET) != 0) // Active-low OFF
        printf("Failed to set LED OFF\n");
    ocre_sleep(1000);

    // Register dispatchers (still needed for WASM exports)
    if (ocre_register_dispatcher(OCRE_RESOURCE_TYPE_TIMER, "timer_callback") != 0)
    {
        printf("Failed to register timer dispatcher\n");
        return -1;
    }
    if (ocre_register_dispatcher(OCRE_RESOURCE_TYPE_GPIO, "gpio_callback") != 0)
    {
        printf("Failed to register GPIO dispatcher\n");
        return -1;
    }

    // =============================================================================
    // REGISTER YOUR CUSTOM CALLBACK FUNCTIONS
    // =============================================================================

    // Register timer callback - you can use any function here!
    if (ocre_register_timer_callback(timer_id, my_timer_function) != 0)
    {
        printf("Failed to register timer callback function\n");
        return -1;
    }

    // Register GPIO callback - you can use any function here!
    if (ocre_register_gpio_callback(BUTTON_0, BUTTON_PORT, my_button_function) != 0)
    {
        printf("Failed to register GPIO callback function\n");
        return -1;
    }

    // You could also register different functions:
    // ocre_register_timer_callback(timer_id, my_other_function);
    // ocre_register_gpio_callback(BUTTON_0, my_other_function);

    // Create and start timer
    if (ocre_timer_create(timer_id) != 0)
    {
        printf("Timer creation failed\n");
        return -1;
    }
    printf("Timer created. ID: %d, Interval: %dms\n", timer_id, interval_ms);

    if (ocre_timer_start(timer_id, interval_ms, is_periodic) != 0)
    {
        printf("Timer start failed\n");
        return -1;
    }

    printf("Starting event loop...\n");
    while (1)
    {
        ocre_poll_events(); // Now uses the generic system internally
        ocre_sleep(10);
    }

    printf("Blinky exiting.\n");
    return 0;
}
