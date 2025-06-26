/*
 * Copyright (C) 2025 Atym Incorporated. All rights reserved.
 * Button Controlled LED Blinky Example
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <ocre_api.h>

#define LED0_PORT 7
#define LED0 7
#define BUTTON_PORT 2
#define BUTTON_PIN 13

static bool blinky_active = false;
static bool led_state = false;
static int blink_count = 0;
static bool button_pressed = false;

// Timer callback function for LED blinking
static void timer_blink_function(void)
{
    if (!blinky_active)
        return;

    printf("Timer triggered - blinking LED!\n");

    // Active-low: RESET (low) = ON, SET (high) = OFF
    int ret = led_state ? ocre_gpio_pin_set(LED0_PORT, LED0, OCRE_GPIO_PIN_RESET) // ON
                        : ocre_gpio_pin_set(LED0_PORT, LED0, OCRE_GPIO_PIN_SET);  // OFF

    if (ret != 0)
    {
        printf("Failed to set LED: %d\n", ret);
    }
    else
    {
        printf("LED state set to %s (logical %d, count %d)\n",
               led_state ? "ON" : "OFF", led_state, ++blink_count);
    }
    led_state = !led_state;
}

// GPIO callback function for button press
static void button_callback_function(void)
{
    // Read button state
    ocre_gpio_pin_state_t button_state = ocre_gpio_pin_get(BUTTON_PORT, BUTTON_PIN);

    // Detect button press (assuming active low button)
    if (button_state == OCRE_GPIO_PIN_RESET && !button_pressed)
    {
        button_pressed = true;

        if (!blinky_active)
        {
            printf("Button pressed - starting blinky!\n");
            blinky_active = true;

            // Blink 3 times quickly to show it works
            for (int i = 0; i < 3; i++)
            {
                ocre_gpio_pin_set(LED0_PORT, LED0, OCRE_GPIO_PIN_RESET); // ON
                printf("Init blink %d - LED ON\n", i + 1);
                ocre_sleep(200);
                ocre_gpio_pin_set(LED0_PORT, LED0, OCRE_GPIO_PIN_SET); // OFF
                printf("Init blink %d - LED OFF\n", i + 1);
                ocre_sleep(200);
            }

            // Start regular blinking
            ocre_timer_start(1, 1000, true);
        }
        else
        {
            printf("Button pressed - stopping blinky!\n");
            blinky_active = false;
            ocre_timer_stop(1);

            // Turn off LED
            ocre_gpio_pin_set(LED0_PORT, LED0, OCRE_GPIO_PIN_SET); // OFF
            led_state = false;
            printf("LED turned OFF - blinky stopped\n");
        }
    }
    else if (button_state == OCRE_GPIO_PIN_SET)
    {
        button_pressed = false; // Button released
    }
}

int main(void)
{
    const int timer_id = 1;

    printf("=== Button Controlled LED Blinky Example ===\n");
    printf("Press button to start blinky!\n");

    // Initialize GPIO
    if (ocre_gpio_init() != 0)
    {
        printf("GPIO init failed\n");
        return -1;
    }

    // Configure LED as output
    if (ocre_gpio_configure(LED0_PORT, LED0, OCRE_GPIO_DIR_OUTPUT) != 0)
    {
        printf("LED config failed\n");
        return -1;
    }

    // Configure button as input
    if (ocre_gpio_configure(BUTTON_PORT, BUTTON_PIN, OCRE_GPIO_DIR_INPUT) != 0)
    {
        printf("Button config failed\n");
        return -1;
    }

    // Register dispatchers
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

    // Register callbacks
    if (ocre_register_timer_callback(timer_id, timer_blink_function) != 0)
    {
        printf("Failed to register timer callback function\n");
        return -1;
    }

    if (ocre_register_gpio_callback(BUTTON_PIN, BUTTON_PORT, button_callback_function) != 0)
    {
        printf("Failed to register GPIO callback function\n");
        return -1;
    }

    // Create timer (but don't start it yet)
    if (ocre_timer_create(timer_id) != 0)
    {
        printf("Timer creation failed\n");
        return -1;
    }

    // Register GPIO callback for button
    if (ocre_gpio_register_callback(BUTTON_PORT, BUTTON_PIN) != 0)
    {
        printf("Failed to register button callback\n");
        return -1;
    }

    printf("System ready. Press button on Port %d, Pin %d to start/stop blinking.\n",
           BUTTON_PORT, BUTTON_PIN);

    while (1)
    {
        ocre_poll_events();
        ocre_sleep(10);
    }

    printf("Button Controlled Blinky exiting.\n");
    return 0;
}