/*
 * GPIO.h
 *
 *  Created on: 02.09.2022
 *      Author: Lefucjusz
 */

#ifndef __GPIO_H__
#define __GPIO_H__

typedef volatile unsigned int* gpio_t;

/* See https://www.raspberry-pi-geek.com/howto/GPIO-Pinout-Rasp-Pi-1-Rev1-and-Rev2 for mapping */
typedef enum {
    GPIO_PIN_3 = 0,
    GPIO_PIN_5 = 1,
    GPIO_PIN_7 = 4,
    GPIO_PIN_8 = 14,
    GPIO_PIN_10 = 15,
    GPIO_PIN_11 = 17,
    GPIO_PIN_12 = 18,
    GPIO_PIN_13 = 21, 
    GPIO_PIN_15 = 22,
    GPIO_PIN_16 = 23,
    GPIO_PIN_18 = 24,
    GPIO_PIN_19 = 10,
    GPIO_PIN_21 = 9,
    GPIO_PIN_22 = 25, 
    GPIO_PIN_23 = 11,
    GPIO_PIN_24 = 8,
    GPIO_PIN_26 = 7
} gpio_pin_t;

typedef enum {
    GPIO_INPUT = 0,
    GPIO_OUTPUT
} gpio_pin_dir_t;

typedef enum {
    GPIO_LOW = 0,
    GPIO_HIGH
} gpio_pin_state_t;

/**
 * @brief Initializes Raspberry PI GPIO driver
 * @return GPIO driver handle if success, NULL if failure
 */
gpio_t gpio_init(void);

/**
 * @brief Sets GPIO pin direction
 * @param gpio Raspberry PI GPIO driver handle
 * @param pin Pin to set direction
 * @param dir Direction - input or output
 */
void gpio_set_pin_dir(gpio_t gpio, gpio_pin_t pin, gpio_pin_dir_t dir);

/**
 * @brief Sets GPIO pin state
 * @param gpio Raspberry PI GPIO driver handle
 * @param pin Pin to set state
 * @param dir State - low or high
 */
void gpio_set_pin_state(gpio_t gpio, gpio_pin_t pin, gpio_pin_state_t state);

/**
 * @brief Deinitializes Raspberry PI GPIO driver
 * @return 0 if success, -1 if failure
 */
int gpio_deinit(void);

#endif

