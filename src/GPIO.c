/* Created by Lefucjusz, 02.09.2022, Gdańsk */
#include "GPIO.h"

#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

/* See https://www.raspberrypi.org/app/uploads/2012/02/BCM2835-ARM-Peripherals.pdf p.90 for source of the defines and magic equations in functions */
#define BCM2835_PERIPH_BASE 0x20000000
#define BCM2835_GPIO_OFFSET 0x200000
#define BCM2835_GPIO_BASE (BCM2835_PERIPH_BASE + BCM2835_GPIO_OFFSET)

#define BCM2835_GPIO_BYTES_PER_REG 4 // Each register has 4 bytes
#define BCM2835_GPIO_BITS_PER_REG (BCM2835_GPIO_BYTES_PER_REG * 8) // 8 bits per byte
#define BCM2835_GPIO_REGS_SIZE 0xB4 // Bytes

#define BCM2835_FUNC_SEL_REG_0_OFFSET 0 // Offset of the function select register 0
#define BCM2835_OUT_SET_REG_0_OFFSET 7 // Offset of the output set register 0
#define BCM2835_OUT_SET_REG_1_OFFSET 8 // Offset of the output set register 1
#define BCM2835_OUT_CLR_REG_0_OFFSET 10 // Offset of the output clear register 0
#define BCM2835_OUT_CLR_REG_1_OFFSET 11 // Offset of the output clear register 1

#define BCM2835_PINS_PER_OUT_REG BCM2835_GPIO_BITS_PER_REG // How many pins are controlled by one output register

#define BCM2835_FUNC_SEL_BITS_PER_PIN 3 // Bits controlling pin function per pin
#define BCM2835_PINS_PER_FUNC_SEL_REG 10  // Each function select register controls 10 pins, 0-9 in register 0, 10-19 in register 1 etc. Register 5 controls only 4 pins, 50-53

static const char* const mem_path = "/dev/mem";
static void* gpio_map = NULL;

void* gpio_init(void) {
    int mem_fd = open(mem_path, O_RDWR | O_SYNC);

    if(mem_fd < 0) {
        return NULL;
    }

    gpio_map = mmap(NULL, BCM2835_GPIO_REGS_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, BCM2835_GPIO_BASE);
    close(mem_fd);

    if(gpio_map == MAP_FAILED) {
        return NULL;
    }

    return gpio_map;
}

void gpio_set_pin_dir(volatile unsigned* const gpio, gpio_pin_t pin, gpio_pin_dir_t dir) {
    unsigned func_sel_reg = ((unsigned)pin) / BCM2835_PINS_PER_FUNC_SEL_REG;
    unsigned pin_in_reg = ((unsigned)pin) % BCM2835_PINS_PER_FUNC_SEL_REG;
    unsigned reg_offset = func_sel_reg + BCM2835_FUNC_SEL_REG_0_OFFSET; // Register offset
    unsigned pin_mask_offset = pin_in_reg * BCM2835_FUNC_SEL_BITS_PER_PIN; // Pin mask offset inside register
    
    volatile unsigned* const func_sel_reg_addr = gpio + reg_offset;

    switch(dir) {
        case GPIO_INPUT:
            *func_sel_reg_addr &= ~(0b111 << pin_mask_offset);
            break;
        case GPIO_OUTPUT:
            *func_sel_reg_addr &= ~(0b111 << pin_mask_offset); // First clear all 3 bits
            *func_sel_reg_addr |= (0b001 << pin_mask_offset); // Then set LSB
            break; 
        default:
            break;
    }
}

void gpio_set_pin_state(volatile unsigned* const gpio, gpio_pin_t pin, gpio_pin_state_t state) {
    unsigned out_reg = ((unsigned)pin) / BCM2835_PINS_PER_OUT_REG;
    unsigned pin_in_reg = ((unsigned)pin) % BCM2835_PINS_PER_OUT_REG;
    volatile unsigned* out_reg_addr = NULL;

    switch(state) {
        case GPIO_LOW:
            if(out_reg == 0) {
                out_reg_addr = gpio + BCM2835_OUT_CLR_REG_0_OFFSET;
            } else {
                out_reg_addr = gpio + BCM2835_OUT_CLR_REG_1_OFFSET;
            }
            break;

        case GPIO_HIGH:
            if(out_reg == 0) {
                out_reg_addr = gpio + BCM2835_OUT_SET_REG_0_OFFSET;
            } else {
                out_reg_addr = gpio + BCM2835_OUT_SET_REG_1_OFFSET;
            }
            break;
        default:
            break;
    }

    *out_reg_addr = (1 << pin_in_reg);
}

int gpio_deinit(void) {
    int ret = munmap(gpio_map, BCM2835_GPIO_REGS_SIZE);
    return ret;
}
